// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"

#include <spot/priv/robin_hood.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/genem.hh>
#include <spot/twaalgos/parity.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/toparity.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/zlktree.hh>

#include <algorithm>
#include <deque>

namespace std
{
  template <typename T>
  inline void hash_combine(size_t &seed, T const &v)
  {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  template <typename T>
  struct hash<vector<T>>
  {
    typedef vector<T> argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const &in) const
    {
      size_t size = in.size();
      size_t seed = 0;
      for (size_t i = 0; i < size; i++)
        // Combine the hash of the current vector with the hashes of the
        // previous ones
        hash_combine(seed, in[i]);
      return seed;
    }
  };
}

namespace spot
{
  inline void
  assign_color(acc_cond::mark_t &mark, unsigned col)
  {
    if (col < SPOT_MAX_ACCSETS)
      mark.set(col);
    else
      acc_cond::mark_t{SPOT_MAX_ACCSETS};
  }

  // Describes if we want to test if it is a Büchi, co-Büchi,… type automaton.
  enum cond_kind
  {
    BUCHI,
    CO_BUCHI,
    // A parity condition with a Inf as outermost term
    INF_PARITY,
    // A parity condition with a Fin as outermost term
    FIN_PARITY
  };

  // This enum describes the status of an edge
  enum edge_status
  {
    NOT_MARKED,
    MARKED,
    IMPOSSIBLE,
    LINK_SCC
  };

  static bool
  cond_type_main_aux(const twa_graph_ptr &aut, const cond_kind kind,
                     const bool need_equivalent,
                     std::vector<edge_status> &status,
                     std::vector<acc_cond::mark_t> &res_colors,
                     acc_cond &new_cond, bool &was_able_to_color,
                     unsigned max_col)
  {
    auto& ev = aut->edge_vector();
    const auto ev_size = ev.size();
    const auto aut_init = aut->get_init_state_number();
    was_able_to_color = false;
    status = std::vector<edge_status>(ev_size, NOT_MARKED);
    res_colors = std::vector<acc_cond::mark_t>(ev_size);
    // Used by accepting_transitions_scc.
    auto keep = std::unique_ptr<bitvect>(make_bitvect(ev_size));
    keep->set_all();

    // Number of edges colored by the procedure, used to test equivalence for
    // parity
    unsigned nb_colored = 0;

    // We need to say that a transition between 2 SCC doesn't have to get a
    // color.
    scc_info si(aut, aut_init, nullptr, nullptr, scc_info_options::NONE);
    status[0] = LINK_SCC;
    if (si.scc_count() > 1)
    {
      for (unsigned edge_number = 1; edge_number < ev_size; ++edge_number)
      {
        auto& e = ev[edge_number];
        if (si.scc_of(e.src) != si.scc_of(e.dst))
        {
          status[edge_number] = LINK_SCC;
          ++nb_colored;
        }
      }
    }

    // If we need to convert to (co-)Büchi, we have to search one accepting
    // set. With parity there is no limit.
    bool want_parity = kind == cond_kind::FIN_PARITY ||
                       kind == cond_kind::INF_PARITY;
    unsigned max_iter = want_parity ? -1U : 1;

    unsigned color = max_col;
    // Do we want always accepting transitions?
    // Don't consider CO_BUCHI as it is done by Büchi
    bool search_inf = kind != cond_kind::FIN_PARITY;

    using filter_data_t = std::pair<const_twa_graph_ptr,
                                    std::vector<edge_status> &>;

    scc_info::edge_filter filter =
        [](const twa_graph::edge_storage_t &t, unsigned, void *data)
        -> scc_info::edge_filter_choice
    {
      auto &d = *static_cast<filter_data_t *>(data);
      // We only keep transitions that can be marked
      if (d.second[d.first->edge_number(t)] == NOT_MARKED)
        return scc_info::edge_filter_choice::keep;
      else
        return scc_info::edge_filter_choice::cut;
    };
    std::vector<bool> not_decidable_transitions(ev_size, false);
    auto aut_acc = aut->get_acceptance();
    auto aut_acc_comp = aut_acc.complement();
    for (unsigned iter = 0; iter < max_iter; ++iter)
    {
      // Share the code with Büchi-type
      if (kind == CO_BUCHI)
        std::swap(aut_acc, aut_acc_comp);
      std::fill(not_decidable_transitions.begin(),
                not_decidable_transitions.end(), false);
      auto cond = acc_cond(search_inf ? aut_acc_comp : aut_acc);
      auto filter_data = filter_data_t{aut, status};
      scc_info si(aut, aut_init, filter, &filter_data,
                  scc_info_options::TRACK_STATES);
      if (search_inf)
        si.determine_unknown_acceptance();
      bool worked = false;
      unsigned ssc_size = si.scc_count();
      for (unsigned scc = 0; scc < ssc_size; ++scc)
      {
        // scc_info can detect that we will not be able to find an
        // accepting cycle.
        if ((search_inf && si.is_accepting_scc(scc)) || !search_inf)
        {
          accepting_transitions_scc(si, scc, cond, {},
                                    not_decidable_transitions, *keep);
          for (auto &e : si.inner_edges_of(scc))
          {
            auto edge_number = aut->edge_number(e);
            if (!not_decidable_transitions[edge_number])
            {
              assert(!res_colors[edge_number]);
              if (color != -1U)
                assign_color(res_colors[edge_number], color);
              was_able_to_color = true;
              status[edge_number] = MARKED;
              ++nb_colored;
              keep->clear(edge_number);
              worked = true;
            }
          }
        }
      }

      if (color-- == -1U)
        break;
      search_inf = !search_inf;
      // If we were not able to add color, we have to add status 2 to
      // remaining transitions.
      if (!worked && !need_equivalent)
      {
        std::replace(status.begin(), status.end(), NOT_MARKED, IMPOSSIBLE);
        break;
      }
    }

    acc_cond::acc_code new_code;
    switch (kind)
    {
    case cond_kind::BUCHI:
      new_code = acc_cond::acc_code::buchi();
      break;
    case cond_kind::CO_BUCHI:
      new_code = acc_cond::acc_code::cobuchi();
      break;
    case cond_kind::INF_PARITY:
    case cond_kind::FIN_PARITY:
      new_code = acc_cond::acc_code::parity_max(
          kind == cond_kind::INF_PARITY, SPOT_MAX_ACCSETS);
      break;
    }

    new_cond = acc_cond(new_code);

    // We check parity
    if (need_equivalent)
    {
      // For parity, it's equivalent if every transition has a color
      // (status 1) or links 2 SCCs.
      if (kind == cond_kind::INF_PARITY || kind == cond_kind::FIN_PARITY)
        return nb_colored == ev_size - 1;
      else
      {
        // For Büchi, we remove the transitions that have {0} in the
        // result from aut and if there is an accepting cycle, res is not
        // equivalent to aut.
        // For co-Büchi, it's the same but we don't want to find a
        // rejecting cycle.
        using filter_data_t = std::pair<const_twa_graph_ptr, bitvect &>;

        scc_info::edge_filter filter =
            [](const twa_graph::edge_storage_t &t, unsigned, void *data)
            -> scc_info::edge_filter_choice
        {
          auto &d = *static_cast<filter_data_t *>(data);
          if (d.second.get(d.first->edge_number(t)))
            return scc_info::edge_filter_choice::keep;
          else
            return scc_info::edge_filter_choice::cut;
        };

        if (kind == CO_BUCHI)
          aut->set_acceptance(acc_cond(aut_acc));

        filter_data_t filter_data = {aut, *keep};
        scc_info si(aut, aut_init, filter, &filter_data);
        si.determine_unknown_acceptance();
        const auto num_scc = si.scc_count();
        for (unsigned scc = 0; scc < num_scc; ++scc)
          if (si.is_accepting_scc(scc))
          {
            if (kind == CO_BUCHI)
              aut->set_acceptance(acc_cond(aut_acc_comp));
            return false;
          }
        if (kind == CO_BUCHI)
          aut->set_acceptance(acc_cond(aut_acc_comp));
      }
    }

    return true;
  }

  static twa_graph_ptr
  cond_type_main(const twa_graph_ptr &aut, const cond_kind kind,
                 bool &was_able_to_color, unsigned max_color)
  {
    std::vector<acc_cond::mark_t> res_colors;
    std::vector<edge_status> status;
    acc_cond new_cond;
    if (cond_type_main_aux(aut, kind, true, status, res_colors, new_cond,
                            was_able_to_color, max_color))
    {
      auto res = make_twa_graph(aut, twa::prop_set::all());
      // GCC 7 warns about potential null pointer dereference.
      SPOT_ASSUME(res);
      auto &res_vector = res->edge_vector();
      unsigned rv_size = res_vector.size();
      for (unsigned i = 1; i < rv_size; ++i)
        res_vector[i].acc = res_colors[i];
      res->set_acceptance(new_cond);
      return res;
    }
    return nullptr;
  }

  twa_graph_ptr
  parity_type_to_parity(const twa_graph_ptr &aut)
  {
    bool odd_cond, max_cond;
    bool parit = aut->acc().is_parity(max_cond, odd_cond);
    // If it is parity, we just copy
    if (parit)
    {
      if (!max_cond)
        return change_parity(aut, parity_kind_max, parity_style_any);
      auto res = make_twa_graph(aut, twa::prop_set::all());
      res->copy_acceptance_of(aut);
      return res;
    }
    bool was_able_to_color;
    // If the automaton is parity-type with a condition that has Inf as
    // outermost term
    auto res = cond_type_main(aut, cond_kind::INF_PARITY,
      was_able_to_color, SPOT_MAX_ACCSETS - 1);

    // If it was impossible to find an accepting edge, it is perhaps possible
    // to find a rejecting transition
    if (res == nullptr && !was_able_to_color)
      res = cond_type_main(aut, cond_kind::FIN_PARITY,
        was_able_to_color, SPOT_MAX_ACCSETS - 1);
    if (res)
    {
      res->prop_state_acc(false);
      reduce_parity_here(res);
    }
    return res;
  }

  twa_graph_ptr
  buchi_type_to_buchi(const twa_graph_ptr &aut)
  {
    bool useless;
    return cond_type_main(aut, cond_kind::BUCHI, useless, 0);
  }

  twa_graph_ptr
  co_buchi_type_to_co_buchi(const twa_graph_ptr &aut)
  {
    bool useless;
    return cond_type_main(aut, cond_kind::CO_BUCHI, useless, 0);
  }

// New version for paritizing

// data type used in a memory for CAR and IAR.
// TAR is a particular case
#if MAX_ACCSETS < UCHAR_MAX
  using memory_type = unsigned char;
  #define MAX_MEM_ELEM UCHAR_MAX
#elif MAX_ACCSETS < USHRT_MAX
  using memory_type = unsigned short;
  #define MAX_MEM_ELEM USHRT_MAX
#else
  using memory_type = unsigned;
  #define MAX_MEM_ELEM UINT_MAX
#endif

  template<typename T>
  using memory = std::vector<T>;

  // Maps a state of the automaton to a parity_state
  class state_2_lar
  {
    public:
    // If to_parity wants to find the newest or the oldest or both, we
    // adapt the algorithms
    enum memory_order
    {
      ONLY_NEWEST,
      ONLY_OLDEST,
      BOTH
    };

    class node
    {
    public:
      // Color that lead to this node
      memory_type color_;
      // For a state in states_, any child can be taken. While a unique state
      // could be used when we search an existing state, here we have
      // to consider opt_.search_ex = False, opt_.use_last_post_process = True.
      // This configuration can lead to 2 states in the same node. For example
      // if we add [0 1 | 2 3] and [0 1 | 3 2] where '|' says which part of the
      // memory can be reordered (right).
      std::vector<unsigned> states_;
      std::vector<node *> children_;
      // A timer used to detect which child is the oldest
      unsigned timer_;

      node() : node(MAX_MEM_ELEM, -1U)
      {
      }

      node(memory_type val, unsigned timer) : color_(val), timer_(timer)
      {
      }

      ~node()
      {
        for (auto c : children_)
          delete c;
      }
    };

    std::vector<node *> nodes_;
    memory_order order_;
    unsigned timer_;

    state_2_lar() : timer_(0)
    {
    }

    void
    init(unsigned nb_states, memory_order order)
    {
      order_ = order;
      nodes_.reserve(nb_states);
      for (unsigned i = 0; i < nb_states; ++i)
        nodes_.push_back(new node());
    }

    ~state_2_lar()
    {
      for (auto x : nodes_)
        delete x;
    }

    void
    add_new_path(unsigned state, const memory<memory_type> &vals,
                 unsigned res_state, unsigned nb_seen)
    {
      ++timer_;
      node *current = nodes_[state];
      // Position in vals
      int pos = vals.size() - 1;
      while (true)
      {
        if (pos == (int)(nb_seen - 1))
          current->states_.push_back(res_state);
        if (pos == -1)
          break;
        const unsigned current_val = vals[pos];
        auto child = std::find_if(current->children_.begin(),
                                  current->children_.end(),
                                  [&](const auto &child) constexpr
                                  { return child->color_ == current_val; });
        // If we don't have a child with the corresponding color…
        if (child == current->children_.end())
        {
          auto nn = new node(current_val, timer_);
          current->children_.push_back(nn);
          current = nn;
        }
        else
        {
          // If get_compatible_state wants the most recent
          // (opt_.use_last or opt_.use_last_post_process), we help this
          // function by moving this node to the last position.
          // Otherwise the oldest leaf will be reachable from the first child.
          // If we have use_last = false and use_last_post_process = true,
          // we need to access to the oldest and newest child. As the tree is
          // smallest when we want to access to the oldest value, we continue
          // to move the value to the last position and compute the oldest
          // child in get_compatible_state.
          if (order_ != memory_order::ONLY_OLDEST)
          {
            std::iter_swap(child, current->children_.end() - 1);
            current = current->children_.back();
          }
          else
            current = *child;
        }
        --pos;
      }
    }

    unsigned
    get_compatible_state(unsigned state, const memory<memory_type> &m,
                        unsigned seen_nb,
                         bool use_last) const
    {
      int pos = m.size() - 1;
      unsigned res = -1U;
      node *current = nodes_[state];
      while (true)
      {
        const auto &current_states = current->states_;
        if (!current_states.empty())
          res = use_last ? current_states.back() : current_states.front();

        const auto &current_children = current->children_;
        if (current_children.empty())
        {
          assert(current->color_ == MAX_MEM_ELEM || pos == -1);
          return res;
        }
        // If we are in the part of the memory where the order does not matter,
        // we just take the oldest/newest state.
        if (pos < (int)seen_nb)
        {
          if (order_ == BOTH)
          {
            if (!use_last)
              current = *std::min_element(
                  current_children.begin(), current_children.end(),
                  [](const auto &x, const auto &y) constexpr
                    { return x->timer_ < y->timer_; });
            else
              current = current_children.back();
          }
          else
          {
            // add_new_path constructed the tree such that the oldest/newest
            // leaf is reachable from the first child.
            current = use_last ? current_children.back()
                               : current_children.front();
          }
        }
        else
        {
          auto current_val = m[pos];
          auto ch = std::find_if(
              current_children.begin(), current_children.end(),
              [&](const auto &x) constexpr
                { return x->color_ == current_val; });
          if (ch != current_children.end())
            current = *ch;
          else
            return -1U;
        }
        --pos;
      }
    }
  };

  class to_parity_generator
  {
  private:
    class relation
    {
    public:
      // Size of the matrix
      unsigned size_;
      // A line/column is indexed by a partial memory
      const std::vector<memory<memory_type>> labels_;
      // Matrix such that vals_[x][y] = ⊤ ⇔ vals_[x] > vals_[y]
      std::vector<bool> vals_;

      inline bool
      at(unsigned i, unsigned j) const
      {
        return vals_.at(i * size_ + j);
      }

      inline void
      set(unsigned i, unsigned j, bool val)
      {
        vals_[i * size_ + j] = val;
      }

      // Test if m1 ⊆ m2
      bool is_included(memory<memory_type> m1, memory<memory_type> m2)
      {
        if (m1.size() > m2.size())
          return false;
        assert(std::is_sorted(m1.begin(), m1.end()));
        assert(std::is_sorted(m2.begin(), m2.end()));
        memory<memory_type> diff;
        std::set_union(m1.begin(), m1.end(), m2.begin(), m2.end(),
                        std::inserter(diff, diff.begin()));
        return diff.size() == m2.size();
      }

      // Supposes that there is no duplicates.
      relation(std::vector<memory<memory_type>> &labels)
          : size_(labels.size()), labels_(labels)
      {
        unsigned long long size_vals;
        if (__builtin_umulll_overflow(size_, size_, &size_vals))
          throw std::bad_alloc();
        vals_ = std::vector<bool>(size_vals);
        for (unsigned i = 0; i < size_; ++i)
          for (unsigned j = 0; j < size_; ++j)
            // We cannot have vals_[i] > vals_[j] and vals_[j] > vals_[i]
            if (!at(j, i))
              set(i, j, (i != j && is_included(labels_[j], labels_[i])));
        // Remove x > z if ∃y s.t. x > y > z
        simplify_relation();
      }

      // Apply a transitive reduction
      void
      simplify_relation()
      {
        for (unsigned j = 0; j < size_; ++j)
          for (unsigned i = 0; i < size_; ++i)
            if (at(i, j))
              for (unsigned k = 0; k < size_; ++k)
                if (at(j, k))
                  set(i, k, false);
      }

      template<typename T>
      void
      add_to_res_(const memory<T> &current,
                  const memory<T> &other,
                  memory<T> &result)
      {
        assert(std::is_sorted(current.begin(), current.end()));
        assert(std::is_sorted(other.begin(), other.end()));
        std::set_difference(current.begin(), current.end(),
                            other.begin(), other.end(),
                            std::inserter(result, result.end()));
      }

      // Gives a compatible ordered partial memory for the partial memory
      // partial_mem.
      memory<memory_type>
      find_order(const memory<memory_type> &partial_mem)
      {
        // Now if we want to find an order, we start from the line
        // that contains partial_mem in the matrix, we find a more restrictive
        // order and add the value that are used in partial_mem but not in this
        // "child" value.
        // The call to simplify_relation implies that we are sure we have
        // used the longest possible path.
        memory<memory_type> result;
        auto elem = std::find(labels_.begin(), labels_.end(), partial_mem);
        assert(elem != labels_.end());
        // Line that contains partial_mem
        unsigned i = std::distance(labels_.begin(), elem);
        while (true)
        {
          // The interval corresponding to the line i
          auto vals_i_begin = vals_.begin() + (i * size_);
          auto vals_i_end = vals_i_begin + size_;
          // End of line i
          auto child = std::find(vals_i_begin, vals_i_end, true);
          // If there is a more restrictive memory, we use this "child"
          if (child != vals_i_end)
          {
            unsigned child_pos = std::distance(vals_i_begin, child);
            add_to_res_(labels_[i], labels_[child_pos], result);
            i = child_pos;
          }
          // If there is no more restrictive memory, we just add the remaining
          // memory.
          else
          {
            add_to_res_(labels_[i], {}, result);
            break;
          }
        }
        // The order want that a value that is in the lowest value is a
        // the head.
        std::reverse(result.begin(), result.end());
        return result;
      }
    };

    class scc_info_to_parity
    {
    private:
      scc_info si_;

    public:
      scc_info_to_parity(const const_twa_graph_ptr aut,
                         const acc_cond::mark_t removed = {})
                          : si_(scc_and_mark_filter(aut, removed))
      {
      }

      scc_info_to_parity(const scc_info lower_si,
                         const std::shared_ptr<bitvect> keep)
          : si_(scc_and_mark_filter(lower_si, 0, acc_cond::mark_t{}, *keep),
                                    scc_info_options::NONE)
      {
      }

      std::vector<twa_graph_ptr>
      split_aut(acc_cond::mark_t mark = {})
      {
        auto aut = si_.get_aut();
        const auto num_scc = si_.scc_count();
        const unsigned aut_num_states = aut->num_states();
        std::vector<twa_graph_ptr> res(num_scc);
        std::vector<unsigned> aut_to_res;
        aut_to_res.reserve(aut_num_states);
        for (auto &g : res)
        {
          g = make_twa_graph(aut->get_dict());
          g->copy_ap_of(aut);
          g->copy_acceptance_of(aut);
          g->prop_copy(aut, {true, true, false, false, false, true});
          auto orig = new std::vector<unsigned>();
          g->set_named_prop("original-states", orig);
        }
        const auto tp_orig_aut =
          aut->get_named_prop<std::vector<unsigned>>("original-states");
        for (unsigned i = 0; i < aut_num_states; ++i)
        {
          unsigned scc_i = si_.scc_of(i);
          auto &g = res[scc_i];
          unsigned ns = g->new_state();
          unsigned ori = tp_orig_aut ? (*tp_orig_aut)[i] : i;
          auto pr = g->get_named_prop<std::vector<unsigned>>("original-states");
          pr->push_back(ori);
          aut_to_res.push_back(ns);
        }

        for (auto &e : aut->edges())
        {
          unsigned src_scc = si_.scc_of(e.src);
          unsigned dst_scc = si_.scc_of(e.dst);
          if (src_scc == dst_scc && !(e.acc & mark))
            res[src_scc]->new_edge(aut_to_res[e.src], aut_to_res[e.dst],
                                    e.cond, e.acc);
        }
        return res;
      }

      std::vector<twa_graph_ptr>
      split_aut(const std::shared_ptr<bitvect> &keep)
      {
        auto aut = si_.get_aut();
        const auto num_scc = si_.scc_count();
        const unsigned aut_num_states = aut->num_states();
        std::vector<twa_graph_ptr> res(num_scc);
        std::vector<unsigned> aut_to_res;
        aut_to_res.reserve(aut_num_states);
        for (auto &g : res)
        {
          g = make_twa_graph(aut->get_dict());
          g->copy_ap_of(aut);
          g->copy_acceptance_of(aut);
          g->prop_copy(aut, {true, true, false, false, false, true});
          auto orig = new std::vector<unsigned>();
          g->set_named_prop("original-states", orig);
        }
        const auto tp_orig_aut =
          aut->get_named_prop<std::vector<unsigned>>("original-states");
        for (unsigned i = 0; i < aut_num_states; ++i)
        {
          unsigned scc_i = si_.scc_of(i);
          auto &g = res[scc_i];
          unsigned ns = g->new_state();
          unsigned ori = tp_orig_aut ? (*tp_orig_aut)[i] : i;
          auto pr = g->get_named_prop<std::vector<unsigned>>("original-states");
          pr->push_back(ori);
          aut_to_res.push_back(ns);
        }

        const auto &ev = si_.get_aut()->edge_vector();
        auto ev_size = ev.size();
        for (unsigned i = 0; i < ev_size; ++i)
          if (keep->get(i))
          {
            auto &e = ev[i];
            unsigned scc_src = si_.scc_of(e.src);
            if (scc_src == si_.scc_of(e.dst))
              res[scc_src]->new_edge(aut_to_res[e.src], aut_to_res[e.dst],
                                      e.cond, e.acc);
          }
        return res;
      }

      unsigned scc_count()
      {
        return si_.scc_count();
      }

      unsigned scc_of(unsigned state)
      {
        return si_.scc_of(state);
      }
    };

    // Original automaton
    const const_twa_graph_ptr aut_;
    // Resulting parity automaton
    twa_graph_ptr res_;
    // options
    to_parity_options opt_;
    // nullptr if opt_.pretty_print is false
    std::vector<std::string> *names_ = nullptr;
    // original_states. Is propagated if the original automaton has
    // this named property
    std::vector<unsigned> *orig_ = nullptr;
    scc_info_to_parity si_;
    bool need_purge_ = false;
    // Tells if we are constructing a parity max odd
    bool is_odd_ = false;
    // min_color used in the automaton + 1 (result of max_set).
    unsigned min_color_used_ = -1U;
    unsigned max_color_scc_ = 0;
    unsigned max_color_used_ = 0;
    std::vector<unsigned> state_to_res_;
    std::vector<unsigned> res_to_aut_;
    // Map a state of aut_ to every copy of this state. Used by a recursive call
    // to to_parity by parity_prefix for example
    std::vector<std::vector<unsigned>> *state_to_nums_ = nullptr;
    unsigned algo_used_ = 0;

    enum algorithm
    {
      CAR = 1,
      IAR_RABIN = 1 << 1,
      IAR_STREETT = 1 << 2,
      TAR = 1 << 3,
      RABIN_TO_BUCHI = 1 << 4,
      STREETT_TO_COBUCHI = 1 << 5,
      PARITY_TYPE = 1 << 6,
      BUCHI_TYPE = 1 << 7,
      CO_BUCHI_TYPE = 1 << 8,
      PARITY_EQUIV = 1 << 9,
      PARITY_PREFIX = 1 << 10,
      PARITY_PREFIX_GENERAL = 1 << 11,
      GENERIC_EMPTINESS = 1 << 12,
      PARTIAL_DEGEN = 1 << 13,
      ACC_CLEAN = 1 << 14,
      NONE = 1 << 15
    };

    static std::string
    algorithm_to_str(const algorithm &algo)
    {
      switch (algo)
      {
      case CAR:
        return "CAR";
      case IAR_RABIN:
        return "IAR (Rabin)";
      case IAR_STREETT:
        return "IAR (Streett)";
      case TAR:
        return "TAR";
      case NONE:
        return "None";
      case BUCHI_TYPE:
        return "Büchi-type";
      case CO_BUCHI_TYPE:
        return "co-Büchi-type";
      case PARITY_TYPE:
        return "Parity-type";
      case PARITY_EQUIV:
        return "Parity equivalent";
      case GENERIC_EMPTINESS:
        return "Generic emptiness";
      case STREETT_TO_COBUCHI:
        return "Streett to co-Büchi";
      case RABIN_TO_BUCHI:
        return "Rabin to Büchi";
      case PARITY_PREFIX:
        return "Parity-prefix";
      case PARITY_PREFIX_GENERAL:
        return "Parity-prefix general";
      case PARTIAL_DEGEN:
        return "Partial degeneralization";
      case ACC_CLEAN:
        return "acceptance cleanup";
      }
      SPOT_UNREACHABLE();
    }

    template<typename T>
    struct to_parity_state
    {
      unsigned state;
      unsigned state_scc;
      memory<T> mem;

      to_parity_state(unsigned st, unsigned st_scc, memory<T> m) :
        state(st), state_scc(st_scc), mem(m)
      {}

      to_parity_state(const to_parity_state &) = default;
      to_parity_state(to_parity_state &&) noexcept = default;

      ~to_parity_state() noexcept = default;

      bool
      operator<(const to_parity_state &other) const
      {
        if (state < other.state)
          return true;
        if (state > other.state)
          return false;
        if (state_scc < other.state_scc)
          return true;
        if (state_scc > other.state_scc)
          return false;
        if (mem < other.mem)
          return true;
        return false;
      }

      std::string
      to_str(const algorithm &algo) const
      {
        std::stringstream s;
        s << state;
        // An empty memory does not mean that we don't use LAR. For example
        // if the condition is true. We don't display a useless memory.
        if (!mem.empty())
        {
          s << ",[";
          const char delim = ',';
          s << ((unsigned)mem[0]);
          auto mem_size = mem.size();
          for (unsigned i = 1; i < mem_size; ++i)
            s << delim << ((unsigned)mem[i]);
          s << ']';
        }
        s << ',' << algorithm_to_str(algo);
        return s.str();
      }

      bool operator==(const to_parity_state &other) const
      {
        return state == other.state && state_scc == other.state_scc
                                    && mem == other.mem;
      }
    };

    template<typename T>
    struct to_parity_hash
    {
      size_t operator()(to_parity_state<T> const &tp) const
      {
        size_t result = std::hash<memory<T>>{}(tp.mem);
        std::hash_combine<unsigned>(result, tp.state);
        std::hash_combine<unsigned>(result, tp.state_scc);
        return result;
      }
    };

    template<typename T>
    unsigned
    add_res_state(const algorithm &algo, const to_parity_state<T> &ps)
    {
      if (names_)
        names_->emplace_back(ps.to_str(algo));
      orig_->push_back(ps.state);
      auto res = res_->new_state();
      if (opt_.datas)
      {
        algo_used_ |= algo;
        ++opt_.datas->nb_states_created;
      }
      assert(ps.state < aut_->num_states());
      // state_to_res_ could be updated even if there is already a value.
      // However it would lead to a result close to BSCC.
      // So it is easier to show the influence of BSCC when the value is not
      // changed when there is already a value.
      if (state_to_res_[ps.state] == -1U)
        state_to_res_[ps.state] = res;
      if (state_to_nums_)
      {
        assert(ps.state < state_to_nums_->size());
        (*state_to_nums_)[ps.state].push_back(res);
      }
      res_to_aut_.push_back(ps.state);
      return res;
    }

    unsigned
    add_res_edge(unsigned res_src, unsigned res_dst,
                 const acc_cond::mark_t &mark, const bdd &cond,
                 const bool can_merge_edge = true,
                 robin_hood::unordered_map<long long, unsigned>*
                  edge_cache = nullptr)
    {
      // In a parity automaton we just need the maximal value
      unsigned simax = mark.max_set();

      const bool need_cache = edge_cache != nullptr && can_merge_edge;
      long long key = 0;
      if (need_cache)
      {
        constexpr auto unsignedsize = sizeof(unsigned) * 8;
        key = (long long)simax << unsignedsize | res_dst;
        auto cache_value = edge_cache->find(key);
        if (cache_value != edge_cache->end())
        {
          auto edge_index = cache_value->second;
          auto &existing_edge = res_->edge_vector()[edge_index];
          existing_edge.cond |= cond;
          return edge_index;
        }
      }

      auto simplified = mark ? acc_cond::mark_t{simax - 1}
                             : acc_cond::mark_t{};
      assert(res_src != -1U);
      assert(res_dst != -1U);

      max_color_scc_ = std::max(max_color_scc_, simax);
      min_color_used_ = std::min(min_color_used_, simax);
      max_color_used_ = std::max(max_color_used_, simax);

      auto new_edge_num = res_->new_edge(res_src, res_dst, cond, simplified);
      if (need_cache)
        edge_cache->emplace(std::make_pair(key, new_edge_num));

      if (opt_.datas)
        ++opt_.datas->nb_edges_created;
      return new_edge_num;
    }

    // copy
    using coloring_function =
        std::function<acc_cond::mark_t(const twa_graph::edge_storage_t &)>;

    void
    apply_copy_general(const const_twa_graph_ptr &sub_automaton,
                       const coloring_function &col_fun,
                       const algorithm &algo)
    {
      if (opt_.datas)
        algo_used_ |= algo;
      auto init_states =
        sub_automaton->get_named_prop<std::vector<unsigned>>("original-states");
      assert(init_states);
      std::vector<unsigned> state_2_res_local;
      auto sub_aut_ns = sub_automaton->num_states();
      state_2_res_local.reserve(sub_aut_ns);
      for (unsigned state = 0; state < sub_aut_ns; ++state)
      {
        to_parity_state<memory_type> ps = {(*init_states)[state], state, {}};
        state_2_res_local.push_back(add_res_state(algo, ps));
      }
      for (auto &e : sub_automaton->edges())
      {
        auto new_mark = col_fun(e);
        add_res_edge(state_2_res_local[e.src], state_2_res_local[e.dst],
                      new_mark, e.cond);
      }
    }

    // Case where one color is replaced by another.
    // new_colors is a vector such that new_colors[i + 1] = j means that the
    // color i is replaced by j. new_colors[0] is the value for an uncolored
    // edge.
    void
    apply_copy(const const_twa_graph_ptr &sub_aut,
               const std::vector<unsigned> &new_colors,
               const algorithm &algo)
    {
      auto col_fun = [&](const twa_graph::edge_storage_t &edge)
      {
        acc_cond::mark_t res{};
        for (auto c : edge.acc.sets())
        {
          auto new_col = new_colors[c + 1];
          if (new_col != -1U)
            assign_color(res, new_col);
        }
        if (!edge.acc && new_colors[0] != -1U)
          assign_color(res, new_colors[0]);
        return res;
      };
      apply_copy_general(sub_aut, col_fun, algo);
    }

    // Case where new_color is a function such that edge_vector[i] should
    // be colored by new_color[i].
    void
    apply_copy_edge_index(const const_twa_graph_ptr &sub_aut,
                          const std::vector<unsigned> &new_color,
                          const algorithm &algo)
    {
      auto col_fun = [&](const twa_graph::edge_storage_t &edge)
      {
        auto res = new_color[sub_aut->edge_number(edge)];
        if (res == -1U)
          return acc_cond::mark_t{};
        return acc_cond::mark_t{res};
      };
      apply_copy_general(sub_aut, col_fun, algo);
    }

    // Create a memory for the first state created by apply_lar.
    // If the algorithm is IAR, it also fills pairs_indices that
    // contains the indices of the pairs that can be moved to the head of
    // the memory.
    template <algorithm algo, class T>
    memory<T>
    initial_memory_of(const const_twa_graph_ptr &sub_aut,
                      const std::vector<acc_cond::rs_pair> &pairs,
                      std::vector<std::map<memory<T>, memory<T>>> &relations)
    {
      unsigned init_state = sub_aut->get_init_state_number();
      if constexpr (algo == algorithm::CAR)
      {
        unsigned max_set = sub_aut->get_acceptance().used_sets().max_set();
        memory<T> values(max_set);
        std::iota(values.begin(), values.end(), 0);
        if (opt_.force_order)
          apply_move_heuristic(init_state, values, max_set, relations);
        return values;
      }
      else if constexpr (algo == algorithm::TAR)
      {
        if (UINT_MAX < sub_aut->num_edges())
        {
          throw std::runtime_error("Too many edges for TAR");
        }
        const auto &ev = sub_aut->edge_vector();
        const auto ev_size = ev.size();
        memory<T> values(ev_size - 1);
        // 0 is not an edge number
        std::iota(values.begin(), values.end(), 1);
        if (opt_.force_order && sub_aut->num_states() > 1)
        {
          unsigned free_pos = 0;
          // If a transition goes to state, it is at the head of the memory.
          for (unsigned i = 1; i < ev_size; ++i)
            if (ev[i].dst == init_state)
            {
              std::swap(values[i - 1], values[free_pos]);
              ++free_pos;
            }
        }
        return values;
      }
      else
      {
        static_assert(algo == IAR_RABIN || algo == IAR_STREETT);
        memory<T> values(pairs.size());
        std::iota(values.begin(), values.end(), 0);
        if (opt_.force_order)
          apply_move_heuristic(init_state, values, values.size(), relations);
        return values;
      }
    }

    // LAR
    algorithm
    choose_lar(const acc_cond &scc_condition,
               std::vector<acc_cond::rs_pair> &pairs,
               const unsigned num_edges)
    {
      std::vector<acc_cond::rs_pair> pairs1, pairs2;
      bool is_rabin_like = scc_condition.is_rabin_like(pairs1);
      bool is_streett_like = scc_condition.is_streett_like(pairs2);
      // If we cannot apply IAR and TAR and CAR are not used
      if ((!(is_rabin_like || is_streett_like) || !opt_.iar)
        && !(opt_.car || opt_.tar))
        throw std::runtime_error("to_parity needs CAR or TAR to process "
                                  "a condition that is not a Rabin or Streett "
                                  "condition or if IAR is not enabled");
      remove_duplicates(pairs1);
      remove_duplicates(pairs2);
      unsigned num_col = scc_condition.num_sets();

      auto num_pairs1 = (opt_.iar && is_streett_like) ? pairs2.size() : -1UL;
      auto num_pairs2 = (opt_.iar && is_rabin_like)   ? pairs1.size() : -1UL;

      // In practice, if the number of pairs is bigger than the number of
      // colors, it will create a color greater than SPOT_MAX_ACCSETS, so
      // we don't consider that it is a Rabin condition.
      // In this case, if CAR or TAR is not used, it will throw a Runtime
      // Error.

      bool iar_overflow = false;
      if ((num_pairs1 > MAX_MEM_ELEM) && (num_pairs2 > MAX_MEM_ELEM))
      {
        num_pairs1 = num_pairs2 = -1U;
        iar_overflow = true;
      }

      const std::vector<unsigned long>
          number_elements =
              {
                  (opt_.iar && is_streett_like) ? pairs2.size() : -1UL,
                  (opt_.iar && is_rabin_like) ? pairs1.size() : -1UL,
                  opt_.car ? num_col : -1UL,
                  opt_.tar ? num_edges : -1UL};
      constexpr std::array<algorithm, 4> algos = {IAR_STREETT, IAR_RABIN, CAR,
                                                  TAR};
      int min_pos = std::distance(number_elements.begin(),
                                  std::min_element(number_elements.begin(),
                                                   number_elements.end()));

      if (number_elements[min_pos] == -1U && iar_overflow)
        throw std::runtime_error(
          "Too many Rabin/Streett pairs, try to increase SPOT_MAX_ACCSETS");
      algorithm algo = algos[min_pos];
      if (algo == IAR_RABIN)
        pairs = pairs1;
      else if (algo == IAR_STREETT)
        pairs = pairs2;
      return algo;
    }

    // Remove duplicates in pairs without changing the order.
    static void
    remove_duplicates(std::vector<acc_cond::rs_pair> &pairs)
    {
      std::vector<acc_cond::rs_pair> res;
      res.reserve(pairs.size());
      for (auto &elem : pairs)
        if (std::find(res.begin(), res.end(), elem) == res.end())
          res.emplace_back(elem);
      pairs = res;
    }

    template <algorithm algo>
    acc_cond::mark_t
    fin(const std::vector<acc_cond::rs_pair> &pairs, unsigned k)
    {
      static_assert(algo == IAR_RABIN || algo == IAR_STREETT);
      if constexpr (algo == IAR_RABIN)
        return pairs[k].fin;
      else
        return pairs[k].inf;
    }

    template <algorithm algo>
    acc_cond::mark_t
    inf(const std::vector<acc_cond::rs_pair> &pairs, unsigned k)
    {
      static_assert(algo == IAR_RABIN || algo == IAR_STREETT);
      if constexpr (algo == IAR_RABIN)
        return pairs[k].inf;
      else
        return pairs[k].fin;
    }

    template <algorithm algo>
    std::vector<std::map<memory<memory_type>, memory<memory_type>>>
    find_relations(const const_twa_graph_ptr &sub_aut,
                   const std::vector<acc_cond::rs_pair> &pairs,
                   const std::set<unsigned> &pairs_indices)
    {
      static_assert(algo == IAR_RABIN || algo == IAR_STREETT || algo == CAR);
      const unsigned sub_aut_num_states = sub_aut->num_states();
      // Set of memory elements that can be at the head of the memory for
      // a given state.
      std::vector<std::set<memory<memory_type>>> incomem(sub_aut_num_states);
      // Add a mark with all colors/pairs to deal with the order of the
      // original state
      if constexpr (algo == algorithm::CAR)
      {
        auto ms = sub_aut->get_acceptance().used_sets().max_set();
        memory<memory_type> m(ms);
        std::iota(m.begin(), m.end(), 0);
        incomem[sub_aut->get_init_state_number()].insert(std::move(m));
      }
      else if constexpr (algo == IAR_RABIN || algo == IAR_STREETT)
      {
        memory<memory_type> m(pairs_indices.begin(), pairs_indices.end());
        incomem[sub_aut->get_init_state_number()].insert(std::move(m));
      }

      for (auto &e : sub_aut->edges())
      {
        auto e_sets = e.acc.sets();
        if constexpr (algo == algorithm::CAR)
          incomem[e.dst].insert({e_sets.begin(), e_sets.end()});
        // IAR
        else
        {
          memory<memory_type> parti;
          for (unsigned k : pairs_indices)
            if (e.acc & fin<algo>(pairs, k))
              parti.push_back(k);
          incomem[e.dst].insert(parti);
        }
      }
      std::vector<std::map<memory<memory_type>, memory<memory_type>>> res;
      res.reserve(sub_aut_num_states);
      for (unsigned i = 0; i < sub_aut_num_states; ++i)
      {
        std::map<memory<memory_type>, memory<memory_type>> ma;
        // Memory incoming to state i.
        std::vector<memory<memory_type>> elem(incomem[i].begin(),
                                              incomem[i].end());
        relation rel(elem);
        for (auto &x : rel.labels_)
          ma.insert({x, rel.find_order(x)});
        res.emplace_back(ma);
      }
      return res;
    }

    void
    apply_move_heuristic(unsigned state, memory<memory_type> &m,
                         unsigned nb_seen,
                         std::vector<std::map<
                            memory<memory_type>,
                            memory<memory_type>>> &relations)
    {
      // If we move 0 or 1 color we cannot change the order
      if (nb_seen < 2)
        return;
      memory<memory_type> seen{m.begin(), m.begin() + nb_seen};
      const auto &new_prefix = relations[state][seen];

      unsigned new_prefix_size = new_prefix.size();
      for (unsigned i = 0; i < new_prefix_size; ++i)
        m[i] = new_prefix[i];
    }

    template <algorithm algo, class T>
    void
    find_new_memory(unsigned state, memory<T> &m, unsigned edge_number,
                    const acc_cond::mark_t &colors,
                    const std::vector<acc_cond::rs_pair> &pairs,
                    const std::set<unsigned> &pairs_indices,
                    unsigned &nb_seen,
                    unsigned &h,
                    std::vector<std::map<memory<T>, memory<T>>> &relations)
    {
      if constexpr (algo == TAR)
      {
        (void)state;
        auto pos = std::find(m.begin(), m.end(), edge_number);
        assert(pos != m.end());
        h = std::distance(m.begin(), pos);
        std::rotate(m.begin(), pos, pos + 1);
      }
      else if constexpr (algo == CAR)
      {
        (void)edge_number;
        for (auto k : colors.sets())
        {
          auto it = std::find(m.begin(), m.end(), k);
          // A color can exist in the automaton but not in the condition.
          if (it != m.end())
          {
            h = std::max(h, (unsigned)(it - m.begin()) + 1);
            std::rotate(m.begin(), it, it + 1);
            ++nb_seen;
          }
        }
        if (opt_.force_order)
        {
          // apply_move_heuristic needs an increasing list of values
          std::reverse(m.begin(), m.begin() + nb_seen);
          apply_move_heuristic(state, m, nb_seen, relations);
        }
      }
      else if constexpr (algo == IAR_RABIN || algo == IAR_STREETT)
      {
        (void)edge_number;
        for (auto k = pairs_indices.rbegin(); k != pairs_indices.rend(); ++k)
          if (colors & fin<algo>(pairs, *k))
          {
            ++nb_seen;
            auto it = std::find(m.begin(), m.end(), *k);
            assert(it != m.end());
            // move the pair in front of the permutation
            std::rotate(m.begin(), it, it + 1);
          }
        if (opt_.force_order)
        {
          // As with CAR, in relation the partial memory is sorted. That is
          // why the previous loop use a reverse iterator.
          assert(std::is_sorted(m.begin(), m.begin() + nb_seen));
          apply_move_heuristic(state, m, nb_seen, relations);
        }
      }
    }

    template <algorithm algo, typename T>
    void
    compute_new_color_lar(const const_twa_graph_ptr &sub_aut,
                          const memory<T> &current_mem,
                          const memory<T> &new_perm,
                          unsigned &h,
                          const acc_cond::mark_t &edge_colors,
                          acc_cond::mark_t &acc,
                          const std::vector<acc_cond::rs_pair> &pairs,
                          robin_hood::unordered_map<acc_cond::mark_t, bool>&
                            acc_cache)
    {
      // This function should not be called with algo ∉ [CAR, IAR, TAR].
      static_assert(algo == CAR || algo == IAR_RABIN || algo == IAR_STREETT
                                || algo == TAR);
      assert(!acc);
      auto sub_aut_cond = sub_aut->acc();
      if constexpr (algo == CAR)
      {
        acc_cond::mark_t m(new_perm.begin(), new_perm.begin() + h);
        auto cc = acc_cache.find(m);
        bool rej;
        if (cc != acc_cache.end())
          rej = cc->second;
        else
        {
          rej = !sub_aut_cond.accepting(m);
          acc_cache.insert({m, rej});
        }
        unsigned value = 2 * h + rej - 1;
        if (value != -1U)
          assign_color(acc, value);
        return;
      }
      else if constexpr (algo == TAR)
      {
        auto &edge_vector = sub_aut->edge_vector();
        acc_cond::mark_t acc_seen {};
        for (unsigned i = 0; i <= h; ++i)
          acc_seen |= edge_vector[new_perm[i]].acc;

        auto cc = acc_cache.find(acc_seen);
        bool rej;
        if (cc != acc_cache.end())
          rej = cc->second;
        else
        {
          rej = !sub_aut_cond.accepting(acc_seen);
          acc_cache.insert({acc_seen, rej});
        }

        unsigned acc_col = 2 * h + rej - 1;
        if (acc_col != -1U)
          assign_color(acc, acc_col);
      }
      else
      {
        // IAR_RABIN produces a parity max even condition. If res_
        // is parity max odd, we add 1 to a transition to produce a parity max
        // odd automaton.
        unsigned delta_acc = ((algo == IAR_RABIN) && is_odd_) - 1;

        unsigned maxint = -1U;
        for (int k = current_mem.size() - 1; k >= 0; --k)
        {
          unsigned pk = current_mem[k];

          if (!inf<algo>(pairs, pk) || (edge_colors
                                        & (pairs[pk].fin | pairs[pk].inf)))
          {
            maxint = k;
            break;
          }
        }

        unsigned value;
        if (maxint == -1U)
          value = delta_acc;
        else if (edge_colors & fin<algo>(pairs, current_mem[maxint]))
          value = 2 * maxint + 2 + delta_acc;
        else
          value = 2 * maxint + 1 + delta_acc;

        if (value != -1U)
          assign_color(acc, value);
      }
    }

    void
    change_to_odd()
    {
      if (is_odd_)
        return;
      is_odd_ = true;
      // We can reduce if we don't have an edge without color.
      bool can_reduce = (min_color_used_ != -1U) && (min_color_used_ != 0);
      int shift;

      if (can_reduce)
        shift = - (min_color_used_ | 1);
      else
        shift = 1;

      // If we cannot decrease and we already the the maximum color, we don't
      // have to try. Constructs a mark_t to avoid to make report_too_many_sets
      // public.
      if (!can_reduce && max_color_used_ + shift >= MAX_ACCSETS)
        acc_cond::mark_t {SPOT_MAX_ACCSETS};
      max_color_used_ += shift;
      if (min_color_used_ < -1U)
        min_color_used_ += shift;
      for (auto &e : res_->edges())
        {
          auto new_val = e.acc.max_set() - 1 + shift;
          if (new_val != -1U)
            e.acc = { new_val };
          else
            e.acc = {};
        }
    }

    template <algorithm algo, typename T>
    void
    apply_lar(twa_graph_ptr &sub_aut,
              const std::vector<acc_cond::rs_pair> &pairs)
    {
      if constexpr (algo != IAR_RABIN)
        change_to_odd();
      // avoids to call LAR if there is one color/pair/transition.
      // LAR can work with this kind of condition but some optimizations
      // like searching an existing state suppose that there is at least
      // one element.
      if ((algo == CAR && sub_aut->acc().num_sets() == 0)
        || ((algo == IAR_RABIN || algo == IAR_STREETT) && pairs.empty())
        || (algo == TAR && sub_aut->num_edges() == 0))
      {
        bool need_col = sub_aut->acc().is_t() != is_odd_;
        auto col_fun = [&](const twa_graph::edge_storage_t &)
        {
          return need_col ? acc_cond::mark_t{0} : acc_cond::mark_t{};
        };
        apply_copy_general(sub_aut, col_fun, algo);
        return;
      }
      // We sometimes need to have a list of the states
      // of res_ constructed by this call to apply_lar.
      const bool use_bscc = opt_.bscc;
      const bool use_last_post_process = opt_.use_last_post_process;
      constexpr bool is_tar = algo == TAR;
      const bool need_tree = !is_tar
                          && (opt_.search_ex || use_last_post_process);
      const bool need_state_list = use_last_post_process || use_bscc;
      const bool is_dfs = opt_.lar_dfs;
      // state_2_lar adapts add_new_state such that depending on the
      // value of use_last in get_compatible_state, we will be able
      // to find a compatible state faster.
      state_2_lar::memory_order order;
      if (!opt_.use_last)
      {
        if (opt_.use_last_post_process)
          order = state_2_lar::memory_order::BOTH;
        else
          order = state_2_lar::memory_order::ONLY_OLDEST;
      }
      else
        order = state_2_lar::memory_order::ONLY_NEWEST;
      state_2_lar s2l;
      if (need_tree)
        s2l.init(sub_aut->num_states(), order);
      std::vector<unsigned> states_scc_res;
      if (need_state_list)
        states_scc_res.reserve(sub_aut->num_states());
      auto init =
        sub_aut->get_named_prop<std::vector<unsigned>>("original-states");

      if (opt_.propagate_col)
        propagate_marks_here(sub_aut);

      auto init_state = sub_aut->get_init_state_number();
      robin_hood::unordered_map<to_parity_state<T>,
                                unsigned, to_parity_hash<T>> ps_2_num;
      unsigned lb_size;
      if constexpr (algo == TAR)
        lb_size = aut_->num_edges();
      else if constexpr (algo == CAR)
        lb_size = aut_->num_states() * aut_->acc().num_sets();
      else
        lb_size = aut_->num_states() * pairs.size();
      // num_2_ps maps a state of the result to a parity_state. As this function
      // does not always create the first state, we need to add
      // "- nb_states_before" to get a value.
      const unsigned nb_states_before = res_->num_states();
      std::vector<to_parity_state<T>> num_2_ps;
      // At least one copy of each state will be created.
      num_2_ps.reserve(lb_size + num_2_ps.size());
      ps_2_num.reserve(lb_size + num_2_ps.size());

      std::deque<unsigned> todo;
      // return a pair new_state, is_new such that
      // ps is associated to the state new_state in res_
      // and is_new is true if a new state was created by
      // get_state
      // We store 2 unsigned in a long long.
      static_assert(sizeof(long long) >= 2 * sizeof(unsigned));
      robin_hood::unordered_map<long long, unsigned>* edge_cache = nullptr;
      if (!use_last_post_process)
      {
        edge_cache = new robin_hood::unordered_map<long long, unsigned>();
        edge_cache->reserve(sub_aut->num_edges());
      }
      auto get_state = [&](const to_parity_state<T> &&ps) constexpr
      {
        auto it = ps_2_num.find(ps);
        if (it == ps_2_num.end())
        {
          unsigned nb = add_res_state(algo, ps);
          ps_2_num[ps] = nb;
          assert(nb == num_2_ps.size() + nb_states_before);
          num_2_ps.emplace_back(ps);
          todo.push_back(nb);
          if (need_state_list)
            states_scc_res.push_back(nb);
          return std::pair<unsigned, bool>{nb, true};
        }
        return std::pair<unsigned, bool>{it->second, false};
      };

      std::set<unsigned> pairs_indices;
      std::vector<std::map<memory<T>, memory<T>>> relations;
      if constexpr (algo == IAR_STREETT || algo == IAR_RABIN)
      {
        const auto num_pairs = pairs.size();
        for (unsigned k = 0; k < num_pairs; ++k)
          if (fin<algo>(pairs, k))
            pairs_indices.insert(k);
      }

      if constexpr (algo != TAR)
        if (opt_.force_order)
          relations = find_relations<algo>(sub_aut, pairs, pairs_indices);

      auto m = initial_memory_of<algo>(sub_aut, pairs, relations);

      assert(init);
      auto init_res = get_state({(*init)[init_state], init_state, m}).first;
      // A path is added when it is a destination. That is why we need to
      // add the initial state.
      unsigned nb_edges_before = res_->num_edges();
      std::vector<unsigned> edge_to_seen_nb;
      if (use_last_post_process && algo != TAR)
        edge_to_seen_nb.reserve(sub_aut->num_edges());
      if constexpr(!is_tar)
        if (need_tree)
          s2l.add_new_path(init_state, m, init_res, 0);

      robin_hood::unordered_map<acc_cond::mark_t, bool> acc_cache;
      // Main loop
      while (!todo.empty())
      {
        if (edge_cache)
          edge_cache->clear();
        // If we want to process the most recent state of the result, we
        // take the last value
        unsigned res_current = is_dfs ? todo.back() : todo.front();
        unsigned res_index = res_current - nb_states_before;
        const auto &current_ps = num_2_ps[res_index];
        const auto current_mem = current_ps.mem;
        if (is_dfs)
          todo.pop_back();
        else
          todo.pop_front();

        // For each edge leaving the state corresponding to res_state in sub_aut
        for (auto &e : sub_aut->out(current_ps.state_scc))
        {
          // We create a new memory and update it
          memory<T> mem(current_mem);
          unsigned nb_seen = 0,
                   h = 0;
          find_new_memory<algo>(e.dst, mem, sub_aut->edge_number(e), e.acc,
                                pairs, pairs_indices, nb_seen, h, relations);
          // Now we try to find a way to move the elements and obtain an
          // existing memory.
          unsigned res_dst = -1U;
          if constexpr (algo != TAR)
            if (opt_.search_ex)
              res_dst = s2l.get_compatible_state(e.dst, mem, nb_seen,
                                                opt_.use_last);
          // If it doesn't exist, we create a new state…
          if (res_dst == -1U)
          {
            auto gs = get_state({(*init)[e.dst], e.dst, mem});
            res_dst = gs.first;
            // And add it to the "tree" used to find a compatible state
            if constexpr (!is_tar)
            {
              if (need_tree && gs.second)
                s2l.add_new_path(e.dst, mem, res_dst, nb_seen);
            }
          }

          // We compute the color assigned to the new edge.
          acc_cond::mark_t new_edge_color{};
          compute_new_color_lar<algo, T>(sub_aut, current_mem, mem, h, e.acc,
                                      new_edge_color, pairs, acc_cache);

          // As we can assign a new destination later when
          // use_last_post_process is true, we cannot try to find a compatible
          // edge.
          auto edge_res_num = add_res_edge(res_current, res_dst,
                                           new_edge_color, e.cond,
                                           !use_last_post_process,
                                           edge_cache);
          (void) edge_res_num;
          // We have to remember how many colors were seen if we do a post
          // processing
          if constexpr (algo != TAR)
            if (use_last_post_process)
            {
              assert(edge_res_num ==
                     edge_to_seen_nb.size() + nb_edges_before + 1);
              edge_to_seen_nb.push_back(nb_seen);
            }
        }
      }

      // We used the most recent compatible state but perhaps that another
      // state was created after. We do a new search. As TAR always moves one
      // element we don't need it.
      if constexpr (algo != TAR)
        if (use_last_post_process)
        {
          for (auto &res_state : states_scc_res)
            for (auto &e : res_->out(res_state))
            {
              auto e_dst = e.dst;
              if (e.src == e_dst)
                continue;
              auto edge_num = res_->edge_number(e);
              const auto &ps = num_2_ps[e_dst - nb_states_before];
              unsigned seen_nb =
                  edge_to_seen_nb[edge_num - nb_edges_before - 1];
              assert(seen_nb < SPOT_MAX_ACCSETS);
              auto new_dst = s2l.get_compatible_state(ps.state_scc, ps.mem,
                                                      seen_nb, true);
              if (new_dst != e_dst)
              {
                assert(new_dst != -1U);
                need_purge_ = true;
                e.dst = new_dst;
              }
            }
        }
      if (use_bscc)
      {
        // Contrary to the (old) implementation of IAR, adding an edge between
        // 2 SCCs of the result is the last thing done. It means that
        // we don't need to use a filter when we compute the BSCC.
        // A state s is in the BSCC if scc_of(s) is 0.
        scc_info sub_scc(res_, init_res, nullptr, nullptr,
                         scc_info_options::NONE);
        if (sub_scc.scc_count() > 1)
        {
          need_purge_ = true;
          for (auto &state_produced : states_scc_res)
            if (sub_scc.scc_of(state_produced) == 0)
              state_to_res_[res_to_aut_[state_produced]] = state_produced;
        }
      }
      delete edge_cache;
    }

    void
    link_sccs()
    {
      if (si_.scc_count() > 1)
      {
        const unsigned res_num_states = res_->num_states();
        for (unsigned i = 0; i < res_num_states; ++i)
        {
          auto aut_i = res_to_aut_[i];
          auto aut_i_scc = si_.scc_of(aut_i);
          for (auto &e : aut_->out(aut_i))
            if (aut_i_scc != si_.scc_of(e.dst))
            {
              auto e_dst_repr = state_to_res_[e.dst];
              add_res_edge(i, e_dst_repr, {}, e.cond);
            }
        }
      }
    }

    bool
    try_parity_equivalence(const zielonka_tree &tree,
                           const twa_graph_ptr &sub_aut)
    {
      if (tree.has_parity_shape())
      {
        bool first_is_accepting = tree.is_even();
        // A vector that stores the difference between 2 levels.
        std::vector<acc_cond::mark_t> colors_diff;
        auto &tree_nodes = tree.nodes_;
        // Supposes that the index of the root is 0.
        unsigned current_index = 0;
        auto current_node = tree_nodes[current_index];
        // While the current node has a child
        while (current_node.first_child != 0)
        {
          auto child_index = current_node.first_child;
          auto child = tree_nodes[child_index];
          acc_cond::mark_t diff = current_node.colors - child.colors;
          colors_diff.emplace_back(diff);
          current_node = child;
        }
        // We have to deal with the edge between the last node and ∅.
        bool is_empty_accepting = sub_aut->acc().accepting({});
        bool is_current_accepting = (current_node.level % 2) != tree.is_even();
        if (is_empty_accepting != is_current_accepting)
          colors_diff.emplace_back(current_node.colors);
        // + 1 as we need to know which value should be given to an uncolored
        // edge.
        std::vector<unsigned> new_colors(
            sub_aut->get_acceptance().used_sets().max_set() + 1, -1U);
        unsigned current_col = colors_diff.size() - 1;
        for (auto &diff : colors_diff)
        {
          for (auto col : diff.sets())
            new_colors[col + 1] = current_col;
          --current_col;
        }
        bool is_max_even = first_is_accepting == (colors_diff.size() % 2);
        if (!is_max_even)
          change_to_odd();

        bool is_even_in_odd_world = is_odd_ && is_max_even;
        if (is_even_in_odd_world)
          for (auto &x : new_colors)
            ++x;
        apply_copy(sub_aut, new_colors, PARITY_EQUIV);
        return true;
      }
      return false;
    }

    bool
    try_parity_prefix(const zielonka_tree &tree, const twa_graph_ptr &sub_aut)
    {
      unsigned index = 0;
      auto current = tree.nodes_[index];
      std::vector<acc_cond::mark_t> prefixes;
      bool first_is_accepting = tree.is_even();

      acc_cond::mark_t removed_cols{};
      auto has_one_child = [&](const auto node) constexpr
      {
        auto fc = node.first_child;
        return tree.nodes_[fc].next_sibling == fc;
      };
      while (has_one_child(current))
      {
        auto child = tree.nodes_[current.first_child];
        acc_cond::mark_t diff{};
        const bool is_leaf = current.first_child == 0;
        if (is_leaf)
          diff = current.colors;
        else
          diff = current.colors - child.colors;
        prefixes.emplace_back(diff);
        removed_cols |= diff;
        if (is_leaf)
          break;
        current = child;
      }
      if (prefixes.empty())
        return false;

      if (opt_.datas)
        algo_used_ |= algorithm::PARITY_PREFIX;

      // As we want to remove the prefix we need to remove it from the
      // condition. As an unused color is not always removed (acc_clean false),
      // we do it here.
      auto used_cols = sub_aut->get_acceptance().used_sets() - removed_cols;
      auto new_cond = sub_aut->acc().restrict_to(used_cols);
      scc_info_to_parity sub(sub_aut, removed_cols);
      // The recursive call will add some informations to help
      // to add missing edges
      state_to_nums_ =
          new std::vector<std::vector<unsigned>>(aut_->num_states());
      opt_.parity_prefix = false;
      bool old_pp_gen = opt_.parity_prefix_general;
      opt_.parity_prefix_general = false;

      unsigned max_scc_color_rec = max_color_scc_;
      for (auto x : sub.split_aut({removed_cols}))
        {
          x->set_acceptance(new_cond);
          process_scc(x, algorithm::PARITY_PREFIX);
          max_scc_color_rec = std::max(max_scc_color_rec, max_color_scc_);
        }
      opt_.parity_prefix = true;
      opt_.parity_prefix_general = old_pp_gen;

      assert(max_scc_color_rec > 0);
      bool max_used_is_accepting = ((max_scc_color_rec - 1) % 2) == is_odd_;
      bool last_prefix_acc = (prefixes.size() % 2) != first_is_accepting;

      unsigned m = prefixes.size() + (max_used_is_accepting != last_prefix_acc)
                                   + max_scc_color_rec - 1;
      auto sub_aut_orig =
          sub_aut->get_named_prop<std::vector<unsigned>>("original-states");
      assert(sub_aut_orig);
      for (auto &e : sub_aut->edges())
        if (e.acc & removed_cols)
        {
          auto el = std::find_if(prefixes.begin(), prefixes.end(),
                                 [&](acc_cond::mark_t &x)
                                 { return x & e.acc; });
          assert(el != prefixes.end());
          unsigned pos = std::distance(prefixes.begin(), el);
          const unsigned col = m - pos;
          // As it is a parity prefix we should never get a lower value than
          // the color recursively produced.
          assert(col >= max_scc_color_rec);
          unsigned dst = state_to_res_[(*sub_aut_orig)[e.dst]];
          for (auto src : (*state_to_nums_)[(*sub_aut_orig)[e.src]])
            if (col != -1U)
              add_res_edge(src, dst, {col}, e.cond);
            else
              add_res_edge(src, dst, {}, e.cond);
        }
        // As when we need to use link_scc, a set of edges that link 2 SCC
        // need to be added and don't need to have a color.
        else if (sub.scc_of(e.src) != sub.scc_of(e.dst))
        {
          unsigned dst = state_to_res_[(*sub_aut_orig)[e.dst]];
          for (auto src : (*state_to_nums_)[(*sub_aut_orig)[e.src]])
            add_res_edge(src, dst, {}, e.cond);
        }
      delete state_to_nums_;
      state_to_nums_ = nullptr;

      return true;
    }

    bool
    try_parity_prefix_general(twa_graph_ptr &sub_aut)
    {
      // This function should not be applied on an "empty" automaton as
      // it must create an empty SCC with the algorithm NONE.
      assert(sub_aut->num_edges() > 0);
      static_assert((MAX_ACCSETS % 2) == 0,
                    "MAX_ACCSETS is supposed to be even");
      std::vector<acc_cond::mark_t> res_colors;
      std::vector<edge_status> status;
      acc_cond new_cond;
      bool was_able_to_color;
      // Is the maximal color accepting?
      bool start_inf = true;
      cond_type_main_aux(sub_aut, cond_kind::INF_PARITY, false, status,
                         res_colors, new_cond, was_able_to_color,
                         SPOT_MAX_ACCSETS - 1);
      // Otherwise we can try to find a rejecting transition as first step
      if (!was_able_to_color)
      {
        cond_type_main_aux(sub_aut, cond_kind::FIN_PARITY, false, status,
                           res_colors, new_cond, was_able_to_color,
                           SPOT_MAX_ACCSETS - 1);
        if (!was_able_to_color)
          return false;
        start_inf = false;
      }

      // If we have a parity-type automaton, it is just a copy.
      if (std::find(status.begin(), status.end(), edge_status::IMPOSSIBLE)
            == status.end())
      {
        std::vector<unsigned> res_cols;
        res_cols.reserve(res_colors.size());

        auto min_set =
          std::min_element(res_colors.begin() + 1, res_colors.end())->max_set();
        // Does the minimal color has the same parity than the maximal parity?
        bool same_acceptance_min_max = (min_set % 2);
        // Do we need to shift to match the parity of res_?
        bool odd_shift = start_inf != is_odd_;
        unsigned shift_col = min_set - (same_acceptance_min_max != odd_shift);
        std::transform(res_colors.begin(), res_colors.end(),
          std::back_inserter(res_cols), [&](auto &x)
                       { return x.max_set() - 1 - shift_col; });
        apply_copy_edge_index(sub_aut, res_cols,
                              algorithm::PARITY_PREFIX_GENERAL);
        return true;
      }

      // At this moment, a prefix exists
      auto& ev = sub_aut->edge_vector();
      const auto ev_size = ev.size();
      auto keep = std::shared_ptr<bitvect>(make_bitvect(ev_size));
      const unsigned status_size = status.size();
      for (unsigned i = 1; i < status_size; ++i)
        if (status[i] == edge_status::IMPOSSIBLE)
          keep->set(i);
        else
          keep->clear(i);

      // Avoid recursive parity prefix
      opt_.parity_prefix_general = false;
      bool old_pp = opt_.parity_prefix;
      opt_.parity_prefix = false;

      unsigned max_scc_color_rec = max_color_scc_;
      scc_info lower_scc(sub_aut, scc_info_options::TRACK_STATES);
      scc_info_to_parity sub(lower_scc, keep);
      state_to_nums_ =
        new std::vector<std::vector<unsigned>>(aut_->num_states());
      for (auto x : sub.split_aut(keep))
      {
        process_scc(x, algorithm::PARITY_PREFIX_GENERAL);
        max_scc_color_rec = std::max(max_scc_color_rec, max_color_scc_);
      }

      // restore options
      opt_.parity_prefix_general = true;
      opt_.parity_prefix = old_pp;

      assert(sub_aut->num_edges() > 0);

      // Compute the minimal color used by parity prefix.
      unsigned min_set_prefix = -2U;
      for (unsigned i = 1; i < ev_size; ++i)
        if (status[i] == edge_status::MARKED)
        {
          auto e_mark = res_colors[i].max_set();
          if (min_set_prefix == -2U)
            min_set_prefix = e_mark - 1;
          else
            min_set_prefix = std::min(min_set_prefix + 1, e_mark) - 1;
        }

      // At least one transition should be marked here.
      assert(min_set_prefix != -2U);

      // Reduce the colors used by parity_prefix.
      const bool min_prefix_accepting = (min_set_prefix % 2) == start_inf;
      // max_scc_color_rec has a value as the automaton is not parity-type,
      // so there was a recursive paritisation
      assert(max_scc_color_rec > 0);
      const bool max_rec_accepting = ((max_scc_color_rec - 1) % 2) == is_odd_;
      const bool same_prio = min_prefix_accepting == max_rec_accepting;
      const unsigned delta =
        min_set_prefix - (max_scc_color_rec + 1) - !same_prio;

      auto sub_aut_orig =
        sub_aut->get_named_prop<std::vector<unsigned>>("original-states");
      assert(sub_aut_orig);
      for (unsigned e_num = 1; e_num < ev_size; ++e_num)
      {
        auto& e = ev[e_num];
        if (status[e_num] == edge_status::MARKED)
        {
          unsigned dst = state_to_res_[(*sub_aut_orig)[e.dst]];
          for (auto src : (*state_to_nums_)[(*sub_aut_orig)[e.src]])
          {
            auto col = res_colors[e_num].max_set() - delta - 1;
            if (col == -1U)
              add_res_edge(src, dst, {}, e.cond);
            else
              add_res_edge(src, dst, {col}, e.cond);
          }
        }
      }

      delete state_to_nums_;
      state_to_nums_ = nullptr;

      return true;
    }

    bool
    try_emptiness(const const_twa_graph_ptr &sub_aut, bool &tried)
    {
      tried = true;
      if (generic_emptiness_check(sub_aut))
      {
        auto col_fun =
          [col = is_odd_ ? acc_cond::mark_t{0} : acc_cond::mark_t{}]
          (const twa_graph::edge_storage_t &) noexcept
        {
          return col;
        };
        apply_copy_general(sub_aut, col_fun, GENERIC_EMPTINESS);
        return true;
      }
      return false;
    }

    bool
    try_rabin_to_buchi(twa_graph_ptr &sub_aut)
    {
      algorithm algo = RABIN_TO_BUCHI;
      auto buch_aut = rabin_to_buchi_if_realizable(sub_aut);
      if (buch_aut == nullptr)
      {
        algo = STREETT_TO_COBUCHI;
        auto old_cond = sub_aut->get_acceptance();
        sub_aut->set_acceptance(acc_cond(old_cond.complement()));
        buch_aut = rabin_to_buchi_if_realizable(sub_aut);
        sub_aut->set_acceptance(acc_cond(old_cond));
      }
      if (buch_aut != nullptr)
      {
        if (algo == STREETT_TO_COBUCHI)
          change_to_odd();
        unsigned shift = (algo == RABIN_TO_BUCHI) && is_odd_;

        auto &buch_aut_ev = buch_aut->edge_vector();
        // 0 is not an edge, so we assign -1;
        std::vector<unsigned> colors;
        colors.reserve(buch_aut_ev.size());
        colors.push_back(-1U);
        std::transform(
            buch_aut_ev.begin() + 1, buch_aut_ev.end(),
            std::back_inserter(colors),
            [&](const twa_graph::edge_storage_t &e) {
              return e.acc.max_set() - 1 + shift;
            });
        apply_copy_edge_index(sub_aut, colors, algo);
        return true;
      }
      return false;
    }

    bool
    try_buchi_type(const twa_graph_ptr &sub_aut)
    {
      std::vector<edge_status> status;
      std::vector<acc_cond::mark_t> res_colors;
      acc_cond new_cond;
      bool is_co_bu = false;
      bool was_able_to_color;
      if (!cond_type_main_aux(sub_aut, cond_kind::BUCHI, true, status,
                              res_colors, new_cond, was_able_to_color, 0))
      {
        is_co_bu = true;
        if (!cond_type_main_aux(sub_aut, cond_kind::CO_BUCHI, true, status,
                                res_colors, new_cond, was_able_to_color, 0))
          return false;
        change_to_odd();
      }
      // Tests if all edges are colored or all edges are uncolored
      auto [min, max] =
        std::minmax_element(res_colors.begin() + 1, res_colors.end());
      const bool one_color = min->max_set() == max->max_set();
      const bool is_colored = min->max_set();
      auto col_fun = [&](const twa_graph::edge_storage_t &edge)
      {
        // If there one color in the automaton, we can simplify.
        if (one_color)
        {
          bool z = (is_colored && !is_odd_) || (!is_colored && is_odd_);
          // When we do co-buchi, we reverse
          if (is_co_bu)
            z = !z;
          return z ? acc_cond::mark_t{0} : acc_cond::mark_t{};
        }
        // Otherwise, copy the color
        auto edge_number = sub_aut->edge_number(edge);
        unsigned mc = res_colors[edge_number].max_set() - 1;
        mc += (!is_co_bu && is_odd_);
        if (mc == -1U)
          return acc_cond::mark_t{};
        return acc_cond::mark_t{mc};
      };
      apply_copy_general(sub_aut, col_fun, is_co_bu ? algorithm::CO_BUCHI_TYPE
                                                    : algorithm::BUCHI_TYPE);
      return true;
    }

    bool
    try_parity_type(const twa_graph_ptr &sub_aut)
    {
      std::vector<edge_status> status;
      std::vector<acc_cond::mark_t> res_colors;
      acc_cond new_cond;
      bool was_able_to_color;
      if (!cond_type_main_aux(sub_aut, cond_kind::INF_PARITY, true, status,
                              res_colors, new_cond, was_able_to_color,
                              SPOT_MAX_ACCSETS - 3))
      {
        if (!cond_type_main_aux(sub_aut, cond_kind::FIN_PARITY, true, status,
                                res_colors, new_cond, was_able_to_color,
                                SPOT_MAX_ACCSETS - 3))
          return false;
      }
      bool is_max, is_odd;
      new_cond.is_parity(is_max, is_odd);
      auto min =
        std::min_element(res_colors.begin() + 1, res_colors.end());
      // cond_type_main_aux returns a parity max condition
      assert(is_max);
      auto col_fun =
        [shift = (is_odd != is_odd_) - (min->max_set() + (min->max_set() % 2)),
         &res_colors, &sub_aut]
        (const twa_graph::edge_storage_t &edge)
      {
        auto edge_number = sub_aut->edge_number(edge);
        unsigned mc = res_colors[edge_number].max_set() - 1;
        mc += shift;
        if (mc == -1U)
          return acc_cond::mark_t{};
        return acc_cond::mark_t{mc};
      };
      apply_copy_general(sub_aut, col_fun, PARITY_TYPE);
      return true;
    }

    // Keeps the result of the partial degeneralization if it reduces the number
    // of colors or it allows to apply IAR.
    bool
    keep_deg(const const_twa_graph_ptr &sub_aut, const const_twa_graph_ptr &deg)
    {
      if (!opt_.reduce_col_deg)
        return true;
      unsigned nb_col_orig = sub_aut->get_acceptance().used_sets().count();

      if (deg->get_acceptance().used_sets().count() < nb_col_orig)
        return true;
      std::vector<acc_cond::rs_pair> pairs;
      if (deg->acc().is_rabin_like(pairs))
        {
          remove_duplicates(pairs);
          if (pairs.size() < nb_col_orig)
            return true;
        }
      if (deg->acc().is_streett_like(pairs))
        {
          remove_duplicates(pairs);
          if (pairs.size() < nb_col_orig)
            return true;
        }
      return false;
    }

    // Process a SCC. If there is no edge in the automaton, a new state is
    // created and we say (if pretty_print is true) that none_algo created
    // this state.
    void
    process_scc(twa_graph_ptr &sub_aut,
                const algorithm none_algo = algorithm::NONE)
    {
      // Init the maximal color produced when processing this SCC.
      max_color_scc_ = 0;
      // If the sub_automaton is "empty", we don't need to apply an algorithm.
      if (sub_aut->num_edges() == 0)
        {
          apply_copy(sub_aut, {}, none_algo);
          return;
        }

      bool tried_emptiness = false;
      bool changed_structure = true;
      while (true)
        {
          auto cond_before_simpl = sub_aut->acc();
          if (opt_.acc_clean)
            simplify_acceptance_here(sub_aut);
          if (opt_.propagate_col)
            {
              propagate_marks_here(sub_aut);
              if (opt_.acc_clean)
                simplify_acceptance_here(sub_aut);
            }
          if (opt_.datas && sub_aut->acc() != cond_before_simpl)
            algo_used_ |= algorithm::ACC_CLEAN;

          if (opt_.parity_equiv || opt_.parity_prefix)
            {
              // If we don't try to find a parity prefix, we can stop
              // to construct the tree when it has not parity shape.
              zielonka_tree_options zopt =
                zielonka_tree_options::MERGE_SUBTREES
                | zielonka_tree_options::CHECK_PARITY;
              if (!opt_.parity_prefix)
                zopt = zopt | zielonka_tree_options::ABORT_WRONG_SHAPE;
              auto tree = zielonka_tree(sub_aut->acc(), zopt);
              // If it is not parity shape, tree.nodes_ will be empty
              if (tree.num_branches() != 0 && opt_.parity_equiv
                  && try_parity_equivalence(tree, sub_aut))
                return;
              if (opt_.parity_prefix && try_parity_prefix(tree, sub_aut))
                return;
            }

          if (changed_structure && opt_.parity_prefix_general
              && try_parity_prefix_general(sub_aut))
            return;

          if (opt_.generic_emptiness
              && !tried_emptiness && try_emptiness(sub_aut, tried_emptiness))
            return;

          // Buchi_type_to_buchi is more general that Rabin_to_buchi so
          // we just call rabin_to_buchi if buchi_type_to_buchi is false.
          if (!opt_.buchi_type_to_buchi
              && !opt_.parity_type_to_parity && opt_.rabin_to_buchi
              && try_rabin_to_buchi(sub_aut))
            return;

          // As parity_type_to_parity is stronger, we don't
          // try if this option is used.
          if (opt_.buchi_type_to_buchi
              && !opt_.parity_type_to_parity && try_buchi_type(sub_aut))
            return;

          // We don't do it if parity_prefix_general is true as on a parity-type
          // automaton parity_prefix_general removes all the transitions and
          // we also get a parity-type automaton.
          if (!opt_.parity_prefix_general && opt_.parity_type_to_parity
              && try_parity_type(sub_aut))
            return;

          if (opt_.partial_degen)
            {
              twa_graph_ptr deg = sub_aut;
              std::vector<acc_cond::mark_t> forbid;
              bool changed = false;
              while (acc_cond::mark_t m =
                     is_partially_degeneralizable(deg, true, true, forbid))
                {
                  twa_graph_ptr tmp = partial_degeneralize(deg, m);
                  simplify_acceptance_here(tmp);
                  if (keep_deg(deg, tmp))
                    {
                      algo_used_ |= algorithm::PARTIAL_DEGEN;
                      deg = tmp;
                      changed = true;
                      changed_structure = true;
                    }
                  else
                    {
                      forbid.emplace_back(m);
                    }
                }
              if (changed)
                {
                  sub_aut = deg;
                  continue;
                }
            }
          break;
        }
      if (opt_.use_generalized_rabin)
        {
          auto gen_rab = to_generalized_rabin(sub_aut);
          // to_generalized_rabin does not propagate original-states.
          auto sub_aut_orig =
            sub_aut->get_named_prop<std::vector<unsigned>>("original-states");
          assert(sub_aut_orig);
          auto orig = new std::vector<unsigned>();
          const auto sub_aut_num_states = sub_aut->num_states();
          orig->reserve(sub_aut_num_states);
          gen_rab->set_named_prop("original-states", orig);
          for (unsigned i = 0; i < sub_aut_num_states; ++i)
            orig->push_back((*sub_aut_orig)[i]);
          sub_aut = partial_degeneralize(gen_rab);
        }
      std::vector<acc_cond::rs_pair> pairs;
      algorithm algo = choose_lar(sub_aut->acc(), pairs, sub_aut->num_edges());
      if (opt_.datas)
        algo_used_ |= algo;
      if (algo == CAR)
        apply_lar<CAR, memory_type>(sub_aut, pairs);
      else if (algo == IAR_STREETT)
        apply_lar<IAR_STREETT, memory_type>(sub_aut, pairs);
      else if (algo == IAR_RABIN)
        apply_lar<IAR_RABIN, memory_type>(sub_aut, pairs);
      else if (algo == TAR)
        apply_lar<TAR, unsigned>(sub_aut, pairs);
      else
        SPOT_UNREACHABLE();
    }

  public:
    twa_graph_ptr
    run()
    {
      res_ = make_twa_graph(aut_->get_dict());
      res_->copy_ap_of(aut_);
      const unsigned num_scc = si_.scc_count();
      std::vector<unsigned>* orig_aut =
        aut_->get_named_prop<std::vector<unsigned>>("original-states");
      std::vector<unsigned> orig_st;
      if (orig_aut)
        {
          orig_st = std::move(*orig_aut);
          std::const_pointer_cast<twa_graph>(aut_)
            ->set_named_prop("original-states", nullptr);
        }
      auto sccs = si_.split_aut();
      for (unsigned scc = 0; scc < num_scc; ++scc)
        {
          auto sub_automaton = sccs[scc];
          process_scc(sub_automaton);
        }

      link_sccs();
      // During the execution, to_parity works on its own
      // original-states and we must combine it with the property original
      // states of aut_ to propagate the information.
      if (orig_aut)
        for (unsigned i = 0; i < orig_->size(); ++i)
          (*orig_)[i] = orig_st[(*orig_)[i]];
      res_->set_named_prop("original-states", orig_);
      if (opt_.pretty_print)
        res_->set_named_prop("state-names", names_);
      if (res_->num_states() == 0)
        add_res_state<memory_type>(NONE, {0, 0, {}});
      res_->set_init_state(state_to_res_[aut_->get_init_state_number()]);
      // There is only a subset of algorithm that can create an unreachable
      // state
      if (need_purge_)
        res_->purge_unreachable_states();
      // A special case is an automaton without edge. It implies
      // max_color_used_ has not value so we need to test it.
      if (max_color_used_ == 0)
        {
          assert(aut_->num_edges() == 0);
          res_->set_acceptance(acc_cond(acc_cond::acc_code::f()));
        }
      else
        {
          res_->set_acceptance(acc_cond(acc_cond::acc_code::
                                        parity(true, is_odd_,
                                               max_color_used_)));
        }
      if (opt_.datas)
      {
        constexpr std::array<algorithm, 14>
          algos = {BUCHI_TYPE, CAR, CO_BUCHI_TYPE, GENERIC_EMPTINESS, IAR_RABIN,
                    IAR_STREETT, NONE, PARITY_EQUIV, PARITY_PREFIX,
                    PARITY_PREFIX_GENERAL, PARITY_TYPE, RABIN_TO_BUCHI,
                    STREETT_TO_COBUCHI, TAR};
          for (auto al : algos)
            if (algo_used_ & al)
              opt_.datas->algorithms_used.emplace_back(algorithm_to_str(al));
      }
      return res_;
    }

    to_parity_generator(const const_twa_graph_ptr &aut,
                        const to_parity_options opt)
        : aut_(aut),
          opt_(opt),
          si_(aut),
          state_to_res_(aut->num_states(), -1U)
    {
      auto aut_num = aut->num_states();
      res_to_aut_.reserve(aut_num);
      orig_ = new std::vector<unsigned>();
      orig_->reserve(aut_num);
      if (opt.pretty_print)
      {
        names_ = new std::vector<std::string>();
        names_->reserve(aut_num);
      }
    }
  };

  twa_graph_ptr
  to_parity(const const_twa_graph_ptr &aut,
            const to_parity_options options)
  {
    bool is_max, is_odd;
    if (aut->acc().is_parity(is_max, is_odd, false))
    {
      if (!is_max)
        return change_parity(aut, parity_kind::parity_kind_max,
                                  parity_style::parity_style_any);
      else
      {
        auto res = make_twa_graph(aut, twa::prop_set::all());
        res->copy_acceptance_of(aut);
        return res;
      }
    }
    to_parity_generator gen(aut, options);
    return gen.run();
  }

  // Old version of CAR

  namespace
  {
    struct lar_state
    {
      unsigned state;
      std::vector<unsigned> perm;

      bool operator<(const lar_state &s) const
      {
        return state == s.state ? perm < s.perm : state < s.state;
      }

      std::string to_string() const
      {
        std::ostringstream s;
        s << state << " [";
        unsigned ps = perm.size();
        for (unsigned i = 0; i < ps; ++i)
        {
          if (i > 0)
            s << ',';
          s << perm[i];
        }
        s << ']';
        return s.str();
      }
    };

    class lar_generator
    {
      const const_twa_graph_ptr &aut_;
      twa_graph_ptr res_;
      const bool pretty_print;

      std::map<lar_state, unsigned> lar2num;

    public:
      explicit lar_generator(const const_twa_graph_ptr &a, bool pretty_print)
          : aut_(a), res_(nullptr), pretty_print(pretty_print)
      {
      }

      twa_graph_ptr run()
      {
        res_ = make_twa_graph(aut_->get_dict());
        res_->copy_ap_of(aut_);

        std::deque<lar_state> todo;
        auto get_state = [this, &todo](const lar_state &s)
        {
          auto it = lar2num.emplace(s, -1U);
          if (it.second) // insertion took place
          {
            unsigned nb = res_->new_state();
            it.first->second = nb;
            todo.push_back(s);
          }
          return it.first->second;
        };

        std::vector<unsigned> initial_perm(aut_->num_sets());
        std::iota(initial_perm.begin(), initial_perm.end(), 0);
        {
          lar_state s0{aut_->get_init_state_number(), initial_perm};
          res_->set_init_state(get_state(s0));
        }

        scc_info si(aut_, scc_info_options::NONE);
        // main loop
        while (!todo.empty())
        {
          lar_state current = todo.front();
          todo.pop_front();

          // TODO: todo could store this number to avoid one lookup
          unsigned src_num = get_state(current);

          unsigned source_scc = si.scc_of(current.state);
          for (const auto &e : aut_->out(current.state))
          {
            // find the new permutation
            std::vector<unsigned> new_perm = current.perm;
            unsigned h = 0;
            for (unsigned k : e.acc.sets())
            {
              auto it = std::find(new_perm.begin(), new_perm.end(), k);
              h = std::max(h, unsigned(new_perm.end() - it));
              std::rotate(it, it + 1, new_perm.end());
            }

            if (source_scc != si.scc_of(e.dst))
            {
              new_perm = initial_perm;
              h = 0;
            }

            lar_state dst{e.dst, new_perm};
            unsigned dst_num = get_state(dst);

            // Do the h last elements satisfy the acceptance condition?
            // If they do, emit 2h, if they don't emit 2h+1.
            acc_cond::mark_t m(new_perm.end() - h, new_perm.end());
            bool rej = !aut_->acc().accepting(m);
            res_->new_edge(src_num, dst_num, e.cond, {2 * h + rej});
          }
        }

        // parity max even
        unsigned sets = 2 * aut_->num_sets() + 2;
        res_->set_acceptance(sets, acc_cond::acc_code::parity_max_even(sets));

        if (pretty_print)
        {
          auto names = new std::vector<std::string>(res_->num_states());
          for (const auto &p : lar2num)
            (*names)[p.second] = p.first.to_string();
          res_->set_named_prop("state-names", names);
        }

        return res_;
      }
    };
  }

  twa_graph_ptr
  to_parity_old(const const_twa_graph_ptr &aut, bool pretty_print)
  {
    if (!aut->is_existential())
      throw std::runtime_error("LAR does not handle alternation");
    // if aut is already parity return it as is
    if (aut->acc().is_parity())
      return std::const_pointer_cast<twa_graph>(aut);

    lar_generator gen(aut, pretty_print);
    return gen.run();
  }

  // Old version of IAR

  namespace
  {
    using perm_t = std::vector<unsigned>;
    struct iar_state
    {
      unsigned state;
      perm_t perm;

      bool
      operator<(const iar_state &other) const
      {
        return state == other.state ? perm < other.perm : state < other.state;
      }
    };

    template <bool is_rabin>
    class iar_generator
    {
      // helper functions: access fin and inf parts of the pairs
      // these functions negate the Streett condition to see it as a Rabin one
      const acc_cond::mark_t &
      fin(unsigned k) const
      {
        if (is_rabin)
          return pairs_[k].fin;
        else
          return pairs_[k].inf;
      }
      acc_cond::mark_t
      inf(unsigned k) const
      {
        if (is_rabin)
          return pairs_[k].inf;
        else
          return pairs_[k].fin;
      }

    public:
      explicit iar_generator(const const_twa_graph_ptr &a,
                             const std::vector<acc_cond::rs_pair> &p,
                             const bool pretty_print)
          : aut_(a), pairs_(p), scc_(scc_info(a)), pretty_print_(pretty_print),
            state2pos_iar_states(aut_->num_states(), -1U)
      {
      }

      twa_graph_ptr
      run()
      {
        res_ = make_twa_graph(aut_->get_dict());
        res_->copy_ap_of(aut_);

        build_iar_scc(scc_.initial());

        {
          // resulting automaton has acceptance condition: parity max odd
          // for Rabin-like input and parity max even for Streett-like input.
          // with priorities ranging from 0 to 2*(nb pairs)
          // /!\ priorities are shifted by -1 compared to the original paper
          unsigned sets = 2 * pairs_.size() + 1;
          res_->set_acceptance(sets, acc_cond::acc_code::parity_max(is_rabin,
                                                                    sets));
        }
        {
          unsigned s = iar2num.at(state2iar.at(aut_->get_init_state_number()));
          res_->set_init_state(s);
        }

        if (pretty_print_)
        {
          unsigned nstates = res_->num_states();
          auto names = new std::vector<std::string>(nstates);
          for (auto e : res_->edges())
          {
            unsigned s = e.src;
            iar_state iar = num2iar[s];
            std::ostringstream st;
            st << iar.state << ' ';
            if (iar.perm.empty())
              st << '[';
            char sep = '[';
            for (unsigned h : iar.perm)
            {
              st << sep << h;
              sep = ',';
            }
            st << ']';
            (*names)[s] = st.str();
          }
          res_->set_named_prop("state-names", names);
        }

        // there could be quite a number of unreachable states, prune them
        res_->purge_unreachable_states();
        return res_;
      }

      void
      build_iar_scc(unsigned scc_num)
      {
        // we are working on an SCC: the state we start from does not matter
        unsigned init = scc_.one_state_of(scc_num);

        std::deque<iar_state> todo;
        auto get_state = [&](const iar_state &s)
        {
          auto it = iar2num.find(s);
          if (it == iar2num.end())
          {
            unsigned nb = res_->new_state();
            iar2num[s] = nb;
            num2iar[nb] = s;
            unsigned iar_pos = iar_states.size();
            unsigned old_newest_pos = state2pos_iar_states[s.state];
            state2pos_iar_states[s.state] = iar_pos;
            iar_states.push_back({s, old_newest_pos});
            todo.push_back(s);
            return nb;
          }
          return it->second;
        };

        auto get_other_scc = [this](unsigned state)
        {
          auto it = state2iar.find(state);
          // recursively build the destination SCC if we detect that it has
          // not been already built.
          if (it == state2iar.end())
            build_iar_scc(scc_.scc_of(state));
          return iar2num.at(state2iar.at(state));
        };

        if (scc_.is_trivial(scc_num))
        {
          iar_state iar_s{init, perm_t()};
          state2iar[init] = iar_s;
          unsigned src_num = get_state(iar_s);
          // Do not forget to connect to subsequent SCCs
          for (const auto &e : aut_->out(init))
            res_->new_edge(src_num, get_other_scc(e.dst), e.cond);
          return;
        }

        // determine the pairs that appear in the SCC
        auto colors = scc_.acc_sets_of(scc_num);
        std::set<unsigned> scc_pairs;
        for (unsigned k = 0; k != pairs_.size(); ++k)
          if (!inf(k) || (colors & (pairs_[k].fin | pairs_[k].inf)))
            scc_pairs.insert(k);

        perm_t p0;
        for (unsigned k : scc_pairs)
          p0.push_back(k);

        iar_state s0{init, p0};
        get_state(s0); // put s0 in todo

        // the main loop
        while (!todo.empty())
        {
          iar_state current = todo.front();
          todo.pop_front();

          unsigned src_num = get_state(current);

          for (const auto &e : aut_->out(current.state))
          {
            // connect to the appropriate state
            if (scc_.scc_of(e.dst) != scc_num)
              res_->new_edge(src_num, get_other_scc(e.dst), e.cond);
            else
            {
              // find the new permutation
              perm_t new_perm = current.perm;
              // Count pairs whose fin-part is seen on this transition
              unsigned seen_nb = 0;
              // consider the pairs for this SCC only
              for (unsigned k : scc_pairs)
                if (e.acc & fin(k))
                {
                  ++seen_nb;
                  auto it = std::find(new_perm.begin(),
                                      new_perm.end(),
                                      k);
                  // move the pair in front of the permutation
                  std::rotate(new_perm.begin(), it, it + 1);
                }

              iar_state dst;
              unsigned dst_num = -1U;

              // Optimization: when several indices are seen in the
              // transition, they move at the front of new_perm in any
              // order. Check whether there already exists an iar_state
              // that matches this condition.

              auto iar_pos = state2pos_iar_states[e.dst];
              while (iar_pos != -1U)
              {
                iar_state &tmp = iar_states[iar_pos].first;
                iar_pos = iar_states[iar_pos].second;
                if (std::equal(new_perm.begin() + seen_nb,
                               new_perm.end(),
                               tmp.perm.begin() + seen_nb))
                {
                  dst = tmp;
                  dst_num = iar2num[dst];
                  break;
                }
              }
              // if such a state was not found, build it
              if (dst_num == -1U)
              {
                dst = iar_state{e.dst, new_perm};
                dst_num = get_state(dst);
              }

              // find the maximal index encountered by this transition
              unsigned maxint = -1U;
              for (int k = current.perm.size() - 1; k >= 0; --k)
              {
                unsigned pk = current.perm[k];
                if (!inf(pk) ||
                    (e.acc & (pairs_[pk].fin | pairs_[pk].inf)))
                {
                  maxint = k;
                  break;
                }
              }

              acc_cond::mark_t acc{};
              if (maxint == -1U)
                acc.set(0);
              else if (e.acc & fin(current.perm[maxint]))
                assign_color(acc, 2 * maxint + 2);
              else
                assign_color(acc, 2 * maxint + 1);

              res_->new_edge(src_num, dst_num, e.cond, acc);
            }
          }
        }

        // Optimization: find the bottom SCC of the sub-automaton we have just
        // built. To that end, we have to ignore edges going out of scc_num.
        auto leaving_edge = [&](unsigned d) constexpr
        {
          return scc_.scc_of(num2iar.at(d).state) != scc_num;
        };
        auto filter_edge = [](const twa_graph::edge_storage_t &,
                              unsigned dst,
                              void *filter_data) constexpr
        {
          decltype(leaving_edge) *data =
              static_cast<decltype(leaving_edge) *>(filter_data);

          if ((*data)(dst))
            return scc_info::edge_filter_choice::ignore;
          return scc_info::edge_filter_choice::keep;
        };
        scc_info sub_scc(res_, get_state(s0), filter_edge, &leaving_edge);
        // SCCs are numbered in reverse topological order, so the bottom SCC
        // has index 0.
        const unsigned bscc = 0;
        assert(sub_scc.succ(0).empty());
        assert(
            [&]()
            {
              for (unsigned s = 1; s != sub_scc.scc_count(); ++s)
                if (sub_scc.succ(s).empty())
                  return false;
              return true;
            }());

        assert(sub_scc.states_of(bscc).size()
                >= scc_.states_of(scc_num).size());

        // update state2iar
        for (unsigned scc_state : sub_scc.states_of(bscc))
        {
          iar_state &iar = num2iar.at(scc_state);
          if (state2iar.find(iar.state) == state2iar.end())
            state2iar[iar.state] = iar;
        }
      }

    private:
      const const_twa_graph_ptr &aut_;
      const std::vector<acc_cond::rs_pair> &pairs_;
      const scc_info scc_;
      twa_graph_ptr res_;
      bool pretty_print_;

      // to be used when entering a new SCC
      // maps a state of aut_ onto an iar_state with the appropriate perm
      std::map<unsigned, iar_state> state2iar;

      std::map<iar_state, unsigned> iar2num;
      std::map<unsigned, iar_state> num2iar;

      std::vector<unsigned> state2pos_iar_states;
      std::vector<std::pair<iar_state, unsigned>> iar_states;
    };

    // Make this a function different from iar_maybe(), so that
    // iar() does not have to call a deprecated function.
    static twa_graph_ptr
    iar_maybe_(const const_twa_graph_ptr &aut, bool pretty_print)
    {
      std::vector<acc_cond::rs_pair> pairs;
      if (!aut->acc().is_rabin_like(pairs))
        if (!aut->acc().is_streett_like(pairs))
          return nullptr;
        else
        {
          iar_generator<false> gen(aut, pairs, pretty_print);
          return gen.run();
        }
      else
      {
        iar_generator<true> gen(aut, pairs, pretty_print);
        return gen.run();
      }
    }
  }

  twa_graph_ptr
  iar(const const_twa_graph_ptr &aut, bool pretty_print)
  {
    if (auto res = iar_maybe_(aut, pretty_print))
      return res;
    throw std::runtime_error("iar() expects Rabin-like or Streett-like input");
  }

  twa_graph_ptr
  iar_maybe(const const_twa_graph_ptr &aut, bool pretty_print)
  {
    return iar_maybe_(aut, pretty_print);
  }
}
