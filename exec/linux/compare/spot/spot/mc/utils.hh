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

#pragma once

#include <spot/mc/intersect.hh>
#include <spot/twa/twa.hh>
#include <spot/twacube_algos/convert.hh>

namespace spot
{
  /// \brief convert a (cube) model  into a twa.
  /// Note that this algorithm cannot be run in parallel but could.
  template<typename State, typename SuccIterator,
           typename StateHash, typename StateEqual>
  class SPOT_API kripkecube_to_twa
  {
  public:

  kripkecube_to_twa(kripkecube<State, SuccIterator>& sys, bdd_dict_ptr dict):
    sys_(sys), dict_(dict)
    {
     static_assert(spot::is_a_kripkecube_ptr<decltype(&sys),
                   State, SuccIterator>::value,
                   "error: does not match the kripkecube requirements");
    }

    ~kripkecube_to_twa()
    {
     visited_.clear();
    }

    void run()
    {
     setup();
     State initial = sys_.initial(0);
     if (SPOT_LIKELY(push(initial, dfs_number_+1)))
       {
         visited_[initial] = dfs_number_++;
         todo_.push_back({initial, sys_.succ(initial, 0)});
       }
     while (!todo_.empty())
       {
         if (todo_.back().it->done())
           {
             if (SPOT_LIKELY(pop(todo_.back().s)))
               {
                 sys_.recycle(todo_.back().it, 0);
                 todo_.pop_back();
               }
           }
         else
           {
             ++transitions_;
             State dst = todo_.back().it->state();
             auto it = visited_.find(dst);
             if (it == visited_.end())
               {
                 if (SPOT_LIKELY(push(dst, dfs_number_+1)))
                   {
                     visited_[dst] = dfs_number_++;
                     todo_.back().it->next();
                     todo_.push_back({dst, sys_.succ(dst, 0)});
                   }
               }
             else
               {
                 edge(visited_[todo_.back().s], visited_[dst]);
                 todo_.back().it->next();
               }
           }
       }
     finalize();
    }

    void setup()
    {
     auto d = spot::make_bdd_dict();
     res_ = make_twa_graph(d);
     names_ = new std::vector<std::string>();

     int i = 0;
     for (auto ap : sys_.ap())
       {
         auto idx = res_->register_ap(ap);
         reverse_binder_[i++] = idx;
       }
    }

    bool push(State s, unsigned i)
    {

      unsigned st = res_->new_state();
      names_->push_back(sys_.to_string(s));
      if (!todo_.empty())
        {
          edge(visited_[todo_.back().s], st);
        }

      SPOT_ASSERT(st+1 == i);
      return true;
    }

    bool pop(State)
    {
      return true;
    }

    void edge(unsigned src, unsigned dst)
    {
     cubeset cs(sys_.ap().size());
     bdd cond = cube_to_bdd(todo_.back().it->condition(),
                            cs, reverse_binder_);
     res_->new_edge(src, dst, cond);
    }

    void finalize()
    {
     res_->purge_unreachable_states();
     res_->set_named_prop<std::vector<std::string>>("state-names", names_);
    }

    twa_graph_ptr twa()
    {
     return res_;
    }

  protected:
    struct todo__element
    {
      State s;
      SuccIterator* it;
    };

    typedef std::unordered_map<const State, int,
                               StateHash, StateEqual> visited__map;

    kripkecube<State, SuccIterator>& sys_;
    std::vector<todo__element> todo_;
    visited__map visited_;
    unsigned int dfs_number_ = 0;
    unsigned int transitions_ = 0;
    spot::twa_graph_ptr res_;
    std::vector<std::string>* names_;
    bdd_dict_ptr dict_;
    std::unordered_map<int, int> reverse_binder_;
  };

  /// \brief convert a (cube) product automaton into a twa
  /// Note that this algorithm cannot be run in parallel.
  template<typename State, typename SuccIterator,
           typename StateHash, typename StateEqual>
  class SPOT_API product_to_twa
  {
    struct product_state
    {
      State st_kripke;
      unsigned st_prop;
    };

    struct product_state_equal
    {
      bool
      operator()(const product_state lhs,
                 const product_state rhs) const
      {
        StateEqual equal;
        return (lhs.st_prop == rhs.st_prop) &&
          equal(lhs.st_kripke, rhs.st_kripke);
      }
    };

    struct product_state_hash
    {
      size_t
      operator()(const product_state lhs) const noexcept
      {
        StateHash hash;
        unsigned u = hash(lhs.st_kripke) % (1<<30);
        u = wang32_hash(lhs.st_prop) ^ u;
        u = u % (1<<30);
        return u;
      }
    };

  public:
    product_to_twa(kripkecube<State, SuccIterator>& sys,
                   twacube_ptr twa):
      sys_(sys), twa_(twa)
    {
      static_assert(spot::is_a_kripkecube_ptr<decltype(&sys),
                                             State, SuccIterator>::value,
                    "error: does not match the kripkecube requirements");
    }

    virtual ~product_to_twa()
    {
     map.clear();
    }

    bool run()
    {
      setup();
      product_state initial = {sys_.initial(0), twa_->get_initial()};
      if (SPOT_LIKELY(push_state(initial, dfs_number_+1, {})))
        {
          todo_.push_back({initial, sys_.succ(initial.st_kripke, 0),
                twa_->succ(initial.st_prop)});

          // Not going further! It's a product with a single state.
          if (todo_.back().it_prop->done())
            return false;

          forward_iterators(sys_, twa_, todo_.back().it_kripke,
                            todo_.back().it_prop, true, 0);
          map[initial] = ++dfs_number_;
        }
      while (!todo_.empty())
        {
          // Check the kripke is enough since it's the outer loop. More
          // details in forward_iterators.
          if (todo_.back().it_kripke->done())
            {
              bool is_init = todo_.size() == 1;
              auto newtop = is_init? todo_.back().st: todo_[todo_.size() -2].st;
              if (SPOT_LIKELY(pop_state(todo_.back().st,
                                             map[todo_.back().st],
                                             is_init,
                                             newtop,
                                             map[newtop])))
                {
                  sys_.recycle(todo_.back().it_kripke, 0);
                  todo_.pop_back();
                }
            }
          else
            {
              ++transitions_;
              product_state dst = {
                todo_.back().it_kripke->state(),
                twa_->trans_storage(todo_.back().it_prop, 0).dst
              };
              auto acc = twa_->trans_data(todo_.back().it_prop, 0).acc_;
              forward_iterators(sys_, twa_, todo_.back().it_kripke,
                                todo_.back().it_prop, false, 0);
              auto it  = map.find(dst);
              if (it == map.end())
                {
                  if (SPOT_LIKELY(push_state(dst, dfs_number_+1, acc)))
                    {
                      map[dst] = ++dfs_number_;
                      todo_.push_back({dst, sys_.succ(dst.st_kripke, 0),
                            twa_->succ(dst.st_prop)});
                      forward_iterators(sys_, twa_, todo_.back().it_kripke,
                                        todo_.back().it_prop, true, 0);
                    }
                }
              else if (SPOT_UNLIKELY(update(todo_.back().st, dfs_number_,
                                            dst, map[dst], acc)))
                return true;
            }
        }
      return false;
    }

    twa_graph_ptr twa()
    {
      res_->purge_unreachable_states();
      res_->set_named_prop<std::vector<std::string>>("state-names", names_);
      return res_;
    }

    void setup()
    {
      auto d = spot::make_bdd_dict();
      res_ = make_twa_graph(d);
      names_ = new std::vector<std::string>();

      int i = 0;
      for (auto ap : sys_.ap())
        {
          auto idx = res_->register_ap(ap);
          reverse_binder_[i++] = idx;
        }
    }

    bool push_state(product_state s, unsigned i, acc_cond::mark_t)
    {
      unsigned st = res_->new_state();

      if (!todo_.empty())
        {
          auto c = twa_->get_cubeset()
            .intersection(twa_->trans_data
                          (todo_.back().it_prop).cube_,
                          todo_.back().it_kripke->condition());

          bdd x = spot::cube_to_bdd(c, twa_->get_cubeset(),
                                    reverse_binder_);
          twa_->get_cubeset().release(c);
          res_->new_edge(map[todo_.back().st]-1, st, x,
                         twa_->trans_data
                         (todo_.back().it_prop).acc_);
        }


      names_->push_back(sys_.to_string(s.st_kripke) +
                        ('*' + std::to_string(s.st_prop)));
      SPOT_ASSERT(st+1 == i);
      return true;
    }

    bool update(product_state, unsigned src,
                product_state, unsigned dst,
                acc_cond::mark_t cond)
    {
      auto c = twa_->get_cubeset()
        .intersection(twa_->trans_data
                      (todo_.back().it_prop).cube_,
                      todo_.back().it_kripke->condition());

      bdd x = spot::cube_to_bdd(c, twa_->get_cubeset(),
                                reverse_binder_);
      twa_->get_cubeset().release(c);
      res_->new_edge(src-1, dst-1, x, cond);
      return false;
    }

    bool pop_state(product_state, unsigned, bool, product_state, unsigned)
    {
      return true;
    }

  private:
    struct todo__element
    {
      product_state st;
      SuccIterator* it_kripke;
      std::shared_ptr<trans_index> it_prop;
    };

    typedef std::unordered_map<const product_state, int,
                               product_state_hash,
                               product_state_equal> visited_map;

    kripkecube<State, SuccIterator>& sys_;
    twacube_ptr twa_;
    std::vector<todo__element> todo_;
    visited_map map;
    unsigned int dfs_number_ = 0;
    unsigned int transitions_ = 0;
    spot::twa_graph_ptr res_;
    std::vector<std::string>* names_;
    std::unordered_map<int, int> reverse_binder_;
  };
}
