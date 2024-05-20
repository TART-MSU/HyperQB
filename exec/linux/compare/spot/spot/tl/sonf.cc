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
#include <spot/tl/nenoform.hh>
#include <spot/tl/sonf.hh>

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace spot
{
  namespace
  {
    /// Uses `extractor` to extract some parts of the formula and replace them
    /// with atomic propositions.
    ///
    /// Returns (f & g1 & g2 & .. & gn) with g1..gn the extracted subformulas.
    ///
    /// `extractor` should be a lambda taking the following parameters as input:
    ///
    /// - `formula` the formula to process
    /// - `std::vector<formula>&` the vector that stores extracted subformulas
    /// - `auto&&` itself, in case it needs to call itself recursively
    ///   (formula::map for example)
    /// - `bool` a boolean indicating whether the lambda is currently being
    ///   called at the formula's "root"
    /// - `bool` a boolean indicating whether the lambda is currently being
    ///   called inside a toplevel `and` construct.
    ///
    /// Note that the last 2 boolean arguments can be used as you see fit in
    /// your recursive calls, the first one being set to true in the original
    /// call, and the second one to false.
    ///
    /// `extractor` should return the new rewritten formula.
    ///
    /// auto sample_extractor = [](formula f,
    ///                            std::vector<formula>& extracted,
    ///                            auto&& extractor,
    ///                            bool top_level,
    ///                            bool in_top_level_and) -> formula
    template<typename Ext>
    static formula
    extract(formula f, Ext extractor)
    {
      std::vector<formula> extracted;
      formula new_f = extractor(f, extracted, extractor, true, false);
      extracted.push_back(new_f);
      return formula::And(extracted);
    }
  }

  std::pair<formula, std::vector<std::string>>
  suffix_operator_normal_form(formula f, const std::string prefix)
  {
    // SONF can only be applied to formulas in negative normal form
    f = negative_normal_form(f);

    std::unordered_set<std::string> used_aps;
    std::vector<std::string> added_aps;
    size_t count = 0;

    // identify all used ap names to avoid them when generating new ones
    auto ap_indexer = [&used_aps](formula f) noexcept {
      if (f.is(op::ap))
        {
          used_aps.insert(f.ap_name());
          return true;
        }

      return false;
    };

    f.traverse(ap_indexer);

    auto new_ap_name =
      [&used_aps, &added_aps, &prefix, &count]() noexcept -> std::string
    {
      std::string new_name = prefix + std::to_string(count++);
      while (used_aps.find(new_name) != used_aps.end())
        new_name = prefix + std::to_string(count++);
      used_aps.insert(new_name);
      added_aps.push_back(new_name);
      return new_name;
    };

    // extracts the SERE part and replaces it with an atomic proposition,
    // storing the extracted formula in `extracted` and returning the rewritten
    // original formula
    auto sonf_extract = [&](formula f,
                            std::vector<formula>& extracted,
                            auto&& extractor,
                            bool top_level,
                            bool in_top_level_and) noexcept -> formula
    {
      const auto kind = f.kind();

      switch (kind)
        {
        case op::G:
          {
            // skip if shape is G(!ap | (regex []-> formula)) and at top level
            if ((top_level || in_top_level_and)
                && f[0].is(op::Or)     // G(_ | _)
                && f[0][0].is(op::Not)       // G(!_ | _)
                && f[0][0][0].is(op::ap)       // G(!ap | _)
                && f[0][1].is(op::EConcat, op::UConcat)) // G(!ap | (_ []-> _))
                return f;
            else
              return f.map(extractor, extracted, extractor, false, false);
          }
        case op::EConcat:
        case op::UConcat:
          {
            // recurse into rhs first (_ []-> rhs)
            formula rhs =
              f[1].map(extractor, extracted, extractor, false, false);
            f = formula::binop(kind, f[0], rhs);

            formula ap = formula::ap(new_ap_name());
            extracted.push_back(formula::G(formula::Or({formula::Not(ap), f})));
            return ap;
          }
        default:
          // tracking if we're in a op::And at the formula root
          in_top_level_and = top_level && f.is(op::And);
          return f.map(extractor, extracted, extractor,
                       false, in_top_level_and);
        }
    };

    f = extract(f, sonf_extract);

    auto ltl_extract = [&](formula f,
                           std::vector<formula>& extracted,
                           auto&& extractor,
                           [[maybe_unused]]
                           bool top_level,
                           [[maybe_unused]]
                           bool in_top_level_and) noexcept -> formula
    {
      switch (f.kind())
        {
        case op::EConcat:
        case op::UConcat:
          {
            formula rhs = f[1];

            if (rhs.is(op::ap))
              return f;

            formula ap = formula::ap(new_ap_name());
            extracted.push_back(
              formula::G(formula::Or({formula::Not(ap), rhs})));

            return formula::binop(f.kind(), f[0], ap);
          }
        default:
          return f.map(extractor, extracted, extractor, false, false);
        }
    };

    f = extract(f, ltl_extract);

    return {f, added_aps};
  }
}
