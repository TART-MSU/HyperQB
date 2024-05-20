/* -*- coding: utf-8 -*-
** Copyright (C) by the Spot authors, see the AUTHORS file for details.
**
** This file is part of Spot, a model checking library.
**
** Spot is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** Spot is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
** License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
%require "3.3"
%language "C++"
%locations
%defines
%define api.prefix {tlyy}
%define api.value.type variant
%define api.value.automove true
%debug
%define parse.error verbose
%expect 0
%lex-param { spot::parse_error_list& error_list }
%define api.location.type {spot::location}

%code requires
{
#include "config.h"
#include <string>
#include <sstream>
#include <variant>
#include <spot/tl/parse.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/print.hh>

  struct minmax_t { unsigned min, max; };

  // pnode (parsing node) is simular to fnode (formula node) except
  // that n-ary operators will delay their construction until all
  // children are known; this is a hack to speedup the parsing,
  // because n-ary operator usually do a lot of work on construction
  // (sorting all children if the operator is commutative, removing
  // duplicates if applicable, etc.).  Building n-ary nodes by
  // repeatedly calling the binary constructor as we did in the past
  // has a prohibitive cost.  See issue #500.

  struct nary
  {
    std::vector<const spot::fnode*> children;
    spot::op kind;
  };

  struct pnode
  {
    // Hold either a constructed formula, or an n-ary operator that we
    // will construct only when it is combined with a different
    // operator.
    std::variant<const spot::fnode*, nary> data;
    // Record whether this pnode has been transformed into a fnode( or
    // moved to another pnode).  If that occurred, the ownership of
    // any fnode we store has been transfered to the constructed fnode
    // (or to the other pnode), and our destructor has nothing to do.
    // This is the usual case while parsing a formula without error.
    // However during error recovering, the parser may have to discard
    // unused pnode, in which case we have to remember to free fnode
    // during destruction.
    //
    // We have to track this used status because pnode are destructed
    // whenever the parser pops a token, and as of Bison 3.7.6, the
    // handling of "%destructor" is broken when
    // "%define api.value.type variant" is used.  See
    // https://lists.gnu.org/archive/html/bug-bison/2022-03/msg00000.html
    bool used = false;

    pnode()
      : data(nullptr)
    {
    }

    pnode(const spot::fnode* ltl)
      : data(ltl)
    {
    }

    // We only support move construction.
    pnode(const pnode& other) = delete;
    pnode& operator=(const pnode& other) = delete;

    pnode(pnode&& other)
      : data(std::move(other.data))
    {
      other.used = true;
    }

    pnode& operator=(pnode&& other)
    {
      data = std::move(other.data);
      other.used = true;
      return *this;
    }

    ~pnode()
    {
      if (used)
        return;
      if (auto* n = std::get_if<nary>(&data))
        {
          for (auto f: n->children)
            f->destroy();
        }
      else
        {
          auto* f = std::get<const spot::fnode*>(data);
          // The only case where we expect f to be nullptr, is if
          // parse_ap() return nullptr: then $$ is unset when YYERROR
          // is called.
          if (f)
            f->destroy();
        }
    }

    // Create a new n-ary node from left and right.
    // This will empty left and right so that their
    // destructor do nothing.
    pnode(spot::op o, pnode&& left, pnode&& right)
      : data(nary{})
    {
      nary& n = std::get<nary>(data);
      n.kind = o;
      if (auto* nleft = std::get_if<nary>(&left.data);
          nleft && nleft->kind == o)
        std::swap(n.children, nleft->children);
      else
        n.children.push_back(left);
      if (auto* nright = std::get_if<nary>(&right.data);
          nright && nright->kind == o)
        {
          auto& rch = nright->children;
          n.children.insert(n.children.end(), rch.begin(), rch.end());
          rch.clear();
        }
      else
        {
          n.children.push_back(right);
        }
    }

    operator const spot::fnode*()
    {
      used = true;
      if (auto* n = std::get_if<nary>(&data))
        {
          return spot::fnode::multop(n->kind, n->children);
        }
      else
        {
          return std::get<const spot::fnode*>(data);
        }
    }

    // Convert to a temporary formula, for printing, do not mark as
    // used.
    const spot::formula tmp() const
    {
      const spot::fnode* f;
      if (auto* n = std::get_if<nary>(&data))
        {
          for (auto c: n->children)
            c->clone();
          f = spot::fnode::multop(n->kind, n->children);
        }
      else
        {
          f = std::get<const spot::fnode*>(data);
          assert(f != nullptr);
          f->clone();
        }
      return spot::formula(f);
    }
  };


}

%parse-param {spot::parse_error_list &error_list}
%parse-param {spot::environment &parse_environment}
%parse-param {spot::formula &result}

%code {
/* parsetl.hh and parsedecl.hh include each other recursively.
   We mut ensure that YYSTYPE is declared (by the above %union)
   before parsedecl.hh uses it. */
#include <spot/parsetl/parsedecl.hh>
using namespace spot;

#define missing_right_op_msg(op, str)		\
  error_list.emplace_back(op,			\
    "missing right operand for \"" str "\"");

#define missing_right_op(res, op, str)		\
  do						\
    {						\
      missing_right_op_msg(op, str);		\
      res = fnode::ff();		\
    }						\
  while (0);

// right is missing, so complain and use left.
#define missing_right_binop(res, left, op, str)	\
  do						\
    {						\
      missing_right_op_msg(op, str);		\
      res = left;				\
    }						\
  while (0);

  static const fnode*
  sere_ensure_bool(const fnode* f, const spot::location& loc,
                   const char* oper, spot::parse_error_list& error_list)
  {
    if (f->is_boolean())
      return f;
    f->destroy();
    std::string s;
    s.reserve(80);
    s = "not a Boolean expression: in a SERE ";
    s += oper;
    s += " can only be applied to a Boolean expression";
    error_list.emplace_back(loc, s);
    return nullptr;
  }

  static const fnode*
  error_false_block(const spot::location& loc,
                    spot::parse_error_list& error_list)
  {
    error_list.emplace_back(loc, "treating this block as false");
    return fnode::ff();
  }

  static const fnode*
  parse_ap(const std::string& str,
           const spot::location& location,
           spot::environment& env,
           spot::parse_error_list& error_list)
  {
    auto res = env.require(str);
    if (!res)
      {
        std::string s;
        s.reserve(64);
        s = "unknown atomic proposition `";
        s += str;
        s += "' in ";
        s += env.name();
        error_list.emplace_back(location, s);
      }
    return res.to_node_();
  }

  enum parser_type { parser_ltl, parser_bool, parser_sere };

  static const fnode*
  try_recursive_parse(const std::string& str,
		      const spot::location& location,
		      spot::environment& env,
		      bool debug,
		      parser_type type,
		      spot::parse_error_list& error_list)
    {
      // We want to parse a U (b U c) as two until operators applied
      // to the atomic propositions a, b, and c.  We also want to
      // parse a U (b == c) as one until operator applied to the
      // atomic propositions "a" and "b == c".  The only problem is
      // that we do not know anything about "==" or in general about
      // the syntax of atomic proposition of our users.
      //
      // To support that, the lexer will return "b U c" and "b == c"
      // as PAR_BLOCK tokens.  We then try to parse such tokens
      // recursively.  If, as in the case of "b U c", the block is
      // successfully parsed as a formula, we return this formula.
      // Otherwise, we convert the string into an atomic proposition
      // (it's up to the environment to check the syntax of this
      // proposition, and maybe reject it).

      if (str.empty())
	{
	  error_list.emplace_back(location, "unexpected empty block");
	  return fnode::ff();
	}

      spot::parsed_formula pf;
      switch (type)
	{
	case parser_sere:
	  pf = spot::parse_infix_sere(str, env, debug, true);
	  break;
	case parser_bool:
	  pf = spot::parse_infix_boolean(str, env, debug, true);
	  break;
	case parser_ltl:
	  pf = spot::parse_infix_psl(str, env, debug, true);
	  break;
	}

      if (pf.errors.empty())
	return pf.f.to_node_();
      return parse_ap(str, location, env, error_list);
    }

}


/* All tokens.  */

%token START_LTL "LTL start marker"
%token START_LBT "LBT start marker"
%token START_SERE "SERE start marker"
%token START_BOOL "BOOLEAN start marker"
%token PAR_OPEN "opening parenthesis" PAR_CLOSE "closing parenthesis"
%token <std::string> PAR_BLOCK "(...) block"
%token <std::string> BRA_BLOCK "{...} block"
%token <std::string> BRA_BANG_BLOCK "{...}! block"
%token BRACE_OPEN "opening brace" BRACE_CLOSE "closing brace"
%token BRACE_BANG_CLOSE "closing brace-bang"
%token OP_OR "or operator" OP_XOR "xor operator"
%token OP_AND "and operator" OP_SHORT_AND "short and operator"
%token OP_IMPLIES "implication operator" OP_EQUIV "equivalent operator"
%token OP_U "until operator" OP_R "release operator"
%token OP_W "weak until operator" OP_M "strong release operator"
%token OP_F "sometimes operator" OP_G "always operator"
%token OP_X "next operator" OP_STRONG_X "strong next operator"
%token OP_NOT "not operator"
%token OP_XREP "X[.] operator"
%token OP_FREP "F[.] operator" OP_GREP "G[.] operator"
%token OP_STAR "star operator" OP_BSTAR "bracket star operator"
%token OP_BFSTAR "bracket fusion-star operator"
%token OP_PLUS "plus operator"
%token OP_FPLUS "fusion-plus operator"
%token OP_STAR_OPEN "opening bracket for star operator"
%token OP_FSTAR_OPEN "opening bracket for fusion-star operator"
%token OP_EQUAL_OPEN "opening bracket for equal operator"
%token OP_GOTO_OPEN "opening bracket for goto operator"
%token OP_SQBKT_CLOSE "closing bracket"
%token OP_SQBKT_STRONG_CLOSE "closing !]"
%token <unsigned> OP_SQBKT_NUM "number for square bracket operator"
%token OP_UNBOUNDED "unbounded mark"
%token OP_SQBKT_SEP "separator for square bracket operator"
%token OP_UCONCAT "universal concat operator"
%token OP_ECONCAT "existential concat operator"
%token OP_UCONCAT_NONO "universal non-overlapping concat operator"
%token OP_ECONCAT_NONO "existential non-overlapping concat operator"
%token OP_FIRST_MATCH "first_match"
%token <std::string> ATOMIC_PROP "atomic proposition"
%token OP_CONCAT "concat operator" OP_FUSION "fusion operator"
%token CONST_TRUE "constant true" CONST_FALSE "constant false"
%token END_OF_INPUT "end of formula"
%token OP_POST_NEG "negative suffix" OP_POST_POS "positive suffix"
%token <unsigned> OP_DELAY_N "SVA delay operator"
%token OP_DELAY_OPEN "opening bracket for SVA delay operator"
%token OP_DELAY_PLUS "##[+] operator"
%token OP_DELAY_STAR "##[*] operator"

/* Priorities.  */

/* Low priority SERE-LTL binding operator. */
%precedence OP_UCONCAT OP_ECONCAT OP_UCONCAT_NONO OP_ECONCAT_NONO

%left OP_CONCAT
%left OP_FUSION
%left OP_DELAY_N OP_DELAY_OPEN OP_DELAY_PLUS OP_DELAY_STAR

/* Logical operators.  */
%right OP_IMPLIES OP_EQUIV
%left OP_OR
%left OP_XOR
%left OP_AND OP_SHORT_AND


 /* OP_STAR can be used as an AND when occurring in some LTL formula in
   Wring's syntax (so it has to be close to OP_AND), and as a Kleen
   Star in SERE (so it has to be close to OP_BSTAR -- luckily
   U/R/M/W/F/G/X are not used in SERE). */
%left OP_STAR

/* LTL operators.  */
%right OP_U OP_R OP_M OP_W
%precedence OP_F OP_G OP_FREP OP_GREP
%precedence OP_X OP_XREP OP_STRONG_X

/* High priority regex operator. */
%precedence OP_BSTAR OP_STAR_OPEN OP_PLUS
          OP_BFSTAR OP_FSTAR_OPEN OP_FPLUS
          OP_EQUAL_OPEN OP_GOTO_OPEN

/* Not has the most important priority (after Wring's `=0' and `=1',
   but as those can only attach to atomic proposition, they do not
   need any precedence).  */
%precedence OP_NOT

%type <pnode> subformula atomprop booleanatom sere lbtformula
%type <pnode> boolformula bracedsere parenthesedsubformula
%type <minmax_t> starargs fstarargs equalargs sqbracketargs gotoargs delayargs
%type <unsigned> sqbkt_num

%printer { debug_stream() << $$; } <std::string>
%printer { print_psl(debug_stream(), $$.tmp()); } <pnode>
%printer { print_sere(debug_stream(), $$.tmp()); } sere bracedsere
%printer { debug_stream() << $$; } <unsigned>
%printer { debug_stream() << $$.min << ".." << $$.max; } <minmax_t>

%%
result:       START_LTL subformula END_OF_INPUT
              {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_LTL enderror
	      {
		result = nullptr;
		YYABORT;
	      }
	    | START_LTL subformula enderror
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_LTL emptyinput
              { YYABORT; }
            | START_BOOL boolformula END_OF_INPUT
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_BOOL enderror
	      {
		result = nullptr;
		YYABORT;
	      }
	    | START_BOOL boolformula enderror
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_BOOL emptyinput
              { YYABORT; }
            | START_SERE sere END_OF_INPUT
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_SERE enderror
	      {
		result = nullptr;
		YYABORT;
	      }
	    | START_SERE sere enderror
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_SERE emptyinput
              { YYABORT; }
            | START_LBT lbtformula END_OF_INPUT
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_LBT enderror
	      {
		result = nullptr;
		YYABORT;
	      }
	    | START_LBT lbtformula enderror
	      {
		result = formula($2);
		YYACCEPT;
	      }
	    | START_LBT emptyinput
              { YYABORT; }

emptyinput: END_OF_INPUT
              {
		error_list.emplace_back(@$, "empty input");
		result = nullptr;
	      }

enderror: error END_OF_INPUT
              {
		error_list.emplace_back(@1, "ignoring trailing garbage");
	      }


OP_SQBKT_SEP_unbounded: OP_SQBKT_SEP | OP_SQBKT_SEP OP_UNBOUNDED
OP_SQBKT_SEP_opt: %empty
                | OP_SQBKT_SEP_unbounded
error_opt: %empty
         | error

sqbkt_num: OP_SQBKT_NUM
         {
           auto n = $1;
           if (n >= fnode::unbounded())
             {
               auto max = fnode::unbounded() - 1;
               std::ostringstream s;
               s << n << " exceeds maximum supported repetition ("
                 << max << ")";
               error_list.emplace_back(@1, s.str());
               $$ = max;
             }
           else
             {
               $$ = n;
             }
         }

/* for [*i..j] and [=i..j] */
sqbracketargs: sqbkt_num OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
              { $$.min = $1; $$.max = $3; }
	     | sqbkt_num OP_SQBKT_SEP_unbounded OP_SQBKT_CLOSE
              { $$.min = $1; $$.max = fnode::unbounded(); }
	     | OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
              { $$.min = 0U; $$.max = $2; }
	     | OP_SQBKT_SEP_opt OP_SQBKT_CLOSE
              { $$.min = 0U; $$.max = fnode::unbounded(); }
	     | sqbkt_num OP_SQBKT_CLOSE
              { $$.min = $$.max = $1; }

/* [->i..j] has default values that are different than [*] and [=]. */
gotoargs: OP_GOTO_OPEN sqbkt_num OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
           { $$.min = $2; $$.max = $4; }
	  | OP_GOTO_OPEN sqbkt_num OP_SQBKT_SEP_unbounded OP_SQBKT_CLOSE
           { $$.min = $2; $$.max = fnode::unbounded(); }
	  | OP_GOTO_OPEN OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
           { $$.min = 1U; $$.max = $3; }
	  | OP_GOTO_OPEN OP_SQBKT_SEP_unbounded OP_SQBKT_CLOSE
           { $$.min = 1U; $$.max = fnode::unbounded(); }
	  | OP_GOTO_OPEN OP_SQBKT_CLOSE
           { $$.min = $$.max = 1U; }
	  | OP_GOTO_OPEN sqbkt_num OP_SQBKT_CLOSE
           { $$.min = $$.max = $2; }
	  | OP_GOTO_OPEN error OP_SQBKT_CLOSE
           { error_list.emplace_back(@$, "treating this goto block as [->]");
             $$.min = $$.max = 1U; }
          | OP_GOTO_OPEN error_opt END_OF_INPUT
	   { error_list.
	       emplace_back(@$, "missing closing bracket for goto operator");
	     $$.min = $$.max = 0U; }

kleen_star: OP_STAR | OP_BSTAR

starargs: kleen_star
            { $$.min = 0U; $$.max = fnode::unbounded(); }
        | OP_PLUS
	    { $$.min = 1U; $$.max = fnode::unbounded(); }
	| OP_STAR_OPEN sqbracketargs
	    { $$ = $2; }
	| OP_STAR_OPEN error OP_SQBKT_CLOSE
            { error_list.emplace_back(@$, "treating this star block as [*]");
              $$.min = 0U; $$.max = fnode::unbounded(); }
        | OP_STAR_OPEN error_opt END_OF_INPUT
	    { error_list.emplace_back(@$, "missing closing bracket for star");
	      $$.min = $$.max = 0U; }

fstarargs: OP_BFSTAR
            { $$.min = 0U; $$.max = fnode::unbounded(); }
        | OP_FPLUS
	    { $$.min = 1U; $$.max = fnode::unbounded(); }
	| OP_FSTAR_OPEN sqbracketargs
	    { $$ = $2; }
	| OP_FSTAR_OPEN error OP_SQBKT_CLOSE
            { error_list.emplace_back
		(@$, "treating this fusion-star block as [:*]");
              $$.min = 0U; $$.max = fnode::unbounded(); }
        | OP_FSTAR_OPEN error_opt END_OF_INPUT
	    { error_list.emplace_back
		(@$, "missing closing bracket for fusion-star");
	      $$.min = $$.max = 0U; }

equalargs: OP_EQUAL_OPEN sqbracketargs
	    { $$ = $2; }
	| OP_EQUAL_OPEN error OP_SQBKT_CLOSE
            { error_list.emplace_back(@$, "treating this equal block as [=]");
              $$.min = 0U; $$.max = fnode::unbounded(); }
        | OP_EQUAL_OPEN error_opt END_OF_INPUT
	    { error_list.
		emplace_back(@$, "missing closing bracket for equal operator");
	      $$.min = $$.max = 0U; }

delayargs: OP_DELAY_OPEN sqbracketargs
	    { $$ = $2; }
	| OP_DELAY_OPEN error OP_SQBKT_CLOSE
            { error_list.emplace_back(@$, "treating this delay block as ##1");
              $$.min = $$.max = 1U; }
        | OP_DELAY_OPEN error_opt END_OF_INPUT
	    { error_list.
		emplace_back(@$, "missing closing bracket for ##[");
	      $$.min = $$.max = 1U; }
        | OP_DELAY_PLUS
          { $$.min = 1; $$.max = fnode::unbounded(); }
        | OP_DELAY_STAR
          { $$.min = 0; $$.max = fnode::unbounded(); }

atomprop: ATOMIC_PROP
          {
            auto* f = parse_ap($1, @1, parse_environment, error_list);
            if (!f)
              YYERROR;
            $$ = f;
          }

booleanatom: atomprop
	    | atomprop OP_POST_POS
	    | atomprop OP_POST_NEG
	      {
		$$ = fnode::unop(op::Not, $1);
	      }
	    | CONST_TRUE
	      { $$ = fnode::tt(); }
	    | CONST_FALSE
	      { $$ = fnode::ff(); }

sere: booleanatom
            | OP_NOT sere
	      {
		if (auto f = sere_ensure_bool($2, @2, "`!'", error_list))
		  {
		    $$ = fnode::unop(op::Not, f);
		  }
		else
		  {
		    $$ = error_false_block(@$, error_list);
		  }
	      }
            | bracedsere
	    | PAR_BLOCK
              {
		$$ =
		  try_recursive_parse($1, @1, parse_environment,
				      debug_level(), parser_sere, error_list);
		if (!$$)
		  YYERROR;
	      }
	    | PAR_OPEN sere PAR_CLOSE
	      { $$ = $2; }
	    | PAR_OPEN error PAR_CLOSE
	      { error_list.
		  emplace_back(@$,
			       "treating this parenthetical block as false");
		$$ = fnode::ff();
	      }
	    | PAR_OPEN sere END_OF_INPUT
	      { error_list.emplace_back(@1 + @2, "missing closing parenthesis");
		$$ = $2;
	      }
	    | PAR_OPEN error END_OF_INPUT
	      { error_list.emplace_back(@$,
                    "missing closing parenthesis, "
		    "treating this parenthetical block as false");
		$$ = fnode::ff();
	      }
	    | sere OP_AND sere
	      { $$ = pnode(op::AndRat, $1, $3); }
	    | sere OP_AND error
	      { missing_right_binop($$, $1, @2,
				    "length-matching and operator"); }
	    | sere OP_SHORT_AND sere
              { $$ = pnode(op::AndNLM, $1, $3); }
	    | sere OP_SHORT_AND error
              { missing_right_binop($$, $1, @2,
                                    "non-length-matching and operator"); }
	    | sere OP_OR sere
            { $$ = pnode(op::OrRat, $1, $3); }
	    | sere OP_OR error
              { missing_right_binop($$, $1, @2, "or operator"); }
	    | sere OP_CONCAT sere
	      { $$ = pnode(op::Concat, $1, $3); }
	    | sere OP_CONCAT error
              { missing_right_binop($$, $1, @2, "concat operator"); }
	    | sere OP_FUSION sere
	      { $$ = pnode(op::Fusion, $1, $3); }
	    | sere OP_FUSION error
              { missing_right_binop($$, $1, @2, "fusion operator"); }
	    | OP_DELAY_N sere
              { unsigned n = $1; $$ = formula::sugar_delay(formula($2), n, n).to_node_(); }
	    | OP_DELAY_N error
              { missing_right_binop($$, fnode::tt(), @1, "SVA delay operator"); }
	    | sere OP_DELAY_N sere
            { unsigned n = $2;
              $$ = formula::sugar_delay(formula($1), formula($3),
                                        n, n).to_node_(); }
	    | sere OP_DELAY_N error
              { missing_right_binop($$, $1, @2, "SVA delay operator"); }
	    | delayargs sere %prec OP_DELAY_OPEN
              {
                auto [min, max] = $1;
		if (max < min)
		  {
		    error_list.emplace_back(@1, "reversed range");
		    std::swap(max, min);
		  }
                $$ = formula::sugar_delay(formula($2),
                                          min, max).to_node_();
              }
	    | delayargs error
              { missing_right_binop($$, fnode::tt(), @1, "SVA delay operator"); }
	    | sere delayargs sere %prec OP_DELAY_OPEN
              {
                auto [min, max] = $2;
		if (max < min)
		  {
		    error_list.emplace_back(@1, "reversed range");
		    std::swap(max, min);
		  }
                $$ = formula::sugar_delay(formula($1), formula($3),
                                          min, max).to_node_();
              }
	    | sere delayargs error
              { missing_right_binop($$, $1, @2, "SVA delay operator"); }
	    | starargs
	      {
                auto [min, max] = $1;
		if (max < min)
		  {
		    error_list.emplace_back(@1, "reversed range");
		    std::swap(max, min);
		  }
		$$ = fnode::bunop(op::Star, fnode::tt(), min, max);
	      }
	    | sere starargs
	      {
                auto [min, max] = $2;
		if (max < min)
		  {
		    error_list.emplace_back(@2, "reversed range");
		    std::swap(max, min);
		  }
		$$ = fnode::bunop(op::Star, $1, min, max);
	      }
	    | sere fstarargs
	      {
                auto [min, max] = $2;
		if (max < min)
		  {
		    error_list.emplace_back(@2, "reversed range");
		    std::swap(max, min);
		  }
		$$ = fnode::bunop(op::FStar, $1, min, max);
	      }
	    | sere equalargs
	      {
                auto [min, max] = $2;
		if (max < min)
		  {
		    error_list.emplace_back(@2, "reversed range");
		    std::swap(max, min);
		  }
		if (auto f = sere_ensure_bool($1, @1, "[=...]", error_list))
		  {
		    $$ = formula::sugar_equal(formula(f),
					      min, max).to_node_();
		  }
		else
		  {
		    $$ = error_false_block(@$, error_list);
		  }
	      }
	    | sere gotoargs
	      {
                auto [min, max] = $2;
		if (max < min)
		  {
		    error_list.emplace_back(@2, "reversed range");
		    std::swap(max, min);
		  }
		if (auto f = sere_ensure_bool($1, @1, "[->...]", error_list))
		  {
		    $$ = formula::sugar_goto(formula(f), min, max).to_node_();
		  }
		else
		  {
		    $$ = error_false_block(@$, error_list);
		  }
	      }
	    | sere OP_XOR sere
	      {
                auto left = sere_ensure_bool($1, @1, "`^'", error_list);
                auto right = sere_ensure_bool($3, @3, "`^'", error_list);
		if (left && right)
		  {
		    $$ = fnode::binop(op::Xor, left, right);
		  }
		else
		  {
                    if (left)
                      left->destroy();
                    else if (right)
                      right->destroy();
		    $$ = error_false_block(@$, error_list);
		  }
	      }
	    | sere OP_XOR error
	      { missing_right_binop($$, $1, @2, "xor operator"); }
	    | sere OP_IMPLIES sere
	      {
                auto left = sere_ensure_bool($1, @1, "`->'", error_list);
		if (left)
		  {
		    $$ = fnode::binop(op::Implies, left, $3);
		  }
		else
		  {
		    $$ = error_false_block(@$, error_list);
		  }
	      }
	    | sere OP_IMPLIES error
	      { missing_right_binop($$, $1, @2, "implication operator"); }
	    | sere OP_EQUIV sere
	      {
                auto left = sere_ensure_bool($1, @1, "`<->'", error_list);
                auto right = sere_ensure_bool($3, @3, "`<->'", error_list);
                if (left && right)
		  {
		    $$ = fnode::binop(op::Equiv, left, right);
		  }
		else
		  {
                    if (left)
                      left->destroy();
                    else if (right)
                      right->destroy();
		    $$ = error_false_block(@$, error_list);
		  }
	      }
	    | sere OP_EQUIV error
	      { missing_right_binop($$, $1, @2, "equivalent operator"); }
            | OP_FIRST_MATCH PAR_OPEN sere PAR_CLOSE
              { $$ = fnode::unop(op::first_match, $3); }

bracedsere: BRACE_OPEN sere BRACE_CLOSE
              { $$ = $2; }
            | BRACE_OPEN sere error BRACE_CLOSE
	      { error_list.emplace_back(@3, "ignoring this");
		$$ = $2;
	      }
            | BRACE_OPEN error BRACE_CLOSE
	      { error_list.emplace_back(@$,
					"treating this brace block as false");
		$$ = fnode::ff();
	      }
            | BRACE_OPEN sere END_OF_INPUT
	      { error_list.emplace_back(@1 + @2,
					"missing closing brace");
		$$ = $2;
	      }
	    | BRACE_OPEN sere error END_OF_INPUT
	      { error_list. emplace_back(@3,
                  "ignoring trailing garbage and missing closing brace");
		$$ = $2;
	      }
	    | BRACE_OPEN error END_OF_INPUT
	      { error_list.emplace_back(@$,
                    "missing closing brace, "
		    "treating this brace block as false");
		$$ = fnode::ff();
	      }
            | BRA_BLOCK
              {
		$$ = try_recursive_parse($1, @1, parse_environment,
					 debug_level(),
                                         parser_sere, error_list);
		if (!$$)
		  YYERROR;
	      }

parenthesedsubformula: PAR_BLOCK
              {
		$$ = try_recursive_parse($1, @1, parse_environment,
					 debug_level(), parser_ltl, error_list);
		if (!$$)
		  YYERROR;
	      }
            | PAR_OPEN subformula PAR_CLOSE
	      { $$ = $2; }
	    | PAR_OPEN subformula error PAR_CLOSE
	      { error_list.emplace_back(@3, "ignoring this");
		$$ = $2;
	      }
	    | PAR_OPEN error PAR_CLOSE
	      { error_list.emplace_back(@$,
		 "treating this parenthetical block as false");
		$$ = fnode::ff();
	      }
	    | PAR_OPEN subformula END_OF_INPUT
	      { error_list.emplace_back(@1 + @2, "missing closing parenthesis");
		$$ = $2;
	      }
	    | PAR_OPEN subformula error END_OF_INPUT
	      { error_list.emplace_back(@3,
                "ignoring trailing garbage and missing closing parenthesis");
		$$ = $2;
	      }
	    | PAR_OPEN error END_OF_INPUT
	      { error_list.emplace_back(@$,
                    "missing closing parenthesis, "
		    "treating this parenthetical block as false");
		$$ = fnode::ff();
	      }


boolformula: booleanatom
            | PAR_BLOCK
              {
		$$ = try_recursive_parse($1, @1, parse_environment,
					 debug_level(),
                                         parser_bool, error_list);
		if (!$$)
		  YYERROR;
	      }
            | PAR_OPEN boolformula PAR_CLOSE
	      { $$ = $2; }
	    | PAR_OPEN boolformula error PAR_CLOSE
	      { error_list.emplace_back(@3, "ignoring this");
		$$ = $2;
	      }
	    | PAR_OPEN error PAR_CLOSE
	      { error_list.emplace_back(@$,
		 "treating this parenthetical block as false");
		$$ = fnode::ff();
	      }
	    | PAR_OPEN boolformula END_OF_INPUT
	      { error_list.emplace_back(@1 + @2,
					"missing closing parenthesis");
		$$ = $2;
	      }
	    | PAR_OPEN boolformula error END_OF_INPUT
	      { error_list.emplace_back(@3,
                "ignoring trailing garbage and missing closing parenthesis");
		$$ = $2;
	      }
	    | PAR_OPEN error END_OF_INPUT
	      { error_list.emplace_back(@$,
                    "missing closing parenthesis, "
		    "treating this parenthetical block as false");
		$$ = fnode::ff();
	      }
	    | boolformula OP_AND boolformula
	      { $$ = pnode(op::And, $1, $3); }
	    | boolformula OP_AND error
              { missing_right_binop($$, $1, @2, "and operator"); }
	    | boolformula OP_SHORT_AND boolformula
              { $$ = pnode(op::And, $1, $3); }
	    | boolformula OP_SHORT_AND error
              { missing_right_binop($$, $1, @2, "and operator"); }
	    | boolformula OP_STAR boolformula
	      { $$ = pnode(op::And, $1, $3); }
	    | boolformula OP_STAR error
              { missing_right_binop($$, $1, @2, "and operator"); }
	    | boolformula OP_OR boolformula
	      { $$ = pnode(op::Or, $1, $3); }
	    | boolformula OP_OR error
              { missing_right_binop($$, $1, @2, "or operator"); }
	    | boolformula OP_XOR boolformula
	      { $$ = fnode::binop(op::Xor, $1, $3); }
	    | boolformula OP_XOR error
	      { missing_right_binop($$, $1, @2, "xor operator"); }
	    | boolformula OP_IMPLIES boolformula
	      { $$ = fnode::binop(op::Implies, $1, $3); }
	    | boolformula OP_IMPLIES error
	      { missing_right_binop($$, $1, @2, "implication operator"); }
	    | boolformula OP_EQUIV boolformula
	      { $$ = fnode::binop(op::Equiv, $1, $3); }
	    | boolformula OP_EQUIV error
	      { missing_right_binop($$, $1, @2, "equivalent operator"); }
	    | OP_NOT boolformula
	      { $$ = fnode::unop(op::Not, $2); }
	    | OP_NOT error
	      { missing_right_op($$, @1, "not operator"); }

subformula: booleanatom
            | parenthesedsubformula
	    | subformula OP_AND subformula
              { $$ = pnode(op::And, $1, $3); }
	    | subformula OP_AND error
              { missing_right_binop($$, $1, @2, "and operator"); }
	    | subformula OP_SHORT_AND subformula
              { $$ = pnode(op::And, $1, $3); }
	    | subformula OP_SHORT_AND error
              { missing_right_binop($$, $1, @2, "and operator"); }
	    | subformula OP_STAR subformula
	      { $$ = pnode(op::And, $1, $3); }
	    | subformula OP_STAR error
              { missing_right_binop($$, $1, @2, "and operator"); }
	    | subformula OP_OR subformula
	      { $$ = pnode(op::Or, $1, $3); }
	    | subformula OP_OR error
              { missing_right_binop($$, $1, @2, "or operator"); }
	    | subformula OP_XOR subformula
 	      { $$ = fnode::binop(op::Xor, $1, $3); }
	    | subformula OP_XOR error
	      { missing_right_binop($$, $1, @2, "xor operator"); }
	    | subformula OP_IMPLIES subformula
	      { $$ = fnode::binop(op::Implies, $1, $3); }
	    | subformula OP_IMPLIES error
	      { missing_right_binop($$, $1, @2, "implication operator"); }
	    | subformula OP_EQUIV subformula
	      { $$ = fnode::binop(op::Equiv, $1, $3); }
	    | subformula OP_EQUIV error
	      { missing_right_binop($$, $1, @2, "equivalent operator"); }
	    | subformula OP_U subformula
	      { $$ = fnode::binop(op::U, $1, $3); }
	    | subformula OP_U error
	      { missing_right_binop($$, $1, @2, "until operator"); }
	    | subformula OP_R subformula
	      { $$ = fnode::binop(op::R, $1, $3); }
	    | subformula OP_R error
	      { missing_right_binop($$, $1, @2, "release operator"); }
	    | subformula OP_W subformula
	      { $$ = fnode::binop(op::W, $1, $3); }
	    | subformula OP_W error
	      { missing_right_binop($$, $1, @2, "weak until operator"); }
	    | subformula OP_M subformula
	      { $$ = fnode::binop(op::M, $1, $3); }
	    | subformula OP_M error
	      { missing_right_binop($$, $1, @2, "strong release operator"); }
	    | OP_F subformula
	      { $$ = fnode::unop(op::F, $2); }
	    | OP_F error
	      { missing_right_op($$, @1, "sometimes operator"); }
	    | OP_FREP sqbkt_num OP_SQBKT_CLOSE subformula %prec OP_FREP
              { unsigned n = $2;
                $$ = fnode::nested_unop_range(op::X, op::Or, n, n, $4);
                error_list.emplace_back(@1 + @3,
                                        "F[n:m] expects two parameters");
              }
	    | OP_FREP sqbkt_num OP_SQBKT_STRONG_CLOSE subformula
              %prec OP_FREP
              { unsigned n = $2;
                $$ = fnode::nested_unop_range(op::strong_X, op::Or, n, n, $4);
                error_list.emplace_back(@1 + @3,
                                        "F[n:m!] expects two parameters");
              }
	    | OP_FREP sqbkt_num OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
              subformula %prec OP_FREP
              { $$ = fnode::nested_unop_range(op::X, op::Or, $2, $4, $6); }
	    | OP_FREP sqbkt_num OP_SQBKT_SEP sqbkt_num
              OP_SQBKT_STRONG_CLOSE subformula %prec OP_FREP
              { $$ = fnode::nested_unop_range(op::strong_X,
                                              op::Or, $2, $4, $6); }
	    | OP_FREP sqbkt_num OP_SQBKT_SEP_unbounded OP_SQBKT_CLOSE
              subformula %prec OP_FREP
            { $$ = fnode::nested_unop_range(op::X, op::Or, $2,
                                            fnode::unbounded(), $5); }
	    | OP_FREP sqbkt_num OP_SQBKT_SEP_unbounded OP_SQBKT_STRONG_CLOSE
              subformula %prec OP_FREP
            { $$ = fnode::nested_unop_range(op::strong_X, op::Or, $2,
                                            fnode::unbounded(), $5); }
	    | OP_FREP sqbkt_num OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
              error
	      { missing_right_op($$, @1 + @5, "F[.] operator"); }
	    | OP_FREP sqbkt_num OP_SQBKT_SEP sqbkt_num
              OP_SQBKT_STRONG_CLOSE error
	      { missing_right_op($$, @1 + @5, "F[.!] operator"); }
            | OP_FREP error_opt END_OF_INPUT
              { error_list.emplace_back(@$, "missing closing bracket for F[.]");
                $$ = fnode::ff(); }
            | OP_FREP error OP_SQBKT_CLOSE subformula %prec OP_FREP
              { error_list.emplace_back(@1 + @3,
                                        "treating this F[.] as a simple F");
                $$ = fnode::unop(op::F, $4); }
            | OP_FREP error OP_SQBKT_STRONG_CLOSE subformula %prec OP_FREP
              { error_list.emplace_back(@1 + @3,
                                        "treating this F[.!] as a simple F");
                $$ = fnode::unop(op::F, $4); }
	    | OP_G subformula
	      { $$ = fnode::unop(op::G, $2); }
	    | OP_G error
	      { missing_right_op($$, @1, "always operator"); }
	    | OP_GREP sqbkt_num OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
              subformula %prec OP_GREP
              { $$ = fnode::nested_unop_range(op::X, op::And, $2, $4, $6); }
	    | OP_GREP sqbkt_num OP_SQBKT_SEP sqbkt_num
              OP_SQBKT_STRONG_CLOSE subformula %prec OP_GREP
              { $$ = fnode::nested_unop_range(op::strong_X, op::And,
                                              $2, $4, $6); }
	    | OP_GREP sqbkt_num OP_SQBKT_SEP_unbounded OP_SQBKT_CLOSE
              subformula %prec OP_GREP
            { $$ = fnode::nested_unop_range(op::X, op::And, $2,
                                            fnode::unbounded(), $5); }
	    | OP_GREP sqbkt_num OP_SQBKT_SEP_unbounded OP_SQBKT_STRONG_CLOSE
              subformula %prec OP_GREP
            { $$ = fnode::nested_unop_range(op::strong_X, op::And, $2,
                                            fnode::unbounded(), $5); }
	    | OP_GREP sqbkt_num OP_SQBKT_CLOSE subformula %prec OP_GREP
              { unsigned n = $2;
                $$ = fnode::nested_unop_range(op::X, op::And, n, n, $4);
                error_list.emplace_back(@1 + @3,
                                        "G[n:m] expects two parameters");
              }
	    | OP_GREP sqbkt_num OP_SQBKT_STRONG_CLOSE subformula
              %prec OP_GREP
              { unsigned n = $2;
                $$ = fnode::nested_unop_range(op::strong_X, op::And,
                                              n, n, $4);
                error_list.emplace_back(@1 + @3,
                                        "G[n:m!] expects two parameters");
              }
	    | OP_GREP sqbkt_num OP_SQBKT_SEP sqbkt_num OP_SQBKT_CLOSE
              error
	      { missing_right_op($$, @1 + @5, "G[.] operator"); }
	    | OP_GREP sqbkt_num OP_SQBKT_SEP sqbkt_num
              OP_SQBKT_STRONG_CLOSE error
	      { missing_right_op($$, @1 + @5, "G[.!] operator"); }
            | OP_GREP error_opt END_OF_INPUT
              { error_list.emplace_back(@$, "missing closing bracket for G[.]");
                $$ = fnode::ff(); }
            | OP_GREP error OP_SQBKT_CLOSE subformula %prec OP_GREP
              { error_list.emplace_back(@1 + @3,
                                        "treating this G[.] as a simple G");
                $$ = fnode::unop(op::F, $4); }
            | OP_GREP error OP_SQBKT_STRONG_CLOSE subformula %prec OP_GREP
              { error_list.emplace_back(@1 + @3,
                                        "treating this G[.!] as a simple G");
                $$ = fnode::unop(op::F, $4); }
	    | OP_X subformula
	      { $$ = fnode::unop(op::X, $2); }
	    | OP_X error
	      { missing_right_op($$, @1, "next operator"); }
	    | OP_STRONG_X subformula
	      { $$ = fnode::unop(op::strong_X, $2); }
	    | OP_STRONG_X error
	      { missing_right_op($$, @1, "strong next operator"); }
	    | OP_XREP sqbkt_num OP_SQBKT_CLOSE subformula %prec OP_XREP
              { unsigned n = $2;
                $$ = fnode::nested_unop_range(op::X, op::Or, n, n, $4); }
	    | OP_XREP sqbkt_num OP_SQBKT_CLOSE error
	      { missing_right_op($$, @1 + @3, "X[.] operator"); }
            | OP_XREP error OP_SQBKT_CLOSE subformula %prec OP_XREP
              { error_list.emplace_back(@$, "treating this X[.] as a simple X");
                $$ = fnode::unop(op::X, $4); }
	    | OP_XREP OP_SQBKT_STRONG_CLOSE subformula %prec OP_XREP
              { $$ = fnode::unop(op::strong_X, $3); }
	    | OP_XREP sqbkt_num OP_SQBKT_STRONG_CLOSE subformula
              %prec OP_XREP
              { unsigned n = $2;
                $$ = fnode::nested_unop_range(op::strong_X,
                                              op::Or, n, n, $4); }
            | OP_XREP error OP_SQBKT_STRONG_CLOSE subformula %prec OP_XREP
              { error_list.emplace_back(@$, "treating this X[.!] as a simple X[!]");
                $$ = fnode::unop(op::strong_X, $4); }
            | OP_XREP error_opt END_OF_INPUT
              { error_list.emplace_back(@$, "missing closing bracket for X[.]");
                $$ = fnode::ff(); }
	    | OP_NOT subformula
	      { $$ = fnode::unop(op::Not, $2); }
	    | OP_NOT error
	      { missing_right_op($$, @1, "not operator"); }
            | bracedsere
	      { $$ = fnode::unop(op::Closure, $1); }
            | bracedsere OP_UCONCAT subformula
	      { $$ = fnode::binop(op::UConcat, $1, $3); }
            | bracedsere parenthesedsubformula
	      { $$ = fnode::binop(op::UConcat, $1, $2); }
            | bracedsere OP_UCONCAT error
              { missing_right_op($$, @2,
                                 "universal overlapping concat operator"); }
            | bracedsere OP_ECONCAT subformula
	      { $$ = fnode::binop(op::EConcat, $1, $3); }
            | bracedsere OP_ECONCAT error
	      { missing_right_op($$, @2,
                                 "existential overlapping concat operator");
	      }
            | bracedsere OP_UCONCAT_NONO subformula
	      /* {SERE}[]=>EXP = {SERE;1}[]->EXP */
	      { $$ = fnode::binop(op::UConcat,
				  pnode(op::Concat, $1, fnode::tt()),
				  $3); }
            | bracedsere OP_UCONCAT_NONO error
	      { missing_right_op($$, @2,
                                 "universal non-overlapping concat operator");
	      }
            | bracedsere OP_ECONCAT_NONO subformula
	      /* {SERE}<>=>EXP = {SERE;1}<>->EXP */
	      { $$ = fnode::binop(op::EConcat,
				  pnode(op::Concat, $1, fnode::tt()),
				  $3); }
            | bracedsere OP_ECONCAT_NONO error
	      { missing_right_op($$, @2,
                                 "existential non-overlapping concat operator");
	      }
            | BRACE_OPEN sere BRACE_BANG_CLOSE
	      /* {SERE}! = {SERE} <>-> 1 */
	      { $$ = fnode::binop(op::EConcat, $2, fnode::tt()); }
            | BRA_BANG_BLOCK
              {
		$$ = try_recursive_parse($1, @1, parse_environment,
					 debug_level(),
                                         parser_sere, error_list);
		if (!$$)
		  YYERROR;
		$$ = fnode::binop(op::EConcat, $$, fnode::tt());
	      }

lbtformula: atomprop
            | '!' lbtformula
	      { $$ = fnode::unop(op::Not, $2); }
            | '&' lbtformula lbtformula
	      { $$ = pnode(op::And, $2, $3); }
            | '|' lbtformula lbtformula
	      { $$ = pnode(op::Or, $2, $3); }
            | '^' lbtformula lbtformula
	      { $$ = fnode::binop(op::Xor, $2, $3); }
            | 'i' lbtformula lbtformula
	      { $$ = fnode::binop(op::Implies, $2, $3); }
            | 'e' lbtformula lbtformula
	      { $$ = fnode::binop(op::Equiv, $2, $3); }
            | 'X' lbtformula
	      { $$ = fnode::unop(op::X, $2); }
            | 'F' lbtformula
	      { $$ = fnode::unop(op::F, $2); }
            | 'G' lbtformula
	      { $$ = fnode::unop(op::G, $2); }
            | 'U' lbtformula lbtformula
	      { $$ = fnode::binop(op::U, $2, $3); }
            | 'V' lbtformula lbtformula
	      { $$ = fnode::binop(op::R, $2, $3); }
            | 'R' lbtformula lbtformula
	      { $$ = fnode::binop(op::R, $2, $3); }
            | 'W' lbtformula lbtformula
	      { $$ = fnode::binop(op::W, $2, $3); }
            | 'M' lbtformula lbtformula
	      { $$ = fnode::binop(op::M, $2, $3); }
            | 't'
	      { $$ = fnode::tt(); }
            | 'f'
	      { $$ = fnode::ff(); }
            ;

%%

void
tlyy::parser::error(const location_type& location, const std::string& message)
{
  error_list.emplace_back(location, message);
}

namespace spot
{
  parsed_formula
  parse_infix_psl(const std::string& ltl_string,
		  environment& env,
		  bool debug, bool lenient)
  {
    parsed_formula result(ltl_string);
    flex_set_buffer(ltl_string,
		    tlyy::parser::token::START_LTL,
		    lenient);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  parsed_formula
  parse_infix_boolean(const std::string& ltl_string,
		      environment& env,
		      bool debug, bool lenient)
  {
    parsed_formula result(ltl_string);
    flex_set_buffer(ltl_string,
		    tlyy::parser::token::START_BOOL,
		    lenient);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  parsed_formula
  parse_prefix_ltl(const std::string& ltl_string,
		   environment& env,
		   bool debug)
  {
    parsed_formula result(ltl_string);
    flex_set_buffer(ltl_string,
		    tlyy::parser::token::START_LBT,
		    false);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  parsed_formula
  parse_infix_sere(const std::string& sere_string,
		   environment& env,
		   bool debug,
		   bool lenient)
  {
    parsed_formula result(sere_string);
    flex_set_buffer(sere_string,
		    tlyy::parser::token::START_SERE,
		    lenient);
    tlyy::parser parser(result.errors, env, result.f);
    parser.set_debug_level(debug);
    parser.parse();
    flex_unset_buffer();
    return result;
  }

  formula
  parse_formula(const std::string& ltl_string, environment& env)
  {
    parsed_formula pf = parse_infix_psl(ltl_string, env);
    std::ostringstream s;
    if (pf.format_errors(s))
      {
	parsed_formula pg = parse_prefix_ltl(ltl_string, env);
	if (pg.errors.empty())
	  return pg.f;
	else
	  throw parse_error(s.str());
      }
    return pf.f;
  }
}

// Local Variables:
// mode: c++
// End:
