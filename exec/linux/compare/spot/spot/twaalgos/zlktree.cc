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
#include <iostream>
#include <deque>
#include <algorithm>
#include <spot/twaalgos/zlktree.hh>
#include <spot/twaalgos/genem.hh>
#include <spot/misc/escape.hh>
#include <spot/priv/robin_hood.hh>
using namespace std::string_literals;

namespace spot
{
  namespace
  {
    // Or goal is the find the list of maximal models for a formula
    // named cond and defined later for each node.
    //
    // For instance if cond is satisfied by {1}, {3}, {1,2}, {1,2,3},
    // {0,3}, and {0,1,3}, then the maximal models are {1,2,3} and
    // {0,1,3}.
    //
    // To do that we build a list of models ordered by decreasing
    // size.  When inserting a model, we will compare it to all
    // model of larger size first, and abort the insertion if
    // needed.  Otherwise we insert it, and then compare it to
    // smaller models to possibly remove them.
    //
    // "models" is the variable where we store those ordered models.
    // This list is local to each node, but we reuse the vector
    // between each iteration to avoid unnecessary allocations.
    struct size_model
    {
      unsigned size;
      acc_cond::mark_t model;
    };

    void max_models(acc_cond cond,
                    acc_cond::mark_t colors,
                    std::vector<size_model>& out)
    {
      if (!colors)
        return;
      if (cond.accepting(colors))
        {
          unsigned sz = colors.count();
          auto iter = out.begin();
          while (iter != out.end())
            {
              if (iter->size < sz)
                // We have checked all larger models.
                break;
              if (colors.subset(iter->model))
                // curmodel is covered by iter->model.
                return;
              ++iter;
            }
          // insert the model
          iter = out.insert(iter, {sz, colors});
          ++iter;
          // erase all models it contains
          out.erase(std::remove_if(iter, out.end(),
                                   [&](auto& mod) {
                                     return mod.model.subset(colors);
                                   }), out.end());
          return;
        }
      // If the condition has some Inf(x) forced at the top-level, we
      // may simply replace Inf(x) by t and Fin(x) by f in condition
      // to simplify it.  This is a kind of cheap unit-propagation.
      if (acc_cond::mark_t fu = cond.inf_unit())
        cond = cond.remove(fu, false);
      // Now if we have some Fin(x) forced at the top-level,
      // we know the corresponding color need to me removed from
      // the set.
      if (acc_cond::mark_t fu = cond.fin_unit())
        {
          max_models(cond.remove(fu, true), colors - fu, out);
        }
      // Otherwise, we simply have to pick some arbitrary Fin(x) and
      // see if we can satisfy the condition when x is present or
      // absent.  In this case, we do not know whether the generated
      // models will be maximal, so this justifies the inclusion
      // checks between models at the top of this function.
      else if (auto [fo, fpart] = cond.fin_one_extract(); fo >= 0)
        {
          acc_cond::mark_t fo_m = {(unsigned) fo};
          max_models(fpart.remove(fo_m, true), colors - fo_m, out);
          max_models(cond.remove(fo_m, false), colors, out);
        }
    }
  }

  zielonka_tree::zielonka_tree(const acc_cond& cond,
                               zielonka_tree_options opt)
  {
    const acc_cond::acc_code& code = cond.get_acceptance();
    auto all = cond.all_sets();
    acc_cond negcond(cond.num_sets(), cond.get_acceptance().complement());

    nodes_.emplace_back();
    nodes_[0].parent = 0;
    nodes_[0].colors = all;
    nodes_[0].level = 0;

    robin_hood::unordered_node_map<acc_cond::mark_t, unsigned> nmap;

    std::vector<size_model> models;
    // This loop is a BFS over the increasing set of nodes.
    for (unsigned node = 0; node < nodes_.size(); ++node)
    {
      acc_cond::mark_t colors = nodes_[node].colors;
      unsigned nextlvl = nodes_[node].level + 1;

      // Have we already seen this combination of colors previously?
      // If yes, simply copy the children.
      if (auto p = nmap.emplace(colors, node); !p.second)
        {
          unsigned fc = nodes_[p.first->second].first_child;
          if (!fc)              // this is a leaf
            {
              ++num_branches_;
              continue;
            }
          if (!!(opt & zielonka_tree_options::MERGE_SUBTREES))
            {
              nodes_[node].first_child = fc;
              continue;
            }
          unsigned child = fc;
          unsigned first = nodes_.size();
          nodes_[node].first_child = first;
          do
            {
              auto& c = nodes_[child];
              child = c.next_sibling;
              nodes_.push_back({node, static_cast<unsigned>(nodes_.size() + 1),
                                0, nextlvl, c.colors});
            }
          while (child != fc);
          nodes_.back().next_sibling = first;
          // We do not have to test the shape since this is the second time
          // we see these colors;
          continue;
        }

      bool is_accepting = code.accepting(colors);
      if (node == 0)
        is_even_ = is_accepting;

      acc_cond c = (is_accepting ? negcond : cond).restrict_to(colors);
      models.clear();
      max_models(c, colors, models);

      unsigned num_children = models.size();
      if (num_children == 0) // This is a leaf of the tree.
        {
          if (num_branches_++ == 0)
            one_branch_ = node;
          continue;
        }
      unsigned first = nodes_.size();
      nodes_[node].first_child = first;
      nodes_.reserve(first + num_children);
      for (auto& m: models)
        nodes_.push_back({node, static_cast<unsigned>(nodes_.size() + 1),
                          0, nextlvl, m.model});
      nodes_.back().next_sibling = first;

      if (num_children > 1)
        {
          bool abort = false;
          if (is_accepting)
            {
              has_rabin_shape_ = false;
              if (!!(opt & zielonka_tree_options::ABORT_WRONG_SHAPE)
                  && !!(opt & zielonka_tree_options::CHECK_RABIN))
                abort = true;
            }
          else
            {
              has_streett_shape_ = false;
              if (!!(opt & zielonka_tree_options::ABORT_WRONG_SHAPE)
                  && !!(opt & zielonka_tree_options::CHECK_STREETT))
                abort = true;
            }
          if (abort)
            {
              nodes_.clear();
              num_branches_ = 0;
              return;
            }
        }
    }

    bool empty_is_accepting = code.accepting(acc_cond::mark_t{});
    empty_is_even_ = empty_is_accepting == is_even_;
  }

  void zielonka_tree::dot(std::ostream& os) const
  {
    os << "digraph zielonka_tree {\n";
    unsigned ns = nodes_.size();
    for (unsigned n = 0; n < ns; ++n)
      {
        os << "  " << n << " [label=\"" << nodes_[n].colors;
        unsigned first_child = nodes_[n].first_child;
        if (!first_child)
            os << "\n<" << n << '>';
        os << "\", shape="
           << (((nodes_[n].level & 1) ^ is_even_) ? "ellipse" : "box")
           << "]\n";
        if (first_child)
          {
            unsigned child = first_child;
            do
              {
                os << "  " << n << " -> " << child << '\n';
                child = nodes_[child].next_sibling;
              }
            while (child != first_child);
          }
      }
    os << "}\n";
  }

  std::pair<unsigned, unsigned>
  zielonka_tree::step(unsigned branch, acc_cond::mark_t colors) const
  {
    if (SPOT_UNLIKELY(nodes_.size() <= branch || nodes_[branch].first_child))
      throw std::runtime_error
        ("zielonka_tree::step(): incorrect branch number");

    if (!colors)
      {
        unsigned lvl = nodes_[branch].level;
        return {branch, lvl + ((lvl & 1) == empty_is_even_)};
      }
    unsigned child = 0;
    for (;;)
      {
        colors -= nodes_[branch].colors;
        if (!colors)
          break;
        child = branch;
        branch = nodes_[branch].parent;
      }
    unsigned lvl = nodes_[branch].level;
    if (child != 0)
      {
        // The following computation could be precomputed.
        branch = nodes_[child].next_sibling;
        while (nodes_[branch].first_child)
          branch = nodes_[branch].first_child;
      }
    return {branch, lvl};
  }

  namespace
  {
    // A state in the zielonka_tree_transform or acd_transform outputs
    // corresponds to a state in the input associated to a branch of
    // the tree.
    typedef std::pair<unsigned, unsigned> zlk_state;

    struct zlk_state_hash
    {
      size_t
      operator()(const zlk_state& s) const noexcept
      {
        return wang32_hash(s.first ^ wang32_hash(s.second));
      }
    };
  }

  twa_graph_ptr
  zielonka_tree_transform(const const_twa_graph_ptr& a)
  {
    auto res = make_twa_graph(a->get_dict());
    res->copy_ap_of(a);
    auto& acc = a->get_acceptance();
    zielonka_tree zlk(acc);
    acc_cond::mark_t mask = acc.used_sets();

    // Preserve determinism, weakness, and stutter-invariance
    res->prop_copy(a, { false, true, true, true, true, true });

    auto orig_states = new std::vector<unsigned>();
    auto branches = new std::vector<unsigned>();
    unsigned ns = a->num_states();
    orig_states->reserve(ns); // likely more are needed.
    res->set_named_prop("original-states", orig_states);
    res->set_named_prop("degen-levels", branches);

    // Associate each zlk_state to its number.
    typedef std::unordered_map<zlk_state, unsigned, zlk_state_hash> zs2num_map;
    zs2num_map zs2num;

    // Queue of states to be processed.
    std::deque<zlk_state> todo;

    auto new_state = [&](zlk_state zs)
    {
      if (auto i = zs2num.find(zs); i != zs2num.end())
        return i->second;

      unsigned ns = res->new_state();
      zs2num[zs] = ns;
      todo.emplace_back(zs);

      unsigned orig = zs.first;
      assert(ns == orig_states->size());
      orig_states->emplace_back(orig);
      branches->emplace_back(zs.second);
      return ns;
    };

    zlk_state s(a->get_init_state_number(), zlk.first_branch());
    new_state(s);
    unsigned max_color = 0;
    while (!todo.empty())
      {
        s = todo.front();
        todo.pop_front();
        int src = zs2num[s];
        unsigned branch = s.second;

        for (auto& i: a->out(s.first))
          {
            auto [newbranch, prio] = zlk.step(branch, i.acc & mask);
            zlk_state d(i.dst, newbranch);
            unsigned dst = new_state(d);
            max_color = std::max(max_color, prio);
            res->new_edge(src, dst, i.cond, {prio});
          }
      }

    res->set_acceptance(max_color + 1,
                        acc_cond::acc_code::parity_min(!zlk.is_even(),
                                                       max_color + 1));

    // compose original-states with the any previously existing one.
    // We do that now, because for the bottommost copy below, it's better
    // if we compose everything.
    if (auto old_orig_states =
        a->get_named_prop<std::vector<unsigned>>("original-states"))
      for (auto& s: *orig_states)
        s = (*old_orig_states)[s];

    // Now we will iterate over the SCCs in topological order to
    // remember the "bottommost" SCCs that contain each original
    // state.  If an original state is duplicated in a higher SCC,
    // it can be shunted away.  Amen.
    scc_info si_res(res, scc_info_options::TRACK_STATES);
    unsigned res_scc_count = si_res.scc_count();
    unsigned maxorig = *std::max_element(orig_states->begin(),
                                         orig_states->end());
    std::vector<unsigned> bottommost_occurence(maxorig + 1);
    {
      unsigned n = res_scc_count;
      do
        for (unsigned s: si_res.states_of(--n))
          bottommost_occurence[(*orig_states)[s]] = s;
      while (n);
    }
    unsigned res_ns = res->num_states();
    std::vector<unsigned> retarget(res_ns);
    for (unsigned n = 0; n < res_ns; ++n)
      {
        unsigned other = bottommost_occurence[(*orig_states)[n]];
        retarget[n] =
          (si_res.scc_of(n) != si_res.scc_of(other)) ? other : n;
      }
    for (auto& e: res->edges())
      e.dst = retarget[e.dst];
    res->set_init_state(retarget[res->get_init_state_number()]);
    res->purge_unreachable_states();

    return res;
  }

  void acd::report_invalid_scc_number(unsigned num, const char* fn)
  {
    throw std::runtime_error(std::string(fn) +
                             "(): SCC number " + std::to_string(num)
                             + " is too large");
  }

  void acd::report_need_opt(const char* opt)
  {
    throw std::runtime_error("ACD should be built with option "s + opt);
  }

  void acd::report_empty_acd(const char* fn)
  {
    throw
      std::runtime_error(std::string(fn) +
                         "(): ACD is empty, did you use ABORT_WRONG_SHAPE?");
  }

  acd::acd(const const_twa_graph_ptr& aut, acd_options opt)
    : si_(new scc_info(aut)), own_si_(true), opt_(opt), trees_(si_->scc_count())
  {
    build_();
  }

  acd::acd(const scc_info& si, acd_options opt)
    : si_(&si), own_si_(false), opt_(opt), trees_(si_->scc_count())
  {
    build_();
  }

  acd::~acd()
  {
    if (own_si_)
      delete si_;
  }

  void acd::build_()
  {
    unsigned scc_count = scc_count_ = si_->scc_count();
    const_twa_graph_ptr aut = aut_ = si_->get_aut();
    unsigned nedges = aut->get_graph().edge_vector().size();
    unsigned nstates = aut->num_states();
    acc_cond posacc = aut->acc();
    acc_cond negacc(posacc.num_sets(), posacc.get_acceptance().complement());

    // Cache for nodes.  The first instance of any node that
    // has {colors,minstate} is given in this map.  This can be used
    // to speedup the computation of successors of other nodes
    // with the same parameters.
    typedef std::pair<acc_cond::mark_t, unsigned> nmap_key_t;
    struct nmap_key_hash
    {
      std::size_t
      operator() (const std::pair<acc_cond::mark_t, unsigned> &pair) const
      {
        return pair.first.hash() ^ std::hash<unsigned>()(pair.second);
      }
    };
    robin_hood::unordered_node_map<nmap_key_t, unsigned, nmap_key_hash> nmap;

    if (nstates <= 1)
      // The ordering heuristic does not help if we have a single state.
      opt_ = opt_ - acd_options::ORDER_HEURISTIC;

    // The bitvectors store edge and state-vectors that are shared
    // among the different trees.
    auto allocate_vectors_maybe = [&](unsigned n)
    {
      if (bitvectors.size() > 2 * n)
        return;
      bitvectors.emplace_back(std::unique_ptr<bitvect>(make_bitvect(nedges)));
      bitvectors.emplace_back(std::unique_ptr<bitvect>(make_bitvect(nstates)));
    };
    auto edge_vector = [&] (unsigned n) -> bitvect&
    {
      return *bitvectors[2 * n];
    };
    auto state_vector = [&] (unsigned n) -> bitvect&
    {
      return *bitvectors[2 * n + 1];
    };
    allocate_vectors_maybe(0);

    // Remember the max level since of each tree of different parity.
    // We will use that to decide if the output should have parity
    // "min even" or "min odd" so as to minimize the number of colors
    // used.
    int max_level_of_even_tree = -1;
    int max_level_of_odd_tree = -1;

    for (unsigned scc = 0; scc < scc_count; ++scc)
      {
        if ((trees_[scc].trivial = si_->is_trivial(scc)))
          continue;
        trees_[scc].num_nodes = 1;
        unsigned root = nodes_.size();
        trees_[scc].root = root;
        bool is_even = si_->is_maximally_accepting_scc(scc);
        trees_[scc].is_even = is_even;
        nodes_.emplace_back(edge_vector(0), state_vector(0));
        auto& n = nodes_.back();
        n.parent = root;
        n.level = 0;
        n.scc = scc;
        n.colors = si_->acc_sets_of(scc);
        n.minstate = si_->one_state_of(scc);
        for (auto& e: si_->inner_edges_of(scc))
          {
            n.edges.set(aut->edge_number(e));
            n.states.set(e.src);
          }
      }

    struct size_model
    {
      unsigned size;
      std::unique_ptr<bitvect> trans;
      acc_cond::mark_t colors;
      unsigned minstate;
    };
    std::vector<size_model> out;

    auto handle_leaf = [&](unsigned scc, unsigned lvl)
    {
      // this node is a leaf.
      if (trees_[scc].is_even)
        max_level_of_even_tree = lvl;
      else
        max_level_of_odd_tree = lvl;
    };
    // Chain the children together, and connect them to the parent
    auto chain_children = [this](unsigned node, unsigned before, unsigned after)
    {
      // Chain the children together, and connect them to the parent
      for (unsigned child = before; child < after; ++child)
        {
          unsigned next = child + 1;
          if (next == after)
            {
              next = before;
              nodes_[node].first_child = before;
            }
          nodes_[child].next_sibling = next;
        }
    };

    std::unique_ptr<bitvect> seen_src;
    std::unique_ptr<bitvect> seen_dst;
    std::unique_ptr<bitvect> seen_dup;
    if (!!(opt_ & acd_options::ORDER_HEURISTIC))
      {
        seen_src.reset(make_bitvect(nstates));
        seen_dst.reset(make_bitvect(nstates));
        seen_dup.reset(make_bitvect(nstates));
      }
    // This loop is a BFS over the increasing set of nodes.
    for (unsigned node = 0; node < nodes_.size(); ++node)
    {
      auto& nn = nodes_[node];
      unsigned scc = nn.scc;
      unsigned lvl = nn.level;
      bool accepting_node = (lvl & 1) != trees_[scc].is_even;

      // If we have already computed an SCC restricted to nn.colors and using
      // nn.minstate as smallest state, then simply copy its successors.
      if (auto it = nmap.find({nn.colors, nn.minstate}); it != nmap.end())
        {
          auto& ref = nodes_[it->second];
          unsigned fc = ref.first_child;
          if (!fc)
            {
              handle_leaf(scc, lvl);
              continue;
            }
          unsigned child = fc;
          unsigned before = nodes_.size();
          do
            {
              auto& c = nodes_[child];
              // We have to read anything we need from C
              // before emplace_back, which may reallocate.
              acc_cond::mark_t colors = c.colors;
              unsigned minstate = c.minstate;
              child = c.next_sibling;
              nodes_.emplace_back(c.edges, c.states);
              auto& n = nodes_.back();
              n.parent = node;
              n.level = lvl + 1;
              n.scc = ref.scc;
              n.colors = colors;
              n.minstate = minstate;
            }
          while (child != fc);
          chain_children(node, before, nodes_.size());
          continue;
        }
      else
        {
          nmap.emplace(nmap_key_t{nn.colors, nn.minstate}, node);
        }

      out.clear();
      auto callback = [&](scc_info si, unsigned siscc)
      {
        bitvect* bv = make_bitvect(nedges);
        unsigned sz = 0;
        acc_cond::mark_t colors = si.acc_sets_of(siscc);
        unsigned minstate = -1U;
        for (auto& e: si.inner_edges_of(siscc))
          {
            bv->set(aut->edge_number(e));
            ++sz;
            minstate = std::min(minstate, e.src);
          }
        auto iter = out.begin();
        while (iter != out.end())
          {
            if (iter->size < sz)
              // We have checked all larger models.
              break;
            // Checking inclusion of edges is enough,
            // but checking inclusion of colors is way faster
            if (colors.subset(iter->colors)
                && (minstate == iter->minstate
                    || bv->is_subset_of(*iter->trans)))
              // ignore smaller models
              {
                delete bv;
                return;
              }
            ++iter;
          }
        // insert the model
        iter = out.insert(iter, {sz, std::unique_ptr<bitvect>(bv),
                                 colors, minstate});
        ++iter;
        // erase all models it contains
        out.erase(std::remove_if(iter, out.end(),
                                 [&](auto& mod) {
                                   return mod.trans->is_subset_of(*bv);
                                 }), out.end());
      };
      maximal_accepting_loops_for_scc(*si_, scc,
                                      accepting_node ? negacc : posacc,
                                      nodes_[node].edges, callback);

      if (!!(opt_ & acd_options::ORDER_HEURISTIC))
        {
          // Find states that appear in multiple children
          seen_dup->clear_all(); // duplicate states
          seen_src->clear_all(); // union of all children states
          for (auto& [sz, bv, colors, minstate]: out)
            {
              (void) sz;
              (void) colors;
              (void) minstate;
              seen_dst->clear_all(); // local states of the node
              bv->foreach_set_index([&aut, &seen_dst](unsigned e)
              {
                seen_dst->set(aut->edge_storage(e).src);
              });
              seen_dup->add_common(*seen_src, *seen_dst);
              *seen_src |= *seen_dst;
            }
          if (seen_dup->is_fully_clear())
            goto skip_sort;
          // Now the union in seen_src is not useful anymore.  Process
          // each node again, but consider only the states that are in
          // seen_dup.
          for (auto& [sz, bv, colors, minstate]: out)
            {
              (void) sz;
              (void) colors;
              (void) minstate;
              seen_src->clear_all(); // local source of the node
              bv->foreach_set_index([&aut, &seen_src](unsigned e)
              {
                seen_src->set(aut->edge_storage(e).src);
              });
              seen_dst->clear_all();
              *seen_src &= *seen_dup;
              // Count the number of states reached by leaving this node.
              seen_src->foreach_set_index([&aut, bv=bv.get(),
                                           &seen_dst](unsigned s)
              {
                for (auto& e: aut->out(s))
                  if (!bv->get(aut->edge_number(e)))
                    seen_dst->set(e.dst);
              });
              sz = seen_dst->count();
            }
          // reorder by decreasing number of new states reached
          std::stable_sort(out.begin(), out.end(),
                           [&](auto& p1, auto& p2) {
                             return p1.size > p2.size;
                           });
        skip_sort:;
        }

      unsigned before_size = nodes_.size();
      for (const auto& [sz, bv, colors, minstate]: out)
        {
          (void) sz;
          (void) colors;
          (void) minstate;
          unsigned vnum = trees_[scc].num_nodes++;
          allocate_vectors_maybe(vnum);
          nodes_.emplace_back(edge_vector(vnum), state_vector(vnum));
          auto& n = nodes_.back();
          n.parent = node;
          n.level = lvl + 1;
          n.scc = scc;
          n.edges |= *bv;
          n.colors = colors;
          n.minstate = minstate;
        }
      unsigned after_size = nodes_.size();
      unsigned children = after_size - before_size;
      chain_children(node, before_size, after_size);

      if (children == 0)
        {
          handle_leaf(scc, lvl);
        }
      else if (children > 1)
        {
          // Check the shape if requested.
          if ((has_rabin_shape_
               && !!(opt_ & acd_options::CHECK_RABIN)
               && accepting_node)
              || (has_streett_shape_
                  && !!(opt_ & acd_options::CHECK_STREETT)
                  && !accepting_node))
            {
              std::unique_ptr<bitvect> seen(make_bitvect(nstates));
              std::unique_ptr<bitvect> cur(make_bitvect(nstates));
              for (const auto& [sz, bv, colors, minstate]: out)
                {
                  (void) sz;
                  (void) colors;
                  (void) minstate;
                  cur->clear_all();
                  bv->foreach_set_index([&aut, &cur](unsigned e)
                  {
                    cur->set(aut->edge_storage(e).src);
                  });
                  if (cur->intersects(*seen))
                    {
                      if (accepting_node)
                        has_rabin_shape_ = false;
                      else
                        has_streett_shape_ = false;
                      if (!!(opt_ & acd_options::ABORT_WRONG_SHAPE))
                        {
                          nodes_.clear();
                          return;
                        }
                      break;
                    }
                  *seen |= *cur;
                }
            }
        }
    }
    // Now we decide if the ACD corresponds to a "min even" or "max
    // even" parity.  We want to minimize the number of colors
    // introduced (because of Spot's limitation to a fixed number of
    // those), so the parity of the tallest tree will give the parity
    // of the ACD.
    bool is_even = is_even_ = max_level_of_even_tree >= max_level_of_odd_tree;
    // add one to the level of each node belonging to a tree of the
    // opposite parity
    for (auto& node: nodes_)
      {
        unsigned scc = node.scc;
        if (trees_[scc].is_even != is_even)
          ++node.level;
        trees_[scc].max_level = std::max(trees_[scc].max_level, node.level);
      }
    // Compute the set of states hit by each edge vector.
    // Skip the first ones, because they are used in the roots of each tree
    // and have already been filled.
    unsigned nv = bitvectors.size();
    for (unsigned n = 2; n < nv; n += 2)
      bitvectors[n]->foreach_set_index([&aut,
                                        &states=*bitvectors[n+1]](unsigned e)
      {
        states.set(aut->edge_storage(e).src);
      });
  }

  unsigned acd::leftmost_branch_(unsigned n, unsigned state) const
  {
  loop:
    unsigned first_child = nodes_[n].first_child;
    if (first_child == 0)   // no children
      return n;

    unsigned child = first_child;
    do
      {
        if (nodes_[child].states.get(state))
          {
            n = child;
            goto loop;
          }
        child = nodes_[child].next_sibling;
      }
    while (child != first_child);
    return n;
  }


  unsigned acd::first_branch(unsigned s) const
  {
    if (SPOT_UNLIKELY(aut_->num_states() < s))
      throw std::runtime_error("acd::first_branch(): unknown state " +
                               std::to_string(s));
    unsigned scc = si_->scc_of(s);
    if (trees_[scc].trivial)    // the branch is irrelevant for transient SCCs
      return 0;
    if (SPOT_UNLIKELY(nodes_.empty()))
      // make sure we do not complain about this if all SCCs are trivial.
      report_empty_acd("acd::first_branch");
    unsigned n = trees_[scc].root;
    assert(nodes_[n].states.get(s));
    return leftmost_branch_(n, s);
  }


  std::pair<unsigned, unsigned>
  acd::step(unsigned branch, unsigned edge) const
  {
    if (SPOT_UNLIKELY(nodes_.size() <= branch))
      throw std::runtime_error("acd::step(): incorrect branch number");
    if (SPOT_UNLIKELY(nodes_[branch].edges.size() < edge))
      throw std::runtime_error("acd::step(): incorrect edge number");

    unsigned child = 0;
    unsigned dst = aut_->edge_storage(edge).dst;
    while (!nodes_[branch].edges.get(edge))
      {
        unsigned parent = nodes_[branch].parent;
        if (SPOT_UNLIKELY(branch == parent))
          {
            // We are changing SCC, so the level emitted does not
            // matter.
            assert(si_->scc_of(aut_->edge_storage(edge).src)
                   != si_->scc_of(dst));
            return { first_branch(dst), 0 };
          }
        child = branch;
        branch = parent;
      }
    unsigned lvl = nodes_[branch].level;
    if (child == 0)             // came here without climbing up
      return { leftmost_branch_(branch, dst), lvl };

    unsigned start_child = child;
    // find the next children that contains dst.
    do
      {
        child = nodes_[child].next_sibling;
        if (nodes_[child].states.get(dst))
          return {leftmost_branch_(child, dst), lvl};
      }
    while (child != start_child);
    return { branch, lvl };
  }

  unsigned acd::state_step(unsigned node, unsigned edge) const
  {
    // The rule is almost similar to step(), except we do note
    // go down to the leftmost leave after we have seen a round
    // of all children.
    if (SPOT_UNLIKELY(nodes_.size() <= node))
      throw std::runtime_error("acd::step(): incorrect node number");
    if (SPOT_UNLIKELY(nodes_[node].edges.size() < edge))
      throw std::runtime_error("acd::step(): incorrect edge number");

    unsigned child = 0;
    auto& es = aut_->edge_storage(edge);
    unsigned dst = es.dst;
    unsigned src = es.src;
    // If we are not on a leaf of the subtree of src, go there before
    // continuing.
    node = leftmost_branch_(node, src);
    while (!nodes_[node].edges.get(edge))
      {
        unsigned parent = nodes_[node].parent;
        if (SPOT_UNLIKELY(node == parent))
          {
            // Changing SCc
            return first_branch(dst);
          }
        child = node;
        node = parent;
      }
    if (child == 0)             // came here without climbing up
      return leftmost_branch_(node, dst);
    unsigned start_child = child;
    // find the next children that contains dst.
    do
      {
        child = nodes_[child].next_sibling;
        if (nodes_[child].states.get(dst))
          {
            if (child <= start_child)
              return node; // full loop of children done
            else
              return child;
          }
      }
    while (child != start_child);
    return node;
  }


  void acd::dot(std::ostream& os, const char* id) const
  {
    unsigned ns = nodes_.size();
    os << "digraph acd {\n  labelloc=\"t\"\n  label=\"\n"
       << (ns ? (is_even_ ? "min even\"" : "min odd\"") : "empty ACD\"");
    if (id)
      escape_str(os << "  id=\"", id)
        // fill the node so that the entire node is clickable
        << "\"\n  node [id=\"N\\N\", style=filled, fillcolor=white]\n";
    for (unsigned n = 0; n < ns; ++n)
      {
        os << "  " << n << " [label=\"";
        unsigned scc = nodes_[n].scc;
        // The top of each tree has level 0 or 1, depending on whether
        // the tree's parity matches the overall ACD parity.
        if (nodes_[n].level == (trees_[scc].is_even != is_even_))
          os << "SCC #" << scc << '\n';
        // Prints the indices that are true in edges.  To save space,
        // we print span of 3 or more elements as start-end.
        auto& edges = nodes_[n].edges;
        unsigned nedges = edges.size();
        bool lastval = false;
        unsigned lastchange = 0;
        const char* sep = "T: ";
        for (unsigned n = 1; n <= nedges; ++n)
          {
            bool val = n < nedges && edges.get(n)
              && si_->scc_of(aut_->edge_storage(n).dst) == scc;
            if (val != lastval)
              {
                if (lastval)
                  switch (n - lastchange)
                    {
                    case 1:
                      break;
                    case 2:
                      os << ',' << n - 1;
                      break;
                    default:
                      os << '-' << n - 1;
                      break;
                    }
                else
                  os << sep << n;
                lastval = val;
                lastchange = n;
                sep = ",";
              }
          }
        unsigned first_child = nodes_[n].first_child;
        os << '\n' << nodes_[n].colors;
        auto& states = nodes_[n].states;
        unsigned nstates = states.size();
        sep = "\nQ: ";
        for (unsigned n = 0; n <= nstates; ++n)
          {
            bool val = n < nstates && states.get(n) && si_->scc_of(n) == scc;
            if (val != lastval)
              {
                if (lastval)
                  switch (n - lastchange)
                    {
                    case 1:
                      break;
                    case 2:
                      os << ',' << n - 1;
                      break;
                    default:
                      os << '-' << n - 1;
                      break;
                    }
                else
                  os << sep << n;
                lastval = val;
                lastchange = n;
                sep = ",";
              }
          }
        os << "\nlvl: " << nodes_[n].level;
        if (!first_child)
            os << "\n<" << n << '>';
        // use a fillcolor so that the entire node is clickable
        os << "\", shape="
           << (((nodes_[n].level & 1) ^ is_even_) ? "ellipse" : "box");
        if (id)
          {
            os << " class=\"";
            const char* sep = "";
            for (unsigned n = 0; n < nstates; ++n)
              if (states.get(n) && si_->scc_of(n) == scc)
                {
                  os << sep << "acdS" << n << '\n';
                  sep = " ";
                }
            os << '\"';
          }
        os << "]\n";
        if (first_child)
          {
            unsigned child = first_child;
            do
              {
                os << "  " << n << " -> " << child << '\n';
                child = nodes_[child].next_sibling;
              }
            while (child != first_child);
          }
      }
    os << "}\n";
  }

  bool acd::node_acceptance(unsigned n) const
  {
    if (SPOT_UNLIKELY(nodes_.size() <= n))
      throw std::runtime_error("acd::node_acceptance(): unknown node");
    return (nodes_[n].level & 1) ^ is_even_;
  }

  /// Return the level of a node.
  unsigned acd::node_level(unsigned n) const
  {
    if (SPOT_UNLIKELY(nodes_.size() <= n))
      throw std::runtime_error("acd::node_level(): unknown node");
    return nodes_[n].level;
  }

  const acc_cond::mark_t& acd::node_colors(unsigned n) const
  {
    if (SPOT_UNLIKELY(nodes_.size() <= n))
      throw std::runtime_error("acd::node_colors(): unknown node");
    return nodes_[n].colors;
  }

  std::vector<unsigned> acd::edges_of_node(unsigned n) const
  {
    if (SPOT_UNLIKELY(nodes_.size() <= n))
      throw std::runtime_error("acd::edges_of_node(): unknown node");
    std::vector<unsigned> res;
    unsigned scc = nodes_[n].scc;
    auto& edges = nodes_[n].edges;
    unsigned nedges = edges.size();
    for (unsigned e = 1; e < nedges; ++e)
      if (edges.get(e) && si_->scc_of(aut_->edge_storage(e).dst) == scc)
        res.push_back(e);
    return res;
  }

  bool acd::has_rabin_shape() const
  {
    if (!(opt_ & acd_options::CHECK_RABIN))
      report_need_opt("CHECK_RABIN");
    return has_rabin_shape_;
  }

  bool acd::has_streett_shape() const
  {
    if (!(opt_ & acd_options::CHECK_STREETT))
      report_need_opt("CHECK_STREETT");
    return has_streett_shape_;
  }

  bool acd::has_parity_shape() const
  {
    if ((opt_ & acd_options::CHECK_PARITY) != acd_options::CHECK_PARITY)
      report_need_opt("CHECK_PARITY");
    return has_streett_shape_ && has_rabin_shape_;
  }

  // This handle both the transition-based and state-based version of
  // ACD, to
  template<bool sbacc>
  twa_graph_ptr
  acd_transform_(const const_twa_graph_ptr& a, bool colored,
                 acd_options options = acd_options::NONE)
  {
    auto res = make_twa_graph(a->get_dict());
    res->copy_ap_of(a);
    scc_info si(a, scc_info_options::TRACK_STATES);
    acd theacd(si, options);

    // If we desire non-colored output, we can omit the maximal
    // color of each SCC if it has the same parity as max_level.
    unsigned max_level = 0;
    if (!colored)
      {
        unsigned ns = si.scc_count();
        for (unsigned n = 0; n < ns; ++n)
          max_level = std::max(max_level, theacd.scc_max_level(n));
      }
    bool max_level_is_odd = max_level & 1;

    // Preserve determinism, and stutter-invariance.
    // state-based acceptance is lost,
    // inherently-weak automata become weak.
    res->prop_copy(a, { false, false, true, true, true, true });
    if constexpr (sbacc)
      res->prop_state_acc(true);

    auto orig_states = new std::vector<unsigned>();
    auto branches = new std::vector<unsigned>();
    unsigned ns = a->num_states();
    orig_states->reserve(ns); // likely more are needed.
    res->set_named_prop("original-states", orig_states);
    res->set_named_prop("degen-levels", branches);

    // Associate each zlk_state to its number.
    typedef std::unordered_map<zlk_state, unsigned, zlk_state_hash> zs2num_map;
    zs2num_map zs2num;

    // Queue of states to be processed.
    std::deque<zlk_state> todo;

    auto new_state = [&](zlk_state zs)
    {
      if (auto i = zs2num.find(zs); i != zs2num.end())
        return i->second;

      unsigned ns = res->new_state();
      zs2num[zs] = ns;
      todo.emplace_back(zs);

      unsigned orig = zs.first;
      assert(ns == orig_states->size());
      orig_states->emplace_back(orig);
      branches->emplace_back(zs.second);
      return ns;
    };

    unsigned init = a->get_init_state_number();
    zlk_state s(init, theacd.first_branch(init));
    new_state(s);
    unsigned max_color = 0;
    bool is_even = theacd.is_even();
    while (!todo.empty())
      {
        s = todo.front();
        todo.pop_front();
        int src = zs2num[s];
        unsigned branch = s.second;
        unsigned src_scc = si.scc_of(s.first);
        unsigned scc_max_lvl = theacd.scc_max_level(src_scc);
        bool scc_max_lvl_can_be_omitted = (scc_max_lvl & 1) == max_level_is_odd;
        unsigned src_prio;
        if constexpr (!sbacc)
          {
            (void)src_prio; // this is only used for sbacc
          }
        else
          {
            src_prio = si.is_trivial(src_scc) ?
              scc_max_lvl : theacd.node_level(branch);
            if (colored
                || !scc_max_lvl_can_be_omitted
                || src_prio != scc_max_lvl)
              max_color = std::max(max_color, src_prio);
          }
        for (auto& i: a->out(s.first))
          {
            unsigned newbranch;
            unsigned prio;
            if constexpr (sbacc) // All successor have the same colors.
              prio = src_prio;
            unsigned dst_scc = si.scc_of(i.dst);
            if (dst_scc != src_scc)
              {
                newbranch = theacd.first_branch(i.dst);
                if constexpr (!sbacc)
                  prio = 0;
              }
            else
              {
                if constexpr (!sbacc)
                  std::tie(newbranch, prio) =
                    theacd.step(branch, a->edge_number(i));
                else
                  newbranch = theacd.state_step(branch, a->edge_number(i));
              }
            zlk_state d(i.dst, newbranch);
            unsigned dst = new_state(d);
            if (!colored && ((!sbacc && dst_scc != src_scc)
                             || (scc_max_lvl_can_be_omitted
                                 && scc_max_lvl == prio)))
              {
                res->new_edge(src, dst, i.cond);
              }
            else
              {
                if constexpr (!sbacc)
                  max_color = std::max(max_color, prio);
                res->new_edge(src, dst, i.cond, {prio});
              }
          }
      }

    bool extra = colored || max_level > 0;
    res->set_acceptance(max_color + extra,
                        acc_cond::acc_code::parity_min(!is_even,
                                                       max_color + extra));

    // compose original-states with the any previously existing one.
    if (auto old_orig_states =
        a->get_named_prop<std::vector<unsigned>>("original-states"))
      for (auto& s: *orig_states)
        s = (*old_orig_states)[s];

    // An inherently-weak input necessarily becomes weak.
    if (a->prop_inherently_weak())
      res->prop_weak(true);
    return res;
  }

  twa_graph_ptr
  acd_transform(const const_twa_graph_ptr& a, bool colored)
  {
    return acd_transform_<false>(a, colored);
  }

  twa_graph_ptr
  acd_transform_sbacc(const const_twa_graph_ptr& a, bool colored,
                      bool order_heuristic)
  {
    return acd_transform_<true>(a, colored, order_heuristic
                                ? acd_options::ORDER_HEURISTIC
                                : acd_options::NONE);
  }

}
