// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details. (LRE).
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

#include <spot/priv/partitioned_relabel.hh>


relabeling_map
bdd_partition::to_relabeling_map(twa_graph& for_me) const
{
  relabeling_map res;
  // Change to unordered_map?
  bdd_dict_ptr bdddict = for_me.get_dict();

  bool use_inner = ig->state_storage(0).new_label != bddfalse;
  std::vector<bool> doskip
      = use_inner ? std::vector<bool>(ig->num_states(), false)
                  : std::vector<bool>();

  auto bdd2form = [&bdddict](const bdd& cond)
    {
      return bdd_to_formula(cond, bdddict);
    };

  for (const auto& [old_letter, s] : treated)
    {
      formula new_letter_form = bdd2form(ig->state_storage(s).new_label);
      assert(res.count(new_letter_form) == 0);
      if (use_inner)
          doskip[s] = true;
      res[new_letter_form] = bdd2form(old_letter);
    }

  if (use_inner)
    {
      // This implies that the split option was false,
      // so we can store further info
      auto& all_cond = *all_cond_ptr;
      const unsigned Norig = all_cond.size();

      for (unsigned i = 0; i < Norig; ++i)
        {
          // Internal node -> new ?
          if (doskip[i])
              continue;
          // Leave node -> already exists
          if (ig->state_storage(i).succ == 0)
              continue;
          doskip[i] = true;
          formula new_letter_form
              = bdd2form(ig->state_storage(i).new_label);
#ifdef NDEBUG
          res[new_letter_form] = bdd2form(all_cond[i]);
#else
          // Check if they are the same
          formula old_form = bdd2form(all_cond[i]);
          if (res.count(new_letter_form) == 0)
              res[new_letter_form] = old_form;
          else
              assert(res[new_letter_form] == old_form);
#endif
        }
      }
  return res;
}

/// \brief Tries to partition the given condition vector \a all_cond
/// abandons at \a max_letter.
/// \return The corresponding bdd_partition
/// \note A pointer to all_cond is captured internally, it needs
/// to outlive the returned bdd_partition
bdd_partition
try_partition_me(const std::vector<bdd>& all_cond, unsigned max_letter)
{
  // We create vector that will be successively filled.
  // Each entry corresponds to a "letter", of the partition
  const size_t Norig = all_cond.size();

  bdd_partition result(all_cond);

  auto& treated = result.treated;
  auto& ig = *result.ig;

  for (unsigned io = 0; io < Norig; ++io)
    {
      bdd cond = all_cond[io];
      const auto Nt = treated.size();
      for (size_t in = 0; in < Nt; ++in)
        {
          if (cond == bddfalse)
            break;
          if (treated[in].first == cond)
            {
              // Found this very condition -> make transition
              ig.new_edge(io, treated[in].second);
              cond = bddfalse;
              break;
            }
          if (bdd_have_common_assignment(treated[in].first, cond))
            {
              bdd inter = treated[in].first & cond;
              // Create two new states
              unsigned ssplit = ig.new_states(2);
              // ssplit becomes the state without the intersection
              // ssplit + 1 becomes the intersection
              // Both of them are implied by the original node,
              // Only inter is implied by the current letter
              ig.new_edge(treated[in].second, ssplit);
              ig.new_edge(treated[in].second, ssplit+1);
              ig.new_edge(io, ssplit+1);
              treated.emplace_back(inter, ssplit+1);
              // Update
              cond -= inter;
              treated[in].first -= inter;
              treated[in].second = ssplit;
              if (treated.size() > max_letter)
                return bdd_partition{};
            }
        }
        if (cond != bddfalse)
          {
            unsigned sc = ig.new_state();
            treated.emplace_back(cond, sc);
            ig.new_edge(io, sc);
          }
    }

  result.relabel_succ = true;
  return result;
}