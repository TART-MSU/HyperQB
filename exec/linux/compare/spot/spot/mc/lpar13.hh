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

#include <atomic>
#include <spot/twa/acc.hh>
#include <spot/mc/unionfind.hh>
#include <spot/mc/intersect.hh>
#include <spot/mc/mc.hh>
#include <spot/misc/timer.hh>
#include <spot/twacube/twacube.hh>
#include <spot/twacube/fwd.hh>

namespace spot
{
  /// \brief This class implements the sequential emptiness check as
  /// presented in "Three SCC-based Emptiness Checks for Generalized
  /// Büchi Automata" (Renault et al, LPAR 2013). Among the three
  /// emptiness checks that have been proposed, we opted to implement
  /// yGabow's one.
  template<typename State, typename SuccIterator,
           typename StateHash, typename StateEqual>
  class SPOT_API lpar13
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
      operator()(const product_state that) const noexcept
      {
        // FIXME: wang32_hash(that.st_prop) could have been
        // pre-calculated!
        StateHash hasher;
        return  wang32_hash(that.st_prop) ^ hasher(that.st_kripke);
      }
    };

  public:

    using shared_map = int; // Useless here.
    using shared_struct = int; // Useless here.

    static shared_struct* make_shared_structure(shared_map m, unsigned i)
    {
      return nullptr; // Useless
    }

    lpar13(kripkecube<State, SuccIterator>& sys,
                     twacube_ptr twa,
                     shared_map& map, /* useless here */
                     shared_struct*, /* useless here */
                     unsigned tid,
                     std::atomic<bool>& stop)
      : sys_(sys), twa_(twa), tid_(tid), stop_(stop),
        acc_(twa->acc()), sccs_(0U)
      {
        static_assert(spot::is_a_kripkecube_ptr<decltype(&sys),
                      State, SuccIterator>::value,
                      "error: does not match the kripkecube requirements");
      }

    virtual ~lpar13()
    {
      map.clear();
      while (!todo.empty())
        {
          sys_.recycle(todo.back().it_kripke, tid_);
          todo.pop_back();
        }
    }

    bool run()
    {
      setup();
      product_state initial = {sys_.initial(tid_), twa_->get_initial()};
      if (SPOT_LIKELY(push_state(initial, dfs_number+1, {})))
        {
          todo.push_back({initial, sys_.succ(initial.st_kripke, tid_),
                          twa_->succ(initial.st_prop)});

          // Not going further! It's a product with a single state.
          if (todo.back().it_prop->done())
            return false;

          forward_iterators(sys_, twa_, todo.back().it_kripke,
                            todo.back().it_prop, true, 0);
          map[initial] = ++dfs_number;
        }
      while (!todo.empty() && !stop_.load(std::memory_order_relaxed))
        {
          // Check the kripke is enough since it's the outer loop. More
          // details in forward_iterators.
          if (todo.back().it_kripke->done())
            {
              bool is_init = todo.size() == 1;
              auto newtop = is_init? todo.back().st: todo[todo.size() -2].st;
              if (SPOT_LIKELY(pop_state(todo.back().st,
                                        map[todo.back().st],
                                        is_init,
                                        newtop,
                                        map[newtop])))
                {
                  sys_.recycle(todo.back().it_kripke, tid_);
                  // FIXME: a local storage for twacube iterator?
                  todo.pop_back();
                  if (SPOT_UNLIKELY(found_))
                    {
                      finalize();
                      return true;
                    }
                }
            }
          else
            {
              ++trans_;
              product_state dst =
                {
                 todo.back().it_kripke->state(),
                 twa_->trans_storage(todo.back().it_prop, tid_).dst
                };
              auto acc = twa_->trans_data(todo.back().it_prop, tid_).acc_;
              forward_iterators(sys_, twa_, todo.back().it_kripke,
                                todo.back().it_prop, false, 0);
              auto it  = map.find(dst);
              if (it == map.end())
                {
                  if (SPOT_LIKELY(push_state(dst, dfs_number+1, acc)))
                    {
                      map[dst] = ++dfs_number;
                      todo.push_back({dst, sys_.succ(dst.st_kripke, tid_),
                                      twa_->succ(dst.st_prop)});
                      forward_iterators(sys_, twa_, todo.back().it_kripke,
                                        todo.back().it_prop, true, 0);
                    }
                }
              else if (SPOT_UNLIKELY(update(todo.back().st,
                                            dfs_number,
                                            dst, map[dst], acc)))
                {
                  finalize();
                  return true;
                }
            }
        }
      finalize();
      return false;
    }

    void setup()
    {
      tm_.start("DFS thread " + std::to_string(tid_));
    }

    bool push_state(product_state, unsigned dfsnum, acc_cond::mark_t cond)
    {
      uf_.makeset(dfsnum);
      roots_.push_back({dfsnum, cond, {}});
      return true;
    }

    /// \brief This method is called to notify the emptiness checks
    /// that a state will be popped. If the method return false, then
    /// the state will be popped. Otherwise the state \a newtop will
    /// become the new top of the DFS stack. If the state \a top is
    /// the only one in the DFS stack, the parameter \a is_initial is set
    /// to true and both \a newtop and \a  newtop_dfsnum have inconsistency
    /// values.
    bool pop_state(product_state, unsigned top_dfsnum, bool,
                   product_state, unsigned)
    {
      if (top_dfsnum == roots_.back().dfsnum)
        {
          roots_.pop_back();
          ++sccs_;
          uf_.markdead(top_dfsnum);
        }
      dfs_ = todo.size()  > dfs_ ? todo.size() : dfs_;
      return true;
    }

    /// \brief This method is called for every closing, back, or forward edge.
    /// Return true if a counterexample has been found.
    bool update(product_state, unsigned,
                product_state, unsigned dst_dfsnum,
                acc_cond::mark_t cond)
    {
      if (uf_.isdead(dst_dfsnum))
        return false;

      while (!uf_.sameset(dst_dfsnum, roots_.back().dfsnum))
        {
          auto& el = roots_.back();
          roots_.pop_back();
          uf_.unite(dst_dfsnum, el.dfsnum);
          cond |= el.acc | el.ingoing;
        }
      roots_.back().acc |= cond;
      found_ = acc_.accepting(roots_.back().acc);
      if (SPOT_UNLIKELY(found_))
        stop_ = true;
      return found_;
    }

    void finalize()
    {
      bool tst_val = false;
      bool new_val = true;
      bool exchanged = stop_.compare_exchange_strong(tst_val, new_val);
      if (exchanged)
        finisher_ = true;
      tm_.stop("DFS thread " + std::to_string(tid_));
    }

    bool finisher()
    {
      return finisher_;
    }

    unsigned int states()
    {
      return dfs_number;
    }

    unsigned int transitions()
    {
      return trans_;
    }

    unsigned walltime()
    {
      return tm_.timer("DFS thread " + std::to_string(tid_)).walltime();
    }

    std::string name()
    {
      return "renault_lpar13";
    }

    int sccs()
    {
      return sccs_;
    }

    mc_rvalue result()
    {
      return !found_ ? mc_rvalue::EMPTY : mc_rvalue::NOT_EMPTY;
    }

    std::string trace()
    {
      SPOT_ASSERT(found_);
      std::string res = "Prefix:\n";

       // Compute the prefix of the accepting run
      for (auto& s : todo)
        res += "  " + std::to_string(s.st.st_prop) +
          + "*" + sys_.to_string(s.st.st_kripke) + "\n";

      // Compute the accepting cycle
      res += "Cycle:\n";

      struct ctrx_element
      {
        const product_state* prod_st;
        ctrx_element* parent_st;
        SuccIterator* it_kripke;
        std::shared_ptr<trans_index> it_prop;
      };
      std::queue<ctrx_element*> bfs;

      acc_cond::mark_t acc = {};

      bfs.push(new ctrx_element({&todo.back().st, nullptr,
              sys_.succ(todo.back().st.st_kripke, tid_),
              twa_->succ(todo.back().st.st_prop)}));
      while (true)
        {
        here:
          auto* front = bfs.front();
          bfs.pop();
          // PUSH all successors of the state.
          while (!front->it_kripke->done())
            {
              while (!front->it_prop->done())
                {
                  if (twa_->get_cubeset().intersect
                      (twa_->trans_data(front->it_prop, tid_).cube_,
                       front->it_kripke->condition()))
                    {
                      const product_state dst = {
                        front->it_kripke->state(),
                        twa_->trans_storage(front->it_prop).dst
                      };

                      // Skip Unknown states or not same SCC
                      auto it  = map.find(dst);
                      if (it == map.end() ||
                          !uf_.sameset(it->second,
                                       map[todo.back().st]))
                        {
                          front->it_prop->next();
                          continue;
                        }

                      // This is a valid transition. If this transition
                      // is the one we are looking for, update the counter-
                      // -example and flush the bfs queue.
                      auto mark = twa_->trans_data(front->it_prop,
                                                         tid_).acc_;
                      if (!(acc & mark))
                        {
                          ctrx_element* current = front;
                          while (current != nullptr)
                            {
                              // FIXME: also display acc?
                              res = res + "  " +
                                std::to_string(current->prod_st->st_prop) +
                                + "*" +
                                sys_. to_string(current->prod_st->st_kripke) +
                                "\n";
                              current = current->parent_st;
                            }

                          // empty the queue
                          while (!bfs.empty())
                            {
                              auto* e = bfs.front();
                              sys_.recycle(e->it_kripke, tid_);
                              bfs.pop();
                              delete e;
                            }
                          sys_.recycle(front->it_kripke, tid_);
                          delete front;

                          // update acceptance
                          acc |= mark;
                          if (twa_->acc().accepting(acc))
                            {
                              return res;
                            }

                          const product_state* q = &(it->first);
                          ctrx_element* root = new ctrx_element({
                              q , nullptr,
                              sys_.succ(q->st_kripke, tid_),
                              twa_->succ(q->st_prop)
                          });
                          bfs.push(root);
                          goto here;
                        }

                      // Otherwise increment iterator and push successor.
                      const product_state* q = &(it->first);
                      ctrx_element* root = new ctrx_element({
                          q , nullptr,
                          sys_.succ(q->st_kripke, tid_),
                          twa_->succ(q->st_prop)
                      });
                      bfs.push(root);
                    }
                  front->it_prop->next();
                }
              front->it_prop->reset();
              front->it_kripke->next();
            }
          sys_.recycle(front->it_kripke, tid_);
          delete front;
        }

      // never reach here;
      return res;
    }

  private:

    struct todo_element
    {
      product_state st;
      SuccIterator* it_kripke;
      std::shared_ptr<trans_index> it_prop;
    };

    struct root_element {
      unsigned dfsnum;
      acc_cond::mark_t ingoing;
      acc_cond::mark_t acc;
    };

    typedef std::unordered_map<const product_state, int,
                               product_state_hash,
                               product_state_equal> visited_map;

    kripkecube<State, SuccIterator>& sys_;
    twacube_ptr twa_;
    std::vector<todo_element> todo;
    visited_map map;
    unsigned int dfs_number = 0;
    unsigned int trans_ = 0;
    unsigned tid_;
    std::atomic<bool>& stop_;              ///< \brief Stop-the-world boolean
    bool found_ = false;
    std::vector<root_element> roots_;
    int_unionfind uf_;
    acc_cond acc_;
    unsigned sccs_;
    unsigned dfs_;
    spot::timer_map tm_;
    bool finisher_ = false;
  };
}
