// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.


// Take the name prefix into account.
#define yylex   hoayylex



#include "parseaut.hh"


// Unqualified %code blocks.
#line 233 "parseaut.yy"

#include <sstream>

  /* parseaut.hh and parsedecl.hh include each other recursively.
   We must ensure that YYSTYPE is declared (by the above %union)
   before parsedecl.hh uses it. */
#include <spot/parseaut/parsedecl.hh>

  static void fill_guards(result_& res);

#line 59 "parseaut.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if HOAYYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !HOAYYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !HOAYYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace hoayy {
#line 151 "parseaut.cc"

  /// Build a parser object.
  parser::parser (void* scanner_yyarg, result_& res_yyarg, spot::location initial_loc_yyarg)
#if HOAYYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      res (res_yyarg),
      initial_loc (initial_loc_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}


  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.kind ())
    {
      case symbol_kind::S_IDENTIFIER: // "identifier"
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 365 "parseaut.cc"
        break;

      case symbol_kind::S_HEADERNAME: // "header name"
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 371 "parseaut.cc"
        break;

      case symbol_kind::S_ANAME: // "alias name"
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 377 "parseaut.cc"
        break;

      case symbol_kind::S_STRING: // "string"
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 383 "parseaut.cc"
        break;

      case symbol_kind::S_25_: // '['
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 389 "parseaut.cc"
        break;

      case symbol_kind::S_LINEDIRECTIVE: // "#line"
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 395 "parseaut.cc"
        break;

      case symbol_kind::S_BDD: // BDD
#line 333 "parseaut.yy"
                    { bdd_delref((yysym.value.b)); }
#line 401 "parseaut.cc"
        break;

      case symbol_kind::S_FORMULA: // "boolean formula"
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 407 "parseaut.cc"
        break;

      case symbol_kind::S_string_opt: // string_opt
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 413 "parseaut.cc"
        break;

      case symbol_kind::S_101_state_conj_2: // state-conj-2
#line 336 "parseaut.yy"
                    { delete (yysym.value.states); }
#line 419 "parseaut.cc"
        break;

      case symbol_kind::S_102_init_state_conj_2: // init-state-conj-2
#line 336 "parseaut.yy"
                    { delete (yysym.value.states); }
#line 425 "parseaut.cc"
        break;

      case symbol_kind::S_103_label_expr: // label-expr
#line 333 "parseaut.yy"
                    { bdd_delref((yysym.value.b)); }
#line 431 "parseaut.cc"
        break;

      case symbol_kind::S_105_acceptance_cond: // acceptance-cond
#line 335 "parseaut.yy"
                    { delete (yysym.value.code); }
#line 437 "parseaut.cc"
        break;

      case symbol_kind::S_123_state_conj_checked: // state-conj-checked
#line 336 "parseaut.yy"
                    { delete (yysym.value.states); }
#line 443 "parseaut.cc"
        break;

      case symbol_kind::S_pgame_succs: // pgame_succs
#line 336 "parseaut.yy"
                    { delete (yysym.value.states); }
#line 449 "parseaut.cc"
        break;

      case symbol_kind::S_145_nc_one_ident: // nc-one-ident
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 455 "parseaut.cc"
        break;

      case symbol_kind::S_146_nc_ident_list: // nc-ident-list
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 461 "parseaut.cc"
        break;

      case symbol_kind::S_147_nc_transition_block: // nc-transition-block
#line 351 "parseaut.yy"
                    {
  for (auto i = (yysym.value.list)->begin(); i != (yysym.value.list)->end(); ++i)
  {
    bdd_delref(i->first);
    delete i->second;
  }
  delete (yysym.value.list);
  }
#line 474 "parseaut.cc"
        break;

      case symbol_kind::S_149_nc_transitions: // nc-transitions
#line 351 "parseaut.yy"
                    {
  for (auto i = (yysym.value.list)->begin(); i != (yysym.value.list)->end(); ++i)
  {
    bdd_delref(i->first);
    delete i->second;
  }
  delete (yysym.value.list);
  }
#line 487 "parseaut.cc"
        break;

      case symbol_kind::S_150_nc_formula_or_ident: // nc-formula-or-ident
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 493 "parseaut.cc"
        break;

      case symbol_kind::S_151_nc_formula: // nc-formula
#line 333 "parseaut.yy"
                    { bdd_delref((yysym.value.b)); }
#line 499 "parseaut.cc"
        break;

      case symbol_kind::S_152_nc_opt_dest: // nc-opt-dest
#line 332 "parseaut.yy"
                    { delete (yysym.value.str); }
#line 505 "parseaut.cc"
        break;

      case symbol_kind::S_153_nc_src_dest: // nc-src-dest
#line 334 "parseaut.yy"
                    { bdd_delref((yysym.value.p)->first); delete (yysym.value.p)->second; delete (yysym.value.p); }
#line 511 "parseaut.cc"
        break;

      case symbol_kind::S_154_nc_transition: // nc-transition
#line 334 "parseaut.yy"
                    { bdd_delref((yysym.value.p)->first); delete (yysym.value.p)->second; delete (yysym.value.p); }
#line 517 "parseaut.cc"
        break;

      default:
        break;
    }
  }

#if HOAYYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        switch (yykind)
    {
      case symbol_kind::S_IDENTIFIER: // "identifier"
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 549 "parseaut.cc"
        break;

      case symbol_kind::S_HEADERNAME: // "header name"
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 559 "parseaut.cc"
        break;

      case symbol_kind::S_ANAME: // "alias name"
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 569 "parseaut.cc"
        break;

      case symbol_kind::S_STRING: // "string"
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 579 "parseaut.cc"
        break;

      case symbol_kind::S_INT: // "integer"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 585 "parseaut.cc"
        break;

      case symbol_kind::S_25_: // '['
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 595 "parseaut.cc"
        break;

      case symbol_kind::S_LINEDIRECTIVE: // "#line"
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 605 "parseaut.cc"
        break;

      case symbol_kind::S_FORMULA: // "boolean formula"
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 615 "parseaut.cc"
        break;

      case symbol_kind::S_LBTT: // "LBTT header"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 621 "parseaut.cc"
        break;

      case symbol_kind::S_INT_S: // "state acceptance"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 627 "parseaut.cc"
        break;

      case symbol_kind::S_LBTT_EMPTY: // "acceptance sets for empty automaton"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 633 "parseaut.cc"
        break;

      case symbol_kind::S_ACC: // "acceptance set"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 639 "parseaut.cc"
        break;

      case symbol_kind::S_STATE_NUM: // "state number"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 645 "parseaut.cc"
        break;

      case symbol_kind::S_DEST_NUM: // "destination number"
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 651 "parseaut.cc"
        break;

      case symbol_kind::S_string_opt: // string_opt
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 661 "parseaut.cc"
        break;

      case symbol_kind::S_101_state_conj_2: // state-conj-2
#line 337 "parseaut.yy"
                 {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 680 "parseaut.cc"
        break;

      case symbol_kind::S_102_init_state_conj_2: // init-state-conj-2
#line 337 "parseaut.yy"
                 {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 699 "parseaut.cc"
        break;

      case symbol_kind::S_104_acc_set: // acc-set
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 705 "parseaut.cc"
        break;

      case symbol_kind::S_107_state_num: // state-num
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 711 "parseaut.cc"
        break;

      case symbol_kind::S_108_checked_state_num: // checked-state-num
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 717 "parseaut.cc"
        break;

      case symbol_kind::S_123_state_conj_checked: // state-conj-checked
#line 337 "parseaut.yy"
                 {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 736 "parseaut.cc"
        break;

      case symbol_kind::S_sign: // sign
#line 364 "parseaut.yy"
                 { debug_stream() << (yysym.value.num); }
#line 742 "parseaut.cc"
        break;

      case symbol_kind::S_pgame_succs: // pgame_succs
#line 337 "parseaut.yy"
                 {
  auto& os = debug_stream();
  os << '{';
  bool notfirst = false;
  for (auto i: *(yysym.value.states))
  {
    if (notfirst)
      os << ", ";
    else
      notfirst = true;
    os << i;
  }
  os << '}';
}
#line 761 "parseaut.cc"
        break;

      case symbol_kind::S_145_nc_one_ident: // nc-one-ident
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 771 "parseaut.cc"
        break;

      case symbol_kind::S_146_nc_ident_list: // nc-ident-list
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 781 "parseaut.cc"
        break;

      case symbol_kind::S_150_nc_formula_or_ident: // nc-formula-or-ident
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 791 "parseaut.cc"
        break;

      case symbol_kind::S_152_nc_opt_dest: // nc-opt-dest
#line 359 "parseaut.yy"
                 {
    if ((yysym.value.str))
      debug_stream() << *(yysym.value.str);
    else
      debug_stream() << "\"\""; }
#line 801 "parseaut.cc"
        break;

      default:
        break;
    }
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if HOAYYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // HOAYYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
#line 218 "parseaut.yy"
{ yyla.location = res.h->loc = initial_loc; }

#line 923 "parseaut.cc"


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location, scanner, PARSE_ERROR_LIST));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // aut: aut-1
#line 367 "parseaut.yy"
               { res.h->loc = yylhs.location; YYACCEPT; }
#line 1061 "parseaut.cc"
    break;

  case 3: // $@1: %empty
#line 371 "parseaut.yy"
                   { res.h->filename = *(yystack_[0].value.str); }
#line 1067 "parseaut.cc"
    break;

  case 5: // aut: "end of file"
#line 372 "parseaut.yy"
               { YYABORT; }
#line 1073 "parseaut.cc"
    break;

  case 6: // aut: error "end of file"
#line 373 "parseaut.yy"
                     { YYABORT; }
#line 1079 "parseaut.cc"
    break;

  case 7: // aut: error aut-1
#line 375 "parseaut.yy"
     {
       error(yystack_[1].location, "leading garbage was ignored");
       res.h->loc = yystack_[0].location;
       YYACCEPT;
     }
#line 1089 "parseaut.cc"
    break;

  case 8: // aut-1: hoa
#line 381 "parseaut.yy"
             { res.h->type = spot::parsed_aut_type::HOA; }
#line 1095 "parseaut.cc"
    break;

  case 9: // aut-1: never
#line 382 "parseaut.yy"
             { res.h->type = spot::parsed_aut_type::NeverClaim; }
#line 1101 "parseaut.cc"
    break;

  case 10: // aut-1: lbtt
#line 383 "parseaut.yy"
             { res.h->type = spot::parsed_aut_type::LBTT; }
#line 1107 "parseaut.cc"
    break;

  case 12: // aut-1: pgame
#line 385 "parseaut.yy"
             { res.h->type = spot::parsed_aut_type::PGAME; }
#line 1113 "parseaut.cc"
    break;

  case 15: // string_opt: %empty
#line 394 "parseaut.yy"
                   { (yylhs.value.str) = nullptr; }
#line 1119 "parseaut.cc"
    break;

  case 16: // string_opt: "string"
#line 395 "parseaut.yy"
                   { (yylhs.value.str) = (yystack_[0].value.str); }
#line 1125 "parseaut.cc"
    break;

  case 19: // header: format-version header-items
#line 399 "parseaut.yy"
        {
          bool v1plus = strverscmp("v1", res.format_version.c_str()) < 0;
	  // Preallocate the states if we know their number.
	  if (res.states >= 0)
	    {
	      unsigned states = res.states;
	      for (auto& p : res.start)
                for (unsigned s: p.second)
                  if ((unsigned) res.states <= s)
                    {
                      error(p.first, "initial state number is larger "
                            "than state count...");
                      error(res.states_loc, "... declared here.");
                      states = std::max(states, s + 1);
                    }
	      if (res.opts.want_kripke)
		res.h->ks->new_states(states, bddfalse);
	      else
		res.h->aut->new_states(states);
	      res.info_states.resize(states);
	    }
	  if (res.accset < 0)
	    {
	      error(yylhs.location, "missing 'Acceptance:' header");
	      res.ignore_acc = true;
	    }
          if (res.unknown_ap_max >= 0 && !res.ignore_more_ap)
            {
              error(res.unknown_ap_max_location,
                    "atomic proposition used in Alias without AP declaration");
              for (auto& p: res.alias)
                p.second = bddtrue;
            }
          if (res.has_controllable_ap)
            {
              if (!res.ignore_more_ap && !res.controllable_ap.empty())
                {
                  error(res.controllable_aps_loc,
                        "controllable-AP without AP declaration");
                }
              else
                {
                  bdd cont = bddtrue;
                  unsigned n = res.controllable_ap.size();
                  unsigned maxap = res.ap.size();
                  for (unsigned i = 0; i < n; ++i)
                    {
                      unsigned c = res.controllable_ap[i];
                      if (c >= maxap)
                        {
                          error(res.controllable_ap_loc[i],
                                "controllable AP number is larger than"
                                " the number of APs...");
                          error(res.ap_loc, "... declared here.");
                        }
                      else
                        {
                          cont &= bdd_ithvar(res.ap[c]);
                        }
                    }
                  res.aut_or_ks->set_named_prop("synthesis-outputs",
                                                new bdd(cont));
                }
            }
	  // Process properties.
	  {
	    auto explicit_labels = res.prop_is_true("explicit-labels");
	    auto implicit_labels = res.prop_is_true("implicit-labels");

	    if (implicit_labels)
	      {
		if (res.opts.want_kripke)
		  error(implicit_labels.loc,
			"Kripke structure may not use implicit labels");

		if (explicit_labels)
		  {
		    error(implicit_labels.loc,
			  "'properties: implicit-labels' is incompatible "
			  "with...");
		    error(explicit_labels.loc,
			  "... 'properties: explicit-labels'.");
		  }
		else
		  {
		    res.label_style = Implicit_Labels;
		  }
	      }

	    auto trans_labels = res.prop_is_true("trans-labels");
	    auto state_labels = res.prop_is_true("state-labels");

	    if (trans_labels)
	      {
		if (res.opts.want_kripke)
		  error(trans_labels.loc,
			"Kripke structures may not use transition labels");

		if (state_labels)
		  {
		    error(trans_labels.loc,
			  "'properties: trans-labels' is incompatible with...");
		    error(state_labels.loc,
			  "... 'properties: state-labels'.");
		  }
		else
		  {
		    if (res.label_style != Implicit_Labels)
		      res.label_style = Trans_Labels;
		  }
	      }
	    else if (state_labels)
	      {
		if (res.label_style != Implicit_Labels)
		  {
		    res.label_style = State_Labels;
		  }
		else
		  {
		    error(state_labels.loc,
			  "'properties: state-labels' is incompatible with...");
		    error(implicit_labels.loc,
			  "... 'properties: implicit-labels'.");
		  }
	      }

	    if (res.opts.want_kripke && res.label_style != State_Labels)
	      error(yylhs.location,
		    "Kripke structures should use 'properties: state-labels'");

	    auto state_acc = res.prop_is_true("state-acc");
	    auto trans_acc = res.prop_is_true("trans-acc");
	    if (trans_acc)
	      {
		if (state_acc)
		  {
		    error(trans_acc.loc,
			  "'properties: trans-acc' is incompatible with...");
		    error(state_acc.loc,
			  "... 'properties: state-acc'.");
		  }
		else
		  {
		    res.acc_style = Trans_Acc;
		  }
	      }
	    else if (state_acc)
	      {
		res.acc_style = State_Acc;
	      }

            if (auto univ_branch = res.prop_is_true("univ-branch"))
              if (res.opts.want_kripke)
                error(univ_branch.loc,
                    "Kripke structures may not use 'properties: univ-branch'");
          }
	  {
	    unsigned ss = res.start.size();
	    auto det = res.prop_is_true("deterministic");
	    auto no_exist = res.prop_is_false("exist-branch");
	    if (ss > 1)
	      {
		if (det)
                  error(det.loc,
                        "deterministic automata should have at most "
                        "one initial state");
                else if (no_exist)
                  error(no_exist.loc,
                        "universal automata should have at most "
                        "one initial state");
                // res.universal defaults to maybe() and this is what
                // we need here.  In presence of multiple initial
                // state, fix_initial_state() will have to fuse them,
                // and this could turn a non-deterministic automaton
                // into a deterministic one.
	      }
	    else
	      {
		// Assume the automaton is deterministic until proven
		// wrong, or unless we are building a Kripke structure.
                if (!res.opts.want_kripke)
                  {
                    res.universal = true;
                    res.existential = true;
                  }
	      }
            for (auto& ss: res.start)
              {
                if (ss.second.size() > 1)
                  {
                    if (auto no_univ = res.prop_is_false("univ-branch"))
                      {
                        error(ss.first,
                              "conjunct initial state despite...");
                        error(no_univ.loc, "... property: !univ-branch");
                      }
                    else if (v1plus)
                      if (auto det = res.prop_is_true("deterministic"))
                        {
                          error(ss.first,
                                "conjunct initial state despite...");
                          error(det.loc, "... property: deterministic");
                        }
                    res.existential = false;
                  }
              }
	    auto complete = res.prop_is_true("complete");
	    if (ss < 1)
	      {
		if (complete)
		  {
		    error(complete.loc,
			  "complete automata should have at least "
			  "one initial state");
		  }
		res.complete = false;
	      }
	    else
	      {
		// Assume the automaton is complete until proven
		// wrong.
                if (!res.opts.want_kripke)
                  res.complete = true;
	      }
	    // if ap_count == 0, then a Kripke structure could be
	    // declared complete, although that probably doesn't
	    // matter.
	    if (res.opts.want_kripke && complete && res.ap_count > 0)
	      error(complete.loc,
		    "Kripke structure may not be complete");
	  }
	  if (res.opts.trust_hoa)
	    {
	      auto& a = res.aut_or_ks;
	      auto& p = res.props;
	      auto e = p.end();
	      auto si = p.find("stutter-invariant");
	      if (si != e)
		{
		  a->prop_stutter_invariant(si->second.val);
		  auto i = p.find("stutter-sensitive");
		  if (i != e && si->second.val == i->second.val)
		    error(i->second.loc,
			  "automaton cannot be both stutter-invariant"
			  "and stutter-sensitive");
		}
	      else
		{
		  auto ss = p.find("stutter-sensitive");
		  if (ss != e)
		    a->prop_stutter_invariant(!ss->second.val);
		}
	      auto iw = p.find("inherently-weak");
	      auto vw = p.find("very-weak");
	      auto w = p.find("weak");
	      auto t = p.find("terminal");
              if (vw != e)
                {
                  a->prop_very_weak(vw->second.val);
                  if (w != e && !w->second.val && vw->second.val)
                    {
		      error(w->second.loc,
                            "'properties: !weak' contradicts...");
		      error(vw->second.loc,
			    "... 'properties: very-weak' given here");
                    }
                  if (iw != e && !iw->second.val && vw->second.val)
                    {
		      error(iw->second.loc,
                            "'properties: !inherently-weak' contradicts...");
		      error(vw->second.loc,
			    "... 'properties: very-weak' given here");
                    }
                }
	      if (iw != e)
		{
		  a->prop_inherently_weak(iw->second.val);
		  if (w != e && !iw->second.val && w->second.val)
		    {
		      error(w->second.loc, "'properties: weak' contradicts...");
		      error(iw->second.loc,
			    "... 'properties: !inherently-weak' given here");
		    }
		  if (t != e && !iw->second.val && t->second.val)
		    {
		      error(t->second.loc,
			    "'properties: terminal' contradicts...");
		      error(iw->second.loc,
			    "... 'properties: !inherently-weak' given here");
		    }
		}
	      if (w != e)
		{
		  a->prop_weak(w->second.val);
		  if (t != e && !w->second.val && t->second.val)
		    {
		      error(t->second.loc,
			    "'properties: terminal' contradicts...");
		      error(w->second.loc,
			    "... 'properties: !weak' given here");
		    }
		}
	      if (t != e)
		a->prop_terminal(t->second.val);
              if (a->acc().is_t() || a->acc().is_f())
                {
                  if (w != e && !w->second.val)
                    error(w->second.loc, "an automaton with this condition"
                          " is necessarily weak");
                  if (iw != e && !iw->second.val)
                    error(iw->second.loc, "an automaton with this condition"
                          " is necessarily inherently-weak");
                  if (vw != e && !vw->second.val
                      && (res.states == 0 || res.states == 1))
                    error(vw->second.loc, "an automaton with 0 or 1 state "
                          "is necessarily very-weak");
                }
	      auto u = p.find("unambiguous");
	      if (u != e)
		{
		  a->prop_unambiguous(u->second.val);
		  auto d = p.find("deterministic");
		  if (d != e && !u->second.val && d->second.val)
		    {
		      error(d->second.loc,
			    "'properties: deterministic' contradicts...");
		      error(u->second.loc,
			    "... 'properties: !unambiguous' given here");
		    }
		}
	      auto sd = p.find("semi-deterministic");
	      if (sd != e)
		{
		  a->prop_semi_deterministic(sd->second.val);
		  auto d = p.find("deterministic");
		  if (d != e && !sd->second.val && d->second.val)
		    {
		      error(d->second.loc,
			    "'properties: deterministic' contradicts...");
		      error(sd->second.loc,
			    "... 'properties: !semi-deterministic' given here");
		    }
		}
	    }
	}
#line 1475 "parseaut.cc"
    break;

  case 20: // version: "identifier"
#line 746 "parseaut.yy"
         {
	   res.format_version = *(yystack_[0].value.str);
	   res.format_version_loc = yystack_[0].location;
	   delete (yystack_[0].value.str);
	 }
#line 1485 "parseaut.cc"
    break;

  case 21: // $@2: %empty
#line 752 "parseaut.yy"
                       { res.h->loc = yystack_[0].location; }
#line 1491 "parseaut.cc"
    break;

  case 24: // controllable-aps: controllable-aps "integer"
#line 756 "parseaut.yy"
     {
       res.controllable_ap.push_back((yystack_[0].value.num));
       res.controllable_ap_loc.push_back(yystack_[0].location);
     }
#line 1500 "parseaut.cc"
    break;

  case 25: // $@3: %empty
#line 762 "parseaut.yy"
     {
       if (res.ignore_more_ap)
	 {
	   error(yystack_[1].location, "ignoring this redeclaration of APs...");
	   error(res.ap_loc, "... previously declared here.");
	 }
       else
	 {
	   res.ap_count = (yystack_[0].value.num);
	   res.ap.reserve((yystack_[0].value.num));
	 }
     }
#line 1517 "parseaut.cc"
    break;

  case 26: // aps: "AP:" "integer" $@3 ap-names
#line 775 "parseaut.yy"
     {
       if (!res.ignore_more_ap)
	 {
	   res.ap_loc = yystack_[3].location + yystack_[2].location;
	   if ((int) res.ap.size() != res.ap_count)
	     {
	       std::ostringstream out;
	       out << "found " << res.ap.size()
		   << " atomic propositions instead of the "
		   << res.ap_count << " announced";
	       error(yylhs.location, out.str());
	     }
	   res.ignore_more_ap = true;
           // If we have seen Alias: before AP: we have some variable
           // renaming to perform.
           if (res.unknown_ap_max >= 0)
             {
               int apsize = res.ap.size();
               if (apsize <= res.unknown_ap_max)
                 {
                   error(res.unknown_ap_max_location,
                         "AP number is larger than the number of APs...");
                   error(yystack_[3].location, "... declared here");
                 }
               bddPair* pair = bdd_newpair();
               int max = std::min(res.unknown_ap_max, apsize - 1);
               for (int i = 0; i <= max; ++i)
                 if (i != res.ap[i])
                   bdd_setbddpair(pair, i, res.ap[i]);
               bdd extra = bddtrue;
               for (int i = apsize; i <= res.unknown_ap_max; ++i)
                 extra &= bdd_ithvar(i);
               for (auto& p: res.alias)
                 p.second = bdd_restrict(bdd_replace(p.second, pair), extra);
               bdd_freepair(pair);
             }
	 }
     }
#line 1560 "parseaut.cc"
    break;

  case 29: // header-item: "States:" "integer"
#line 817 "parseaut.yy"
           {
	     if (res.states >= 0)
	       {
		 error(yylhs.location, "redefinition of the number of states...");
		 error(res.states_loc, "... previously defined here.");
	       }
	     else
	       {
		 res.states_loc = yylhs.location;
	       }
	     if (((int) (yystack_[0].value.num)) < 0)
	       {
		 error(yylhs.location, "too many states for this implementation");
		 YYABORT;
	       }
	     res.states = std::max(res.states, (int) (yystack_[0].value.num));
	   }
#line 1582 "parseaut.cc"
    break;

  case 30: // header-item: "Start:" init-state-conj-2
#line 835 "parseaut.yy"
             {
               res.start.emplace_back(yylhs.location, *(yystack_[0].value.states));
               delete (yystack_[0].value.states);
	     }
#line 1591 "parseaut.cc"
    break;

  case 31: // header-item: "Start:" state-num
#line 840 "parseaut.yy"
             {
	       res.start.emplace_back(yylhs.location, std::vector<unsigned>{(yystack_[0].value.num)});
	     }
#line 1599 "parseaut.cc"
    break;

  case 33: // header-item: "controllable-AP:" controllable-aps
#line 845 "parseaut.yy"
             {
               res.controllable_aps_loc = yystack_[1].location;
               res.has_controllable_ap = true;
             }
#line 1608 "parseaut.cc"
    break;

  case 34: // $@4: %empty
#line 849 "parseaut.yy"
                            { res.in_alias=true; }
#line 1614 "parseaut.cc"
    break;

  case 35: // header-item: "Alias:" "alias name" $@4 label-expr
#line 850 "parseaut.yy"
             {
               res.in_alias = false;
	       if (!res.alias.emplace(*(yystack_[2].value.str), bdd_from_int((yystack_[0].value.b))).second)
		 {
		   std::ostringstream o;
		   o << "ignoring redefinition of alias @" << *(yystack_[2].value.str);
		   error(yylhs.location, o.str());
		 }
               else
                 {
                   res.alias_order.emplace_back(*(yystack_[2].value.str));
                 }
	       delete (yystack_[2].value.str);
	       bdd_delref((yystack_[0].value.b));
	     }
#line 1634 "parseaut.cc"
    break;

  case 36: // $@5: %empty
#line 866 "parseaut.yy"
              {
		if (res.ignore_more_acc)
		  {
		    error(yystack_[1].location + yystack_[0].location, "ignoring this redefinition of the "
			  "acceptance condition...");
		    error(res.accset_loc, "... previously defined here.");
		  }
		else if ((yystack_[0].value.num) > SPOT_MAX_ACCSETS)
		  {
		    error(yystack_[1].location + yystack_[0].location,
			  "this implementation cannot support such a large "
			  "number of acceptance sets");
		    YYABORT;
		  }
		else
		  {
		    res.aut_or_ks->acc().add_sets((yystack_[0].value.num));
		    res.accset = (yystack_[0].value.num);
		    res.accset_loc = yystack_[1].location + yystack_[0].location;
		  }
	     }
#line 1660 "parseaut.cc"
    break;

  case 37: // header-item: "Acceptance:" "integer" $@5 acceptance-cond
#line 888 "parseaut.yy"
             {
	       res.ignore_more_acc = true;
	       // Not setting the acceptance in case of error will
	       // force it to be true.
	       if (res.opts.want_kripke && (!(yystack_[0].value.code)->is_t() || (yystack_[2].value.num) > 0))
		 error(yystack_[2].location + yystack_[0].location,
		       "the acceptance for Kripke structure must be '0 t'");
	       else
		 res.aut_or_ks->set_acceptance((yystack_[2].value.num), *(yystack_[0].value.code));
	       delete (yystack_[0].value.code);
	     }
#line 1676 "parseaut.cc"
    break;

  case 38: // header-item: "acc-name:" "identifier" acc-spec
#line 900 "parseaut.yy"
             {
	       delete (yystack_[1].value.str);
	     }
#line 1684 "parseaut.cc"
    break;

  case 39: // header-item: "tool:" "string" string_opt
#line 904 "parseaut.yy"
             {
	       delete (yystack_[1].value.str);
	       delete (yystack_[0].value.str);
	     }
#line 1693 "parseaut.cc"
    break;

  case 40: // header-item: "name:" "string"
#line 909 "parseaut.yy"
             {
	       res.aut_or_ks->set_named_prop("automaton-name", (yystack_[0].value.str));
	     }
#line 1701 "parseaut.cc"
    break;

  case 42: // $@6: %empty
#line 914 "parseaut.yy"
             { res.highlight_edges = new std::map<unsigned, unsigned>; }
#line 1707 "parseaut.cc"
    break;

  case 44: // $@7: %empty
#line 917 "parseaut.yy"
             { res.highlight_states = new std::map<unsigned, unsigned>; }
#line 1713 "parseaut.cc"
    break;

  case 46: // $@8: %empty
#line 920 "parseaut.yy"
             { auto p = new std::vector<bool>;
               if (res.states >= 0)
                 p->reserve(res.states);
               res.state_player = p;
             }
#line 1723 "parseaut.cc"
    break;

  case 47: // header-item: "spot.state-player:" $@8 state-player
#line 925 "parseaut.yy"
             {
               res.state_player_loc = yylhs.location;
             }
#line 1731 "parseaut.cc"
    break;

  case 48: // header-item: "header name" header-spec
#line 929 "parseaut.yy"
             {
	       char c = (*(yystack_[1].value.str))[0];
	       if (c >= 'A' && c <= 'Z')
		 error(yylhs.location, "ignoring unsupported header \"" + *(yystack_[1].value.str) + ":\"\n\t"
		       "(but the capital indicates information that should not"
		       " be ignored)");
	       delete (yystack_[1].value.str);
	     }
#line 1744 "parseaut.cc"
    break;

  case 52: // ap-name: "string"
#line 942 "parseaut.yy"
         {
	   if (!res.ignore_more_ap)
	     {
	       auto f = res.env->require(*(yystack_[0].value.str));
	       int b = 0;
	       if (f == nullptr)
		 {
		   std::ostringstream out;
		   out << "unknown atomic proposition \"" << *(yystack_[0].value.str) << "\"";
		   error(yystack_[0].location, out.str());
		   b = res.aut_or_ks->register_ap("$unknown$");
		 }
	       else
		 {
		   b = res.aut_or_ks->register_ap(f);
		   if (!res.ap_set.emplace(b).second)
		     {
		       std::ostringstream out;
		       out << "duplicate atomic proposition \"" << *(yystack_[0].value.str) << "\"";
		       error(yystack_[0].location, out.str());
		     }
		 }
	       res.ap.push_back(b);
	     }
	   delete (yystack_[0].value.str);
	 }
#line 1775 "parseaut.cc"
    break;

  case 56: // acc-spec: acc-spec "identifier"
#line 973 "parseaut.yy"
            {
	      delete (yystack_[0].value.str);
	    }
#line 1783 "parseaut.cc"
    break;

  case 58: // properties: properties "identifier"
#line 978 "parseaut.yy"
              {
                bool val = true;
                // no-univ-branch was replaced by !univ-branch in HOA 1.1
                if (*(yystack_[0].value.str) == "no-univ-branch")
                  {
                    *(yystack_[0].value.str) = "univ-branch";
                    val = false;
                  }
		auto pos = res.props.emplace(*(yystack_[0].value.str), result_::prop_info{yystack_[0].location, val});
		if (pos.first->second.val != val)
		  {
		    std::ostringstream out(std::ios_base::ate);
		    error(yystack_[0].location, "'properties: "s + (val ? "" : "!")
                          + *(yystack_[0].value.str) + "' contradicts...");
		    error(pos.first->second.loc,
			  "... 'properties: "s + (val ? "!" : "") + *(yystack_[0].value.str)
			  + "' previously given here.");
		  }
		delete (yystack_[0].value.str);
	      }
#line 1808 "parseaut.cc"
    break;

  case 59: // properties: properties '!' "identifier"
#line 999 "parseaut.yy"
              {
		auto loc = yystack_[1].location + yystack_[0].location;
		auto pos =
		  res.props.emplace(*(yystack_[0].value.str), result_::prop_info{loc, false});
		if (pos.first->second.val)
		  {
		    std::ostringstream out(std::ios_base::ate);
		    error(loc, "'properties: !"s + *(yystack_[0].value.str) + "' contradicts...");
		    error(pos.first->second.loc, "... 'properties: "s + *(yystack_[0].value.str)
                          + "' previously given here.");
		  }
		delete (yystack_[0].value.str);
	      }
#line 1826 "parseaut.cc"
    break;

  case 61: // highlight-edges: highlight-edges "integer" "integer"
#line 1015 "parseaut.yy"
              {
		res.highlight_edges->emplace((yystack_[1].value.num), (yystack_[0].value.num));
	      }
#line 1834 "parseaut.cc"
    break;

  case 63: // highlight-states: highlight-states "integer" "integer"
#line 1020 "parseaut.yy"
              {
		res.highlight_states->emplace((yystack_[1].value.num), (yystack_[0].value.num));
	      }
#line 1842 "parseaut.cc"
    break;

  case 65: // state-player: state-player "integer"
#line 1026 "parseaut.yy"
              {
                if ((yystack_[0].value.num) != 0 && (yystack_[0].value.num) != 1)
                  error(yystack_[0].location, "player should be 0 or 1");
                res.state_player->emplace_back((yystack_[0].value.num));
	      }
#line 1852 "parseaut.cc"
    break;

  case 69: // header-spec: header-spec "string"
#line 1036 "parseaut.yy"
               {
		 delete (yystack_[0].value.str);
	       }
#line 1860 "parseaut.cc"
    break;

  case 70: // header-spec: header-spec "identifier"
#line 1040 "parseaut.yy"
               {
		 delete (yystack_[0].value.str);
	       }
#line 1868 "parseaut.cc"
    break;

  case 71: // state-conj-2: checked-state-num '&' checked-state-num
#line 1045 "parseaut.yy"
            {
              (yylhs.value.states) = new std::vector<unsigned>{(yystack_[2].value.num), (yystack_[0].value.num)};
            }
#line 1876 "parseaut.cc"
    break;

  case 72: // state-conj-2: state-conj-2 '&' checked-state-num
#line 1049 "parseaut.yy"
            {
              (yylhs.value.states) = (yystack_[2].value.states);
              (yylhs.value.states)->emplace_back((yystack_[0].value.num));
            }
#line 1885 "parseaut.cc"
    break;

  case 73: // init-state-conj-2: state-num '&' state-num
#line 1057 "parseaut.yy"
            {
              (yylhs.value.states) = new std::vector<unsigned>{(yystack_[2].value.num), (yystack_[0].value.num)};
            }
#line 1893 "parseaut.cc"
    break;

  case 74: // init-state-conj-2: init-state-conj-2 '&' state-num
#line 1061 "parseaut.yy"
            {
              (yylhs.value.states) = (yystack_[2].value.states);
              (yylhs.value.states)->emplace_back((yystack_[0].value.num));
            }
#line 1902 "parseaut.cc"
    break;

  case 75: // label-expr: 't'
#line 1067 "parseaut.yy"
            {
	      (yylhs.value.b) = bddtrue.id();
	    }
#line 1910 "parseaut.cc"
    break;

  case 76: // label-expr: 'f'
#line 1071 "parseaut.yy"
            {
	      (yylhs.value.b) = bddfalse.id();
	    }
#line 1918 "parseaut.cc"
    break;

  case 77: // label-expr: "integer"
#line 1075 "parseaut.yy"
            {
              if (res.in_alias && !res.ignore_more_ap)
                {
                  // We are reading Alias: before AP: has been given.
                  // Use $1 as temporary variable number.  We will relabel
                  // everything once AP: is known.
                  if (res.unknown_ap_max < (int)(yystack_[0].value.num))
                    {
                      res.unknown_ap_max = (yystack_[0].value.num);
                      res.unknown_ap_max_location = yystack_[0].location;
                      int missing_vars = 1 + bdd_varnum() - (yystack_[0].value.num);
                      if (missing_vars > 0)
                        bdd_extvarnum(missing_vars);
                    }
                  (yylhs.value.b) = bdd_ithvar((yystack_[0].value.num)).id();
                }
	      else if ((yystack_[0].value.num) >= res.ap.size())
		{
		  error(yystack_[0].location, "AP number is larger than the number of APs...");
		  error(res.ap_loc, "... declared here");
		  (yylhs.value.b) = bddtrue.id();
		}
	      else
		{
		  (yylhs.value.b) = bdd_ithvar(res.ap[(yystack_[0].value.num)]).id();
		  bdd_addref((yylhs.value.b));
		}
	    }
#line 1951 "parseaut.cc"
    break;

  case 78: // label-expr: "alias name"
#line 1104 "parseaut.yy"
            {
	      auto i = res.alias.find(*(yystack_[0].value.str));
	      if (i == res.alias.end())
		{
		  error(yylhs.location, "unknown alias @" + *(yystack_[0].value.str));
		  (yylhs.value.b) = 1;
		}
	      else
		{
		  (yylhs.value.b) = i->second.id();
		  bdd_addref((yylhs.value.b));
		}
	      delete (yystack_[0].value.str);
	    }
#line 1970 "parseaut.cc"
    break;

  case 79: // label-expr: '!' label-expr
#line 1119 "parseaut.yy"
            {
              (yylhs.value.b) = bdd_not((yystack_[0].value.b));
              bdd_delref((yystack_[0].value.b));
              bdd_addref((yylhs.value.b));
            }
#line 1980 "parseaut.cc"
    break;

  case 80: // label-expr: label-expr '&' label-expr
#line 1125 "parseaut.yy"
            {
              (yylhs.value.b) = bdd_and((yystack_[2].value.b), (yystack_[0].value.b));
              bdd_delref((yystack_[2].value.b));
              bdd_delref((yystack_[0].value.b));
              bdd_addref((yylhs.value.b));
            }
#line 1991 "parseaut.cc"
    break;

  case 81: // label-expr: label-expr '|' label-expr
#line 1132 "parseaut.yy"
            {
              (yylhs.value.b) = bdd_or((yystack_[2].value.b), (yystack_[0].value.b));
              bdd_delref((yystack_[2].value.b));
              bdd_delref((yystack_[0].value.b));
              bdd_addref((yylhs.value.b));
            }
#line 2002 "parseaut.cc"
    break;

  case 82: // label-expr: '(' label-expr ')'
#line 1139 "parseaut.yy"
          {
	    (yylhs.value.b) = (yystack_[1].value.b);
	  }
#line 2010 "parseaut.cc"
    break;

  case 83: // acc-set: "integer"
#line 1145 "parseaut.yy"
            {
	      if ((int) (yystack_[0].value.num) >= res.accset)
		{
		  if (!res.ignore_acc)
		    {
		      error(yystack_[0].location, "number is larger than the count "
			    "of acceptance sets...");
		      error(res.accset_loc, "... declared here.");
		    }
		  (yylhs.value.num) = -1U;
		}
	      else
		{
		  (yylhs.value.num) = (yystack_[0].value.num);
		}
	    }
#line 2031 "parseaut.cc"
    break;

  case 84: // acceptance-cond: "identifier" '(' acc-set ')'
#line 1163 "parseaut.yy"
                 {
		   if ((yystack_[1].value.num) != -1U)
		     {
		       res.pos_acc_sets |= res.aut_or_ks->acc().mark((yystack_[1].value.num));
		       if (*(yystack_[3].value.str) == "Inf")
                         {
                           (yylhs.value.code) = new spot::acc_cond::acc_code
                             (res.aut_or_ks->acc().inf({(yystack_[1].value.num)}));
                         }
		       else if (*(yystack_[3].value.str) == "Fin")
                         {
                           (yylhs.value.code) = new spot::acc_cond::acc_code
                             (res.aut_or_ks->acc().fin({(yystack_[1].value.num)}));
                         }
                       else
                         {
                           error(yystack_[3].location, "unknown acceptance '"s + *(yystack_[3].value.str)
                                 + "', expected Fin or Inf");
                           (yylhs.value.code) = new spot::acc_cond::acc_code;
                         }
		     }
		   else
		     {
		       (yylhs.value.code) = new spot::acc_cond::acc_code;
		     }
		   delete (yystack_[3].value.str);
		 }
#line 2063 "parseaut.cc"
    break;

  case 85: // acceptance-cond: "identifier" '(' '!' acc-set ')'
#line 1191 "parseaut.yy"
                 {
		   if ((yystack_[1].value.num) != -1U)
		     {
		       res.neg_acc_sets |= res.aut_or_ks->acc().mark((yystack_[1].value.num));
		       if (*(yystack_[4].value.str) == "Inf")
			 (yylhs.value.code) = new spot::acc_cond::acc_code
			   (res.aut_or_ks->acc().inf_neg({(yystack_[1].value.num)}));
		       else
			 (yylhs.value.code) = new spot::acc_cond::acc_code
			   (res.aut_or_ks->acc().fin_neg({(yystack_[1].value.num)}));
		     }
		   else
		     {
		       (yylhs.value.code) = new spot::acc_cond::acc_code;
		     }
		   delete (yystack_[4].value.str);
		 }
#line 2085 "parseaut.cc"
    break;

  case 86: // acceptance-cond: '(' acceptance-cond ')'
#line 1209 "parseaut.yy"
                 {
		   (yylhs.value.code) = (yystack_[1].value.code);
		 }
#line 2093 "parseaut.cc"
    break;

  case 87: // acceptance-cond: acceptance-cond '&' acceptance-cond
#line 1213 "parseaut.yy"
                 {
		   *(yystack_[0].value.code) &= std::move(*(yystack_[2].value.code));
		   (yylhs.value.code) = (yystack_[0].value.code);
		   delete (yystack_[2].value.code);
		 }
#line 2103 "parseaut.cc"
    break;

  case 88: // acceptance-cond: acceptance-cond '|' acceptance-cond
#line 1219 "parseaut.yy"
                 {
		   *(yystack_[0].value.code) |= std::move(*(yystack_[2].value.code));
		   (yylhs.value.code) = (yystack_[0].value.code);
		   delete (yystack_[2].value.code);
		 }
#line 2113 "parseaut.cc"
    break;

  case 89: // acceptance-cond: 't'
#line 1225 "parseaut.yy"
                 {
		   (yylhs.value.code) = new spot::acc_cond::acc_code;
		 }
#line 2121 "parseaut.cc"
    break;

  case 90: // acceptance-cond: 'f'
#line 1229 "parseaut.yy"
               {
	         {
		   (yylhs.value.code) = new spot::acc_cond::acc_code
		     (res.aut_or_ks->acc().fin({}));
		 }
	       }
#line 2132 "parseaut.cc"
    break;

  case 91: // body: states
#line 1238 "parseaut.yy"
      {
	for (auto& p: res.start)
          for (unsigned s: p.second)
            if (s >= res.info_states.size() || !res.info_states[s].declared)
              {
                error(p.first, "initial state " + std::to_string(s) +
                      " has no definition");
                // Pretend that the state is declared so we do not
                // mention it in the next loop.
                if (s < res.info_states.size())
                  res.info_states[s].declared = true;
                res.complete = spot::trival::maybe();
              }
	unsigned n = res.info_states.size();
	// States with number above res.states have already caused a
	// diagnostic, so let not add another one.
	if (res.states >= 0)
	  n = res.states;
        std::vector<unsigned> unused_undeclared;
	for (unsigned i = 0; i < n; ++i)
	  {
	    auto& p = res.info_states[i];
            if (!p.declared)
              {
                if (p.used)
                  error(p.used_loc,
                        "state " + std::to_string(i) + " has no definition");
                if (!p.used)
                  unused_undeclared.push_back(i);
                res.complete = false;
              }
	  }
        if (!unused_undeclared.empty())
          {
            std::ostringstream out;
            unsigned uus = unused_undeclared.size();
            int rangestart = -2;
            int rangecur = -2;
            const char* sep = uus > 1 ? "states " : "state ";
            auto print_range = [&]() {
              if (rangecur < 0)
                return;
              out << sep << rangestart;
              if (rangecur != rangestart)
                out << '-' << rangecur;
              sep = ",";
            };
            for (unsigned s: unused_undeclared)
              {
                if ((int)s != rangecur + 1)
                  {
                    print_range();
                    rangestart = s;
                  }
                rangecur = s;
              }
            print_range();
            out << (uus > 1 ? " are" : " is") << " unused and undefined";
            error(res.states_loc, out.str());

            if (auto p = res.prop_is_true("complete"))
              error(p.loc, "automaton is incomplete because it has "
                    "undefined states");
          }
        if (res.complete)
          if (auto p = res.prop_is_false("complete"))
            {
              error(yystack_[0].location, "automaton is complete...");
              error(p.loc, "... despite 'properties: !complete'");
            }
        bool det_warned = false;
        if (res.universal && res.existential)
          if (auto p = res.prop_is_false("deterministic"))
            {
              error(yystack_[0].location, "automaton is deterministic...");
              error(p.loc, "... despite 'properties: !deterministic'");
              det_warned = true;
            }
        static bool tolerant = getenv("SPOT_HOA_TOLERANT");
        if (res.universal.is_true() && !det_warned && !tolerant)
          if (auto p = res.prop_is_true("exist-branch"))
            {
              error(yystack_[0].location, "automaton has no existential branching...");
              error(p.loc, "... despite 'properties: exist-branch'\n"
                    "note: If this is an issue you cannot fix, you may disable "
                    "this diagnostic\n      by defining the SPOT_HOA_TOLERANT "
                    "environment variable.");
              det_warned = true;
            }
        if (res.existential.is_true() && !det_warned && !tolerant)
          if (auto p = res.prop_is_true("univ-branch"))
            {
              error(yystack_[0].location, "automaton is has no universal branching...");
              error(p.loc, "... despite 'properties: univ-branch'\n"
                    "note: If this is an issue you cannot fix, you may disable "
                    "this diagnostic\n      by defining the SPOT_HOA_TOLERANT "
                    "environment variable.");
              det_warned = true;
            }
        if (res.state_player)
          if (unsigned spsz = res.state_player->size(); spsz != n)
            {
              error(res.state_player_loc,
                    "ignoring state-player header because it has "s
                    + std::to_string(spsz) + " entries while automaton has "
                    + std::to_string(n) + " states");
              delete res.state_player;
              res.state_player = nullptr;
            }
      }
#line 2247 "parseaut.cc"
    break;

  case 92: // state-num: "integer"
#line 1349 "parseaut.yy"
           {
	     if (((int) (yystack_[0].value.num)) < 0)
	       {
		 error(yystack_[0].location, "state number is too large for this implementation");
		 YYABORT;
	       }
	     (yylhs.value.num) = (yystack_[0].value.num);
	   }
#line 2260 "parseaut.cc"
    break;

  case 93: // checked-state-num: state-num
#line 1359 "parseaut.yy"
                   {
		     if ((int) (yystack_[0].value.num) >= res.states)
		       {
			 if (res.states >= 0)
			   {
			     error(yystack_[0].location, "state number is larger than state "
				   "count...");
			     error(res.states_loc, "... declared here.");
			   }
			 if (res.opts.want_kripke)
			   {
			     int missing =
			       ((int) (yystack_[0].value.num)) - res.h->ks->num_states() + 1;
			     if (missing >= 0)
			       {
				 res.h->ks->new_states(missing, bddfalse);
				 res.info_states.resize
				   (res.info_states.size() + missing);
			       }
			   }
			 else
			   {
			     int missing =
			       ((int) (yystack_[0].value.num)) - res.h->aut->num_states() + 1;
			     if (missing >= 0)
			       {
				 res.h->aut->new_states(missing);
				 res.info_states.resize
				   (res.info_states.size() + missing);
			       }
			   }
		       }
		     // Remember the first place were a state is the
		     // destination of a transition.
		     if (!res.info_states[(yystack_[0].value.num)].used)
		       {
			 res.info_states[(yystack_[0].value.num)].used = true;
			 res.info_states[(yystack_[0].value.num)].used_loc = yystack_[0].location;
		       }
		     (yylhs.value.num) = (yystack_[0].value.num);
		   }
#line 2306 "parseaut.cc"
    break;

  case 95: // states: states state
#line 1403 "parseaut.yy"
        {
	  if ((res.universal.is_true() || res.complete.is_true()))
	    {
	      bdd available = bddtrue;
	      bool det = true;
	      for (auto& t: res.h->aut->out(res.cur_state))
		{
		  if (det && !bdd_implies(t.cond, available))
		    det = false;
		  available -= t.cond;
		}
	      if (res.universal.is_true() && !det)
		{
		  res.universal = false;
		  if (auto p = res.prop_is_true("deterministic"))
		    {
		      error(yystack_[0].location, "automaton is not deterministic...");
		      error(p.loc,
			    "... despite 'properties: deterministic'");
		    }
		  else if (auto p = res.prop_is_false("exist-branch"))
		    {
		      error(yystack_[0].location, "automaton has existential branching...");
		      error(p.loc,
			    "... despite 'properties: !exist-branch'");
		    }
		}
	      if (res.complete.is_true() && available != bddfalse)
		{
		  res.complete = false;
		  if (auto p = res.prop_is_true("complete"))
		    {
		      error(yystack_[0].location, "automaton is not complete...");
		      error(p.loc, "... despite 'properties: complete'");
		    }
		}
	    }
         if (res.acc_state &&
             !res.opts.want_kripke &&
             res.h->aut->get_graph().state_storage(res.cur_state).succ == 0)
           res.h->aut->new_edge(res.cur_state, res.cur_state,
                                bddfalse, res.acc_state);
	}
#line 2354 "parseaut.cc"
    break;

  case 97: // state: state-name unlabeled-edges
#line 1448 "parseaut.yy"
       {
	 if (!res.has_state_label) // Implicit labels
	   {
	     if (res.cur_guard != res.guards.end())
	       error(yylhs.location, "not enough transitions for this state");

	     if (res.label_style == State_Labels)
	       {
		 error(yystack_[0].location, "these transitions have implicit labels but the"
		       " automaton is...");
		 error(res.props["state-labels"].loc, "... declared with "
		       "'properties: state-labels'");
		 // Do not repeat this message.
		 res.label_style = Mixed_Labels;
	       }
	     res.cur_guard = res.guards.begin();
	   }
	 else if (res.opts.want_kripke)
	   {
	     res.h->ks->state_from_number(res.cur_state)->cond(res.state_label);
	   }
       }
#line 2381 "parseaut.cc"
    break;

  case 98: // state: error
#line 1471 "parseaut.yy"
       {
	 // Assume the worse.  This skips the tests about determinism
	 // we might perform on the state.
	 res.universal = spot::trival::maybe();
	 res.existential = spot::trival::maybe();
	 res.complete = spot::trival::maybe();
         // also do not try to preserve any color
         res.acc_state = {};
       }
#line 2395 "parseaut.cc"
    break;

  case 99: // state-name: "State:" state-label_opt checked-state-num string_opt state-acc_opt
#line 1483 "parseaut.yy"
          {
	    res.cur_state = (yystack_[2].value.num);
	    if (res.info_states[(yystack_[2].value.num)].declared)
	      {
		std::ostringstream o;
		o << "redeclaration of state " << (yystack_[2].value.num);
		error(yystack_[4].location + yystack_[2].location, o.str());
                // The additional transitions from extra states might
                // led us to believe that the automaton is complete
                // while it is not if we ignore them.
                if (res.complete.is_true())
                  res.complete = spot::trival::maybe();
	      }
	    res.info_states[(yystack_[2].value.num)].declared = true;
	    res.acc_state = (yystack_[0].value.mark);
	    if ((yystack_[1].value.str))
	      {
		if (!res.state_names)
		  res.state_names =
		    new std::vector<std::string>(res.states > 0 ?
						 res.states : 0);
		if (res.state_names->size() < (yystack_[2].value.num) + 1)
		  res.state_names->resize((yystack_[2].value.num) + 1);
		(*res.state_names)[(yystack_[2].value.num)] = std::move(*(yystack_[1].value.str));
		delete (yystack_[1].value.str);
	      }
	    if (res.opts.want_kripke && !res.has_state_label)
	      error(yylhs.location, "Kripke structures should have labeled states");
	  }
#line 2429 "parseaut.cc"
    break;

  case 100: // label: '[' label-expr ']'
#line 1513 "parseaut.yy"
           {
             res.cur_label = bdd_from_int((yystack_[1].value.b));
             bdd_delref((yystack_[1].value.b));
             if ((yystack_[2].value.str))
               res.fcache[*(yystack_[2].value.str)] = res.cur_label;
             delete (yystack_[2].value.str);
	   }
#line 2441 "parseaut.cc"
    break;

  case 101: // label: BDD
#line 1520 "parseaut.yy"
           { res.cur_label = bdd_from_int((yystack_[0].value.b)); }
#line 2447 "parseaut.cc"
    break;

  case 102: // label: '[' error ']'
#line 1522 "parseaut.yy"
           {
	     error(yylhs.location, "ignoring this invalid label");
	     res.cur_label = bddtrue;
             delete (yystack_[2].value.str);
	   }
#line 2457 "parseaut.cc"
    break;

  case 103: // state-label_opt: %empty
#line 1527 "parseaut.yy"
                        { res.has_state_label = false; }
#line 2463 "parseaut.cc"
    break;

  case 104: // state-label_opt: label
#line 1529 "parseaut.yy"
               {
		 res.has_state_label = true;
		 res.state_label_loc = yystack_[0].location;
		 res.state_label = res.cur_label;
		 if (res.label_style == Trans_Labels
		     || res.label_style == Implicit_Labels)
		   {
		     error(yylhs.location,
			   "state label used although the automaton was...");
		     if (res.label_style == Trans_Labels)
		       error(res.props["trans-labels"].loc,
			     "... declared with 'properties: trans-labels'"
			     " here");
		     else
		       error(res.props["implicit-labels"].loc,
			     "... declared with 'properties: implicit-labels'"
			     " here");
		     // Do not show this error anymore.
		     res.label_style = Mixed_Labels;
		   }
	       }
#line 2489 "parseaut.cc"
    break;

  case 105: // trans-label: label
#line 1551 "parseaut.yy"
                 {
		   if (res.has_state_label)
		     {
		       error(yystack_[0].location, "cannot label this edge because...");
		       error(res.state_label_loc,
			     "... the state is already labeled.");
		       res.cur_label = res.state_label;
		     }
		   if (res.label_style == State_Labels
		       || res.label_style == Implicit_Labels)
		     {
		       error(yylhs.location, "transition label used although the "
			     "automaton was...");
		       if (res.label_style == State_Labels)
			 error(res.props["state-labels"].loc,
			       "... declared with 'properties: state-labels' "
			       "here");
		       else
			 error(res.props["implicit-labels"].loc,
			       "... declared with 'properties: implicit-labels'"
			       " here");
		       // Do not show this error anymore.
		       res.label_style = Mixed_Labels;
		     }
		 }
#line 2519 "parseaut.cc"
    break;

  case 106: // acc-sig: '{' acc-sets '}'
#line 1578 "parseaut.yy"
             {
	       (yylhs.value.mark) = (yystack_[1].value.mark);
	       if (res.ignore_acc && !res.ignore_acc_silent)
		 {
		   error(yylhs.location, "ignoring acceptance sets because of "
			 "missing acceptance condition");
		   // Emit this message only once.
		   res.ignore_acc_silent = true;
		 }
	     }
#line 2534 "parseaut.cc"
    break;

  case 107: // acc-sig: '{' error '}'
#line 1589 "parseaut.yy"
             {
	       error(yylhs.location, "ignoring this invalid acceptance set");
	     }
#line 2542 "parseaut.cc"
    break;

  case 108: // acc-sets: %empty
#line 1593 "parseaut.yy"
          {
	    (yylhs.value.mark) = spot::acc_cond::mark_t({});
	  }
#line 2550 "parseaut.cc"
    break;

  case 109: // acc-sets: acc-sets acc-set
#line 1597 "parseaut.yy"
          {
	    if (res.ignore_acc || (yystack_[0].value.num) == -1U)
	      (yylhs.value.mark) = spot::acc_cond::mark_t({});
	    else
	      (yylhs.value.mark) = (yystack_[1].value.mark) | res.aut_or_ks->acc().mark((yystack_[0].value.num));
	  }
#line 2561 "parseaut.cc"
    break;

  case 110: // state-acc_opt: %empty
#line 1605 "parseaut.yy"
               {
                 (yylhs.value.mark) = spot::acc_cond::mark_t({});
               }
#line 2569 "parseaut.cc"
    break;

  case 111: // state-acc_opt: acc-sig
#line 1609 "parseaut.yy"
               {
		 (yylhs.value.mark) = (yystack_[0].value.mark);
		 if (res.acc_style == Trans_Acc)
		   {
		     error(yylhs.location, "state-based acceptance used despite...");
		     error(res.props["trans-acc"].loc,
			   "... declaration of transition-based acceptance.");
		     res.acc_style = Mixed_Acc;
		   }
	       }
#line 2584 "parseaut.cc"
    break;

  case 112: // trans-acc_opt: %empty
#line 1620 "parseaut.yy"
               {
                 (yylhs.value.mark) = spot::acc_cond::mark_t({});
               }
#line 2592 "parseaut.cc"
    break;

  case 113: // trans-acc_opt: acc-sig
#line 1624 "parseaut.yy"
               {
		 (yylhs.value.mark) = (yystack_[0].value.mark);
		 res.trans_acc_seen = true;
		 if (res.acc_style == State_Acc)
		   {
		     error(yylhs.location, "trans-based acceptance used despite...");
		     error(res.props["state-acc"].loc,
			   "... declaration of state-based acceptance.");
		     res.acc_style = Mixed_Acc;
		   }
	       }
#line 2608 "parseaut.cc"
    break;

  case 119: // incorrectly-unlabeled-edge: checked-state-num trans-acc_opt
#line 1643 "parseaut.yy"
                            {
			      bdd cond = bddtrue;
			      if (!res.has_state_label)
				error(yylhs.location, "missing label for this edge "
				      "(previous edge is labeled)");
			      else
				cond = res.state_label;
			      if (cond != bddfalse || !res.opts.drop_false_edges)
				{
                                  unsigned e;
				  if (res.opts.want_kripke)
				    e = res.h->ks->new_edge(res.cur_state, (yystack_[1].value.num));
				  else
				    e = res.h->aut->new_edge(res.cur_state, (yystack_[1].value.num),
                                                             cond,
                                                             (yystack_[0].value.mark) |
                                                             res.acc_state);
                                  res.edge_map.push_back(e);
				}
			    }
#line 2633 "parseaut.cc"
    break;

  case 120: // labeled-edge: trans-label checked-state-num trans-acc_opt
#line 1664 "parseaut.yy"
              {
                unsigned e = 0;
		if (res.cur_label != bddfalse
                    || !res.opts.drop_false_edges
                    // As a hack to allow states to be accepting
                    // even if they do not have transitions, we
                    // do not ignore false-labeled self-loops if they
                    // have some colors)
                    || ((yystack_[1].value.num) == res.cur_state && !!((yystack_[0].value.mark) | res.acc_state)))
		  {
		    if (res.opts.want_kripke)
		      e = res.h->ks->new_edge(res.cur_state, (yystack_[1].value.num));
		    else
		      e = res.h->aut->new_edge(res.cur_state, (yystack_[1].value.num),
                                               res.cur_label,
                                               (yystack_[0].value.mark) | res.acc_state);
		  }
                res.edge_map.push_back(e);
	      }
#line 2657 "parseaut.cc"
    break;

  case 121: // labeled-edge: trans-label state-conj-checked trans-acc_opt
#line 1684 "parseaut.yy"
              {
                unsigned e = 0;
                if (res.cur_label != bddfalse || !res.opts.drop_false_edges)
                  {
                    assert(!res.opts.want_kripke);
                    e = res.h->aut->new_univ_edge(res.cur_state,
                                                  (yystack_[1].value.states)->begin(), (yystack_[1].value.states)->end(),
                                                  res.cur_label,
                                                  (yystack_[0].value.mark) | res.acc_state);
                  }
                res.edge_map.push_back(e);
                delete (yystack_[1].value.states);
	      }
#line 2675 "parseaut.cc"
    break;

  case 122: // state-conj-checked: state-conj-2
#line 1699 "parseaut.yy"
              {
                (yylhs.value.states) = (yystack_[0].value.states);
                if (auto ub = res.prop_is_false("univ-branch"))
                  {
                    error(yystack_[0].location, "universal branch used despite"
                          " previous declaration...");
                    error(ub.loc, "... here");
                  }
                res.existential = false;
              }
#line 2690 "parseaut.cc"
    break;

  case 126: // unlabeled-edge: checked-state-num trans-acc_opt
#line 1717 "parseaut.yy"
                {
		  bdd cond;
		  if (res.has_state_label)
		    {
		      cond = res.state_label;
		    }
		  else
		    {
		      if (res.guards.empty())
			fill_guards(res);
		      if (res.cur_guard == res.guards.end())
			{
			  error(yylhs.location, "too many transitions for this state, "
				"ignoring this one");
			  cond = bddfalse;
			}
		      else
			{
			  cond = *res.cur_guard++;
			}
		    }
                  unsigned e = 0;
		  if (cond != bddfalse || !res.opts.drop_false_edges)
		    {
		      if (res.opts.want_kripke)
			e = res.h->ks->new_edge(res.cur_state, (yystack_[1].value.num));
		      else
			e = res.h->aut->new_edge(res.cur_state, (yystack_[1].value.num),
                                                 cond, (yystack_[0].value.mark) | res.acc_state);
		    }
                  res.edge_map.push_back(e);
		}
#line 2727 "parseaut.cc"
    break;

  case 127: // unlabeled-edge: state-conj-checked trans-acc_opt
#line 1750 "parseaut.yy"
                {
		  bdd cond;
		  if (res.has_state_label)
		    {
		      cond = res.state_label;
		    }
		  else
		    {
		      if (res.guards.empty())
			fill_guards(res);
		      if (res.cur_guard == res.guards.end())
			{
			  error(yylhs.location, "too many transitions for this state, "
				"ignoring this one");
			  cond = bddfalse;
			}
		      else
			{
			  cond = *res.cur_guard++;
			}
		    }
                  unsigned e = 0;
		  if (cond != bddfalse || !res.opts.drop_false_edges)
		    {
		      assert(!res.opts.want_kripke);
                      e = res.h->aut->new_univ_edge(res.cur_state,
                                                    (yystack_[1].value.states)->begin(), (yystack_[1].value.states)->end(),
                                                    cond, (yystack_[0].value.mark) | res.acc_state);
		    }
                  res.edge_map.push_back(e);
                  delete (yystack_[1].value.states);
		}
#line 2764 "parseaut.cc"
    break;

  case 128: // incorrectly-labeled-edge: trans-label unlabeled-edge
#line 1783 "parseaut.yy"
                          {
			    error(yystack_[1].location, "ignoring this label, because previous"
				  " edge has no label");
                          }
#line 2773 "parseaut.cc"
    break;

  case 130: // dstar: dstar_type error "end of DSTAR automaton"
#line 1795 "parseaut.yy"
       {
	 error(yylhs.location, "failed to parse this as an ltl2dstar automaton");
       }
#line 2781 "parseaut.cc"
    break;

  case 131: // dstar_type: "DRA"
#line 1800 "parseaut.yy"
       {
         res.h->type = spot::parsed_aut_type::DRA;
         res.plus = 1;
         res.minus = 0;
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location,
		   "cannot read a Kripke structure out of a DSTAR automaton");
	     YYABORT;
	   }
       }
#line 2797 "parseaut.cc"
    break;

  case 132: // dstar_type: "DSA"
#line 1812 "parseaut.yy"
       {
	 res.h->type = spot::parsed_aut_type::DSA;
         res.plus = 0;
         res.minus = 1;
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location,
		   "cannot read a Kripke structure out of a DSTAR automaton");
	     YYABORT;
	   }
       }
#line 2813 "parseaut.cc"
    break;

  case 133: // dstar_header: dstar_sizes
#line 1825 "parseaut.yy"
  {
    if (res.states < 0)
      error(yystack_[0].location, "missing state count");
    if (!res.ignore_more_acc)
      error(yystack_[0].location, "missing acceptance-pair count");
    if (res.start.empty())
      error(yystack_[0].location, "missing start-state number");
    if (!res.ignore_more_ap)
      error(yystack_[0].location, "missing atomic propositions definition");

    if (res.states > 0)
      {
	res.h->aut->new_states(res.states);
	res.info_states.resize(res.states);
      }
    res.acc_style = State_Acc;
    res.universal = true;
    res.existential = true;
    res.complete = true;
    fill_guards(res);
    res.cur_guard = res.guards.end();
  }
#line 2840 "parseaut.cc"
    break;

  case 136: // dstar_sizes: dstar_sizes "Acceptance-Pairs:" "integer"
#line 1851 "parseaut.yy"
  {
    if (res.ignore_more_acc)
      {
	error(yystack_[2].location + yystack_[1].location, "ignoring this redefinition of the "
	      "acceptance pairs...");
	error(res.accset_loc, "... previously defined here.");
      }
    else{
      res.accset = (yystack_[0].value.num);
      res.h->aut->set_acceptance(2 * (yystack_[0].value.num),
				 res.h->type == spot::parsed_aut_type::DRA
				 ? spot::acc_cond::acc_code::rabin((yystack_[0].value.num))
				 : spot::acc_cond::acc_code::streett((yystack_[0].value.num)));
      res.accset_loc = yystack_[0].location;
      res.ignore_more_acc = true;
    }
  }
#line 2862 "parseaut.cc"
    break;

  case 137: // dstar_sizes: dstar_sizes "States:" "integer"
#line 1869 "parseaut.yy"
  {
    if (res.states < 0)
      {
	res.states = (yystack_[0].value.num);
      }
    else
      {
	error(yylhs.location, "redeclaration of state count");
	if ((unsigned) res.states < (yystack_[0].value.num))
	  res.states = (yystack_[0].value.num);
      }
  }
#line 2879 "parseaut.cc"
    break;

  case 138: // dstar_sizes: dstar_sizes "Start:" "integer"
#line 1882 "parseaut.yy"
  {
    res.start.emplace_back(yystack_[0].location, std::vector<unsigned>{(yystack_[0].value.num)});
  }
#line 2887 "parseaut.cc"
    break;

  case 140: // dstar_state_id: "State:" "integer" string_opt
#line 1888 "parseaut.yy"
  {
    if (res.cur_guard != res.guards.end())
      error(yystack_[2].location, "not enough transitions for previous state");
    if (res.states < 0 || (yystack_[1].value.num) >= (unsigned) res.states)
      {
	std::ostringstream o;
	if (res.states > 0)
	  {
	    o << "state numbers should be in the range [0.."
	      << res.states - 1 << "]";
	  }
	else
	  {
	    o << "no states have been declared";
	  }
	error(yystack_[1].location, o.str());
      }
    else
      {
	res.info_states[(yystack_[1].value.num)].declared = true;

	if ((yystack_[0].value.str))
	  {
	    if (!res.state_names)
	      res.state_names =
		new std::vector<std::string>(res.states > 0 ?
					     res.states : 0);
	    if (res.state_names->size() < (yystack_[1].value.num) + 1)
	      res.state_names->resize((yystack_[1].value.num) + 1);
	    (*res.state_names)[(yystack_[1].value.num)] = std::move(*(yystack_[0].value.str));
	    delete (yystack_[0].value.str);
	  }
      }

    res.cur_guard = res.guards.begin();
    res.dest_map.clear();
    res.cur_state = (yystack_[1].value.num);
  }
#line 2930 "parseaut.cc"
    break;

  case 141: // sign: '+'
#line 1927 "parseaut.yy"
          { (yylhs.value.num) = res.plus; }
#line 2936 "parseaut.cc"
    break;

  case 142: // sign: '-'
#line 1928 "parseaut.yy"
          { (yylhs.value.num) = res.minus; }
#line 2942 "parseaut.cc"
    break;

  case 143: // dstar_accsigs: %empty
#line 1932 "parseaut.yy"
  {
    (yylhs.value.mark) = spot::acc_cond::mark_t({});
  }
#line 2950 "parseaut.cc"
    break;

  case 144: // dstar_accsigs: dstar_accsigs sign "integer"
#line 1936 "parseaut.yy"
  {
    if (res.states < 0 || res.cur_state >= (unsigned) res.states)
      break;
    if (res.accset > 0 && (yystack_[0].value.num) < (unsigned) res.accset)
      {
	(yylhs.value.mark) = (yystack_[2].value.mark);
	(yylhs.value.mark).set((yystack_[0].value.num) * 2 + (yystack_[1].value.num));
      }
    else
      {
	std::ostringstream o;
	if (res.accset > 0)
	  {
	    o << "acceptance pairs should be in the range [0.."
	      << res.accset - 1 << "]";
	  }
	else
	  {
	    o << "no acceptance pairs have been declared";
	  }
	error(yystack_[0].location, o.str());
      }
  }
#line 2978 "parseaut.cc"
    break;

  case 145: // dstar_state_accsig: "Acc-Sig:" dstar_accsigs
#line 1960 "parseaut.yy"
                                             { (yylhs.value.mark) = (yystack_[0].value.mark); }
#line 2984 "parseaut.cc"
    break;

  case 147: // dstar_transitions: dstar_transitions "integer"
#line 1964 "parseaut.yy"
  {
    std::pair<map_t::iterator, bool> i =
      res.dest_map.emplace((yystack_[0].value.num), *res.cur_guard);
    if (!i.second)
      i.first->second |= *res.cur_guard;
    ++res.cur_guard;
  }
#line 2996 "parseaut.cc"
    break;

  case 150: // dstar_states: dstar_states dstar_state_id dstar_state_accsig dstar_transitions
#line 1975 "parseaut.yy"
  {
    for (auto i: res.dest_map)
      res.h->aut->new_edge(res.cur_state, i.first, i.second, (yystack_[1].value.mark));
  }
#line 3005 "parseaut.cc"
    break;

  case 151: // pgamestart: "start of PGSolver game"
#line 1985 "parseaut.yy"
       {
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location,
                   "cannot read a Kripke structure out of a PGSolver game.");
	     YYABORT;
	   }
       }
#line 3018 "parseaut.cc"
    break;

  case 152: // pgame: pgamestart pgame_nodes "end of PGSolver game"
#line 1995 "parseaut.yy"
       {
         unsigned n = res.accset;
         auto p = spot::acc_cond::acc_code::parity_max_odd(n);
         res.h->aut->set_acceptance(n, p);
         res.acc_style = State_Acc;
	 // Pretend that we have declared all states.
         n = res.h->aut->num_states();
         res.info_states.resize(n);
	 for (auto& p: res.info_states)
	   p.declared = true;
       }
#line 3034 "parseaut.cc"
    break;

  case 153: // pgame: pgamestart error "end of PGSolver game"
#line 2007 "parseaut.yy"
       {
	 error(yylhs.location, "failed to parse this as a PGSolver game");
       }
#line 3042 "parseaut.cc"
    break;

  case 156: // pgame_succs: "integer"
#line 2015 "parseaut.yy"
           { (yylhs.value.states) = new std::vector<unsigned>{(yystack_[0].value.num)}; }
#line 3048 "parseaut.cc"
    break;

  case 157: // pgame_succs: pgame_succs ',' "integer"
#line 2017 "parseaut.yy"
           {
             (yylhs.value.states) = (yystack_[2].value.states);
             (yylhs.value.states)->emplace_back((yystack_[0].value.num));
           }
#line 3057 "parseaut.cc"
    break;

  case 158: // pgame_node: "integer" "integer" "integer" pgame_succs string_opt
#line 2023 "parseaut.yy"
            {
              unsigned state = (yystack_[4].value.num);
              unsigned owner = (yystack_[2].value.num);
              if (owner > 1)
                {
                  error(yystack_[2].location, "node owner should be 0 or 1");
                  owner = 0;
                }
              // Create any missing state
              unsigned max_state = state;
              for (unsigned s: *(yystack_[1].value.states))
                max_state = std::max(max_state, s);
              unsigned n = res.h->aut->num_states();
              if (n <= max_state)
                res.h->aut->new_states(max_state + 1 - n);

              // assume the source of the first edge is initial
              if (res.start.empty())
                res.start.emplace_back(yylhs.location, std::vector<unsigned>{state});

              // Create all edges with priority $2
              spot::acc_cond::mark_t m({(yystack_[3].value.num)});
              for (unsigned s: *(yystack_[1].value.states))
                res.h->aut->new_edge(state, s, bddtrue, m);
              res.accset = std::max(res.accset, 1 + (int) (yystack_[3].value.num));

              n = res.h->aut->num_states();
              if (!res.state_player)
                res.state_player = new std::vector<bool>(n);
              else if (res.state_player->size() < n)
                res.state_player->resize(n);
              (*res.state_player)[state] = owner;

              if (std::string* name = (yystack_[0].value.str))
                {
                  if (!res.state_names)
                    res.state_names = new std::vector<std::string>(n);
                  else if (res.state_names->size() < n)
                    res.state_names->resize(n);
                  (*res.state_names)[state] = std::move(*name);
                  delete name;
                }
            }
#line 3105 "parseaut.cc"
    break;

  case 159: // $@9: %empty
#line 2072 "parseaut.yy"
       {
	 if (res.opts.want_kripke)
	   {
	     error(yylhs.location, "cannot read a Kripke structure out of a never claim.");
	     YYABORT;
	   }
	 res.namer = res.h->aut->create_namer<std::string>();
	 res.h->aut->set_buchi();
	 res.acc_style = State_Acc;
	 res.pos_acc_sets = res.h->aut->acc().all_sets();
       }
#line 3121 "parseaut.cc"
    break;

  case 160: // never: "never" $@9 '{' nc-states '}'
#line 2084 "parseaut.yy"
       {
	 // Add an accept_all state if needed.
	 if (res.accept_all_needed && !res.accept_all_seen)
	   {
	     unsigned n = res.namer->new_state("accept_all");
	     res.h->aut->new_acc_edge(n, n, bddtrue);
	   }
	 // If we aliased existing state, we have some unreachable
	 // states to remove.
	 if (res.aliased_states)
	   res.h->aut->purge_unreachable_states();
	 res.info_states.resize(res.h->aut->num_states());
	 // Pretend that we have declared all states.
	 for (auto& p: res.info_states)
	   p.declared = true;
         res.h->aut->register_aps_from_dict();
       }
#line 3143 "parseaut.cc"
    break;

  case 165: // nc-one-ident: "identifier" ':'
#line 2108 "parseaut.yy"
    {
      auto r = res.labels.insert(std::make_pair(*(yystack_[1].value.str), yystack_[1].location));
      if (!r.second)
	{
	  error(yystack_[1].location, "redefinition of "s + *(yystack_[1].value.str) + "...");
	  error(r.first->second, "... "s + *(yystack_[1].value.str) + " previously defined here");
	}
      (yylhs.value.str) = (yystack_[1].value.str);
    }
#line 3157 "parseaut.cc"
    break;

  case 166: // nc-ident-list: nc-one-ident
#line 2119 "parseaut.yy"
    {
      unsigned n = res.namer->new_state(*(yystack_[0].value.str));
      if (res.start.empty())
	{
	  // The first state is initial.
	  res.start.emplace_back(yylhs.location, std::vector<unsigned>{n});
	}
      (yylhs.value.str) = (yystack_[0].value.str);
    }
#line 3171 "parseaut.cc"
    break;

  case 167: // nc-ident-list: nc-ident-list nc-one-ident
#line 2129 "parseaut.yy"
    {
      res.aliased_states |=
	res.namer->alias_state(res.namer->get_state(*(yystack_[1].value.str)), *(yystack_[0].value.str));
      // Keep any identifier that starts with accept.
      if (strncmp("accept", (yystack_[1].value.str)->c_str(), 6))
        {
          delete (yystack_[1].value.str);
          (yylhs.value.str) = (yystack_[0].value.str);
        }
      else
        {
	  delete (yystack_[0].value.str);
	  (yylhs.value.str) = (yystack_[1].value.str);
        }
    }
#line 3191 "parseaut.cc"
    break;

  case 168: // nc-transition-block: "if" nc-transitions "fi"
#line 2147 "parseaut.yy"
    {
      (yylhs.value.list) = (yystack_[1].value.list);
    }
#line 3199 "parseaut.cc"
    break;

  case 169: // nc-transition-block: "do" nc-transitions "od"
#line 2151 "parseaut.yy"
    {
      (yylhs.value.list) = (yystack_[1].value.list);
    }
#line 3207 "parseaut.cc"
    break;

  case 170: // nc-state: nc-ident-list "skip"
#line 2157 "parseaut.yy"
    {
      if (*(yystack_[1].value.str) == "accept_all")
	res.accept_all_seen = true;

      auto acc = !strncmp("accept", (yystack_[1].value.str)->c_str(), 6) ?
	res.h->aut->acc().all_sets() : spot::acc_cond::mark_t({});
      res.namer->new_edge(*(yystack_[1].value.str), *(yystack_[1].value.str), bddtrue, acc);
      delete (yystack_[1].value.str);
    }
#line 3221 "parseaut.cc"
    break;

  case 171: // nc-state: nc-ident-list
#line 2166 "parseaut.yy"
                  { delete (yystack_[0].value.str); }
#line 3227 "parseaut.cc"
    break;

  case 172: // nc-state: nc-ident-list "false"
#line 2167 "parseaut.yy"
                          { delete (yystack_[1].value.str); }
#line 3233 "parseaut.cc"
    break;

  case 173: // nc-state: nc-ident-list nc-transition-block
#line 2169 "parseaut.yy"
    {
      auto acc = !strncmp("accept", (yystack_[1].value.str)->c_str(), 6) ?
	res.h->aut->acc().all_sets() : spot::acc_cond::mark_t({});
      for (auto& p: *(yystack_[0].value.list))
	{
	  bdd c = bdd_from_int(p.first);
	  bdd_delref(p.first);
	  res.namer->new_edge(*(yystack_[1].value.str), *p.second, c, acc);
	  delete p.second;
	}
      delete (yystack_[1].value.str);
      delete (yystack_[0].value.list);
    }
#line 3251 "parseaut.cc"
    break;

  case 174: // nc-transitions: %empty
#line 2184 "parseaut.yy"
         { (yylhs.value.list) = new std::list<pair>; }
#line 3257 "parseaut.cc"
    break;

  case 175: // nc-transitions: nc-transitions nc-transition
#line 2186 "parseaut.yy"
    {
      if ((yystack_[0].value.p))
	{
	  (yystack_[1].value.list)->push_back(*(yystack_[0].value.p));
	  delete (yystack_[0].value.p);
	}
      (yylhs.value.list) = (yystack_[1].value.list);
    }
#line 3270 "parseaut.cc"
    break;

  case 176: // nc-formula-or-ident: "boolean formula"
#line 2195 "parseaut.yy"
                     { (yylhs.value.str) = (yystack_[0].value.str); }
#line 3276 "parseaut.cc"
    break;

  case 177: // nc-formula-or-ident: "identifier"
#line 2195 "parseaut.yy"
                               { (yylhs.value.str) = (yystack_[0].value.str); }
#line 3282 "parseaut.cc"
    break;

  case 178: // nc-formula: nc-formula-or-ident
#line 2198 "parseaut.yy"
     {
       auto i = res.fcache.find(*(yystack_[0].value.str));
       if (i == res.fcache.end())
	 {
	   auto pf = spot::parse_infix_boolean(*(yystack_[0].value.str), *res.env, debug_level(),
					       true);
	   for (auto& j: pf.errors)
	     {
	       // Adjust the diagnostic to the current position.
	       spot::location here = yystack_[0].location;
	       here.end.line = here.begin.line + j.first.end.line - 1;
	       here.end.column = here.begin.column + j.first.end.column - 1;
	       here.begin.line += j.first.begin.line - 1;
	       here.begin.column += j.first.begin.column - 1;
	       res.h->errors.emplace_back(here, j.second);
	     }
           bdd cond = bddfalse;
	   if (pf.f)
	     cond = spot::formula_to_bdd(pf.f,
					 res.h->aut->get_dict(), res.h->aut);
	   (yylhs.value.b) = (res.fcache[*(yystack_[0].value.str)] = cond).id();
	 }
       else
	 {
	   (yylhs.value.b) = i->second.id();
	 }
       bdd_addref((yylhs.value.b));
       delete (yystack_[0].value.str);
     }
#line 3316 "parseaut.cc"
    break;

  case 179: // nc-formula: "false"
#line 2228 "parseaut.yy"
     {
       (yylhs.value.b) = 0;
     }
#line 3324 "parseaut.cc"
    break;

  case 180: // nc-opt-dest: %empty
#line 2234 "parseaut.yy"
    {
      (yylhs.value.str) = nullptr;
    }
#line 3332 "parseaut.cc"
    break;

  case 181: // nc-opt-dest: "->" "goto" "identifier"
#line 2238 "parseaut.yy"
    {
      (yylhs.value.str) = (yystack_[0].value.str);
    }
#line 3340 "parseaut.cc"
    break;

  case 182: // nc-opt-dest: "->" "assert" "boolean formula"
#line 2242 "parseaut.yy"
    {
      delete (yystack_[0].value.str);
      (yylhs.value.str) = new std::string("accept_all");
      res.accept_all_needed = true;
    }
#line 3350 "parseaut.cc"
    break;

  case 183: // nc-src-dest: nc-formula nc-opt-dest
#line 2249 "parseaut.yy"
    {
      // If there is no destination, do ignore the transition.
      // This happens for instance with
      //   if
      //   :: false
      //   fi
      if (!(yystack_[0].value.str))
	{
	  (yylhs.value.p) = nullptr;
	}
      else
	{
	  (yylhs.value.p) = new pair((yystack_[1].value.b), (yystack_[0].value.str));
	  res.namer->new_state(*(yystack_[0].value.str));
	}
    }
#line 3371 "parseaut.cc"
    break;

  case 184: // nc-transition: ':' ':' "atomic" '{' nc-src-dest '}'
#line 2268 "parseaut.yy"
    {
      (yylhs.value.p) = (yystack_[1].value.p);
    }
#line 3379 "parseaut.cc"
    break;

  case 185: // nc-transition: ':' ':' nc-src-dest
#line 2272 "parseaut.yy"
    {
      (yylhs.value.p) = (yystack_[0].value.p);
    }
#line 3387 "parseaut.cc"
    break;

  case 186: // lbtt: lbtt-header lbtt-body "-1"
#line 2281 "parseaut.yy"
      {
	auto& acc = res.h->aut->acc();
	unsigned num = acc.num_sets();
	res.h->aut->set_generalized_buchi(num);
	res.pos_acc_sets = acc.all_sets();
	assert(!res.states_map.empty());
	auto n = res.states_map.size();
	if (n != (unsigned) res.states)
	  {
	    std::ostringstream err;
	    err << res.states << " states have been declared, but "
		<< n << " different state numbers have been used";
	    error(yylhs.location, err.str());
	  }
	if (res.states_map.rbegin()->first > (unsigned) res.states)
	  {
	    // We have seen numbers larger that the total number of
	    // states in the automaton.  Usually this happens when the
	    // states are numbered from 1 instead of 0, but the LBTT
	    // documentation actually allow any number to be used.
	    // What we have done is to map all input state numbers 0
	    // <= .. < n to the digraph states with the same number,
	    // and any time we saw a number larger than n, we mapped
	    // it to a new state.  The correspondence is given by
	    // res.states_map.  Now we just need to remove the useless
	    // states we allocated.
	    std::vector<unsigned> rename(res.h->aut->num_states(), -1U);
	    unsigned s = 0;
	    for (auto& i: res.states_map)
	      rename[i.second] = s++;
	    assert(s == (unsigned) res.states);
	    for (auto& i: res.start)
	      i.second.front() = rename[i.second.front()];
	    res.h->aut->get_graph().defrag_states(rename, s);
	  }
	 res.info_states.resize(res.h->aut->num_states());
	 for (auto& s: res.info_states)
	   s.declared = true;
         res.h->aut->register_aps_from_dict();
      }
#line 3432 "parseaut.cc"
    break;

  case 187: // lbtt: lbtt-header-states "acceptance sets for empty automaton"
#line 2322 "parseaut.yy"
      {
        res.h->aut->set_generalized_buchi((yystack_[0].value.num));
	res.pos_acc_sets = res.h->aut->acc().all_sets();
      }
#line 3441 "parseaut.cc"
    break;

  case 188: // lbtt-header-states: "LBTT header"
#line 2328 "parseaut.yy"
                  {
		    if (res.opts.want_kripke)
		      {
			error(yylhs.location,
			      "cannot read a Kripke structure out of "
			      "an LBTT automaton");
			YYABORT;
		      }
		    res.states = (yystack_[0].value.num);
		    res.states_loc = yystack_[0].location;
		    res.h->aut->new_states((yystack_[0].value.num));
		  }
#line 3458 "parseaut.cc"
    break;

  case 189: // lbtt-header: lbtt-header-states "state acceptance"
#line 2341 "parseaut.yy"
           {
	     res.acc_mapper = new spot::acc_mapper_int(res.h->aut, (yystack_[0].value.num));
	     res.acc_style = State_Acc;
	   }
#line 3467 "parseaut.cc"
    break;

  case 190: // lbtt-header: lbtt-header-states "integer"
#line 2346 "parseaut.yy"
           {
	     res.acc_mapper = new spot::acc_mapper_int(res.h->aut, (yystack_[0].value.num));
	     res.trans_acc_seen = true;
	   }
#line 3476 "parseaut.cc"
    break;

  case 194: // lbtt-state: "state number" "integer" lbtt-acc
#line 2355 "parseaut.yy"
          {
	    if ((yystack_[2].value.num) >= (unsigned) res.states)
	      {
		auto p = res.states_map.emplace((yystack_[2].value.num), 0);
		if (p.second)
		  p.first->second = res.h->aut->new_state();
		res.cur_state = p.first->second;
	      }
	    else
	      {
		res.states_map.emplace((yystack_[2].value.num), (yystack_[2].value.num));
		res.cur_state = (yystack_[2].value.num);
	      }
	    if ((yystack_[1].value.num))
	      res.start.emplace_back(yystack_[2].location + yystack_[1].location,
                                     std::vector<unsigned>{res.cur_state});
	    res.acc_state = (yystack_[0].value.mark);
	  }
#line 3499 "parseaut.cc"
    break;

  case 195: // lbtt-acc: %empty
#line 2373 "parseaut.yy"
                 { (yylhs.value.mark) = spot::acc_cond::mark_t({}); }
#line 3505 "parseaut.cc"
    break;

  case 196: // lbtt-acc: lbtt-acc "acceptance set"
#line 2375 "parseaut.yy"
        {
	  (yylhs.value.mark)  = (yystack_[1].value.mark);
	  auto p = res.acc_mapper->lookup((yystack_[0].value.num));
	  if (p.first)
	    (yylhs.value.mark) |= p.second;
	  else
	    error(yystack_[0].location, "more acceptance sets used than declared");
	}
#line 3518 "parseaut.cc"
    break;

  case 197: // lbtt-guard: "string"
#line 2384 "parseaut.yy"
          {
	    auto pf = spot::parse_prefix_ltl(*(yystack_[0].value.str), *res.env);
	    if (!pf.f || !pf.errors.empty())
	      {
		std::string s = "failed to parse guard: ";
		s += *(yystack_[0].value.str);
		error(yylhs.location, s);
	      }
	    if (!pf.errors.empty())
	      for (auto& j: pf.errors)
		{
		  // Adjust the diagnostic to the current position.
		  spot::location here = yystack_[0].location;
		  here.end.line = here.begin.line + j.first.end.line - 1;
		  here.end.column = here.begin.column + j.first.end.column - 1;
		  here.begin.line += j.first.begin.line - 1;
		  here.begin.column += j.first.begin.column - 1;
		  res.h->errors.emplace_back(here, j.second);
		}
	    if (!pf.f)
	      {
		res.cur_label = bddtrue;
	      }
	    else
	      {
		if (!pf.f.is_boolean())
		  {
		    error(yylhs.location,
			  "non-Boolean transition label (replaced by true)");
		    res.cur_label = bddtrue;
		  }
		else
		  {
		    res.cur_label =
		      formula_to_bdd(pf.f, res.h->aut->get_dict(), res.h->aut);
		  }
	      }
	    delete (yystack_[0].value.str);
	  }
#line 3562 "parseaut.cc"
    break;

  case 199: // lbtt-transitions: lbtt-transitions "destination number" lbtt-acc lbtt-guard
#line 2425 "parseaut.yy"
                {
		  unsigned dst = (yystack_[2].value.num);
		  if (dst >= (unsigned) res.states)
		    {
		      auto p = res.states_map.emplace(dst, 0);
		      if (p.second)
			p.first->second = res.h->aut->new_state();
		      dst = p.first->second;
		    }
		  else
		    {
		      res.states_map.emplace(dst, dst);
		    }
		  res.h->aut->new_edge(res.cur_state, dst,
				       res.cur_label,
				       res.acc_state | (yystack_[1].value.mark));
		}
#line 3584 "parseaut.cc"
    break;


#line 3588 "parseaut.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short parser::yypact_ninf_ = -161;

  const short parser::yytable_ninf_ = -134;

  const short
  parser::yypact_[] =
  {
      16,  -161,    18,    19,  -161,  -161,  -161,  -161,  -161,  -161,
      22,  -161,  -161,    81,  -161,  -161,    31,   119,  -161,  -161,
    -161,    42,  -161,  -161,  -161,    13,    51,    16,    11,  -161,
    -161,   143,   101,    87,    89,   141,    70,    76,  -161,  -161,
    -161,   120,   116,  -161,  -161,  -161,  -161,   155,   161,    14,
    -161,   153,   154,   156,   157,   158,   163,  -161,   162,   164,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
     160,  -161,   117,  -161,  -161,   165,  -161,   110,    97,  -161,
      91,  -161,  -161,  -161,     8,  -161,   112,  -161,  -161,   151,
     159,  -161,  -161,  -161,  -161,   166,   168,  -161,    -7,  -161,
    -161,  -161,    20,   167,    95,   169,  -161,  -161,   133,  -161,
    -161,   155,  -161,  -161,  -161,  -161,  -161,  -161,     3,  -161,
    -161,   154,   170,  -161,   -29,  -161,   154,  -161,   112,  -161,
     129,   112,  -161,   154,   154,  -161,    65,   -11,    32,  -161,
    -161,  -161,  -161,   175,   173,   174,   176,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,   177,   179,   180,  -161,  -161,
     -16,   142,  -161,  -161,    46,    64,   144,  -161,  -161,    65,
    -161,  -161,    65,    54,   168,   154,   154,     2,  -161,  -161,
     -29,   129,   129,  -161,  -161,  -161,   154,  -161,  -161,  -161,
    -161,   182,   122,   147,  -161,  -161,   -11,   132,  -161,  -161,
    -161,  -161,   186,   187,  -161,   105,  -161,  -161,  -161,   188,
    -161,  -161,     1,  -161,   135,  -161,  -161,  -161,  -161,    77,
      65,    65,  -161,   129,  -161,  -161,   148,   -13,  -161,  -161,
    -161,  -161,  -161,  -161,    86,    79,   -11,   -11,  -161,  -161,
    -161,   189,  -161,   181,  -161,  -161,  -161,    57,  -161,   183,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,   192,   171,  -161,
     184,  -161,   168,  -161,  -161,  -161,  -161,   152,  -161,  -161,
     178,  -161,   172,  -161,  -161,   104,   194,    62,   121,  -161,
    -161,  -161,  -161,   195,  -161,   185,   202,   190,  -161,  -161,
    -161,  -161
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     5,     0,     0,     3,   131,   132,   159,   151,   188,
       0,     2,     8,     0,    27,    11,     0,     0,    12,     9,
      10,     0,   192,     6,     7,     0,     0,     0,     0,     1,
      94,     0,     0,     0,     0,     0,     0,     0,   190,   189,
     187,     0,   191,    14,    20,    22,     4,   161,     0,     0,
      49,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      57,    42,    44,    46,    66,    32,    28,   130,   134,   153,
       0,   152,     0,   154,   186,     0,   198,     0,     0,   166,
     171,   162,    13,    98,   103,    95,   114,    29,    92,    30,
      31,    25,    34,    36,    53,    33,    15,    40,    41,    60,
      62,    64,    48,     0,     0,     0,   155,   195,   193,   165,
     160,   164,   170,   174,   174,   172,   167,   173,     0,   101,
     104,     0,   122,    93,   112,   105,     0,    96,   115,   116,
     112,    97,   123,     0,     0,    50,     0,     0,    38,    24,
      16,    39,    58,     0,    43,    45,    47,    70,    69,    68,
      17,    18,    67,   148,   135,     0,     0,     0,   139,   156,
      15,   194,   195,   163,     0,     0,     0,    78,    77,     0,
      75,    76,     0,     0,    15,     0,     0,     0,   113,   126,
     112,   112,   112,   118,   117,   127,     0,   124,   125,    74,
      73,    26,    35,     0,    89,    90,     0,    37,    56,    55,
      54,    59,     0,     0,    65,     0,   137,   138,   136,     0,
     158,   196,     0,   168,     0,   175,   169,   102,    79,     0,
       0,     0,   100,   110,    72,    71,     0,     0,   120,   121,
     119,   128,    52,    51,     0,     0,     0,     0,    61,    63,
     149,     0,   129,     0,   157,   197,   199,     0,    82,    81,
      80,   111,    99,   107,    83,   106,   109,     0,     0,    86,
      88,    87,    15,   143,   146,   177,   179,     0,   176,   178,
     180,   185,     0,    84,   140,   145,   150,     0,     0,   183,
      85,   141,   142,     0,   147,     0,     0,     0,   144,   184,
     181,   182
  };

  const short
  parser::yypgoto_[] =
  {
    -161,   197,  -161,   223,  -161,  -160,    88,  -161,  -161,  -161,
    -161,  -161,   123,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -135,  -111,  -158,  -161,   -50,  -116,  -161,  -161,  -161,
     145,  -161,    99,     5,  -161,  -161,  -107,  -161,  -161,  -161,
     103,   106,  -161,  -125,  -161,  -161,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,  -161,   200,
    -161,  -161,  -161,   191,  -161,  -161,   126,   124,  -161,  -161,
    -161,   -44,  -161,  -161,  -161,  -161,  -161,  -161,  -161,    80,
    -161,  -161
  };

  const short
  parser::yydefgoto_[] =
  {
       0,    10,    27,    11,    12,   141,   152,    13,    45,    14,
      26,    95,    65,   135,    31,    66,   136,   137,    99,   100,
     101,   191,   233,   138,    98,   144,   145,   146,   102,   122,
      89,   173,   256,   197,    48,   123,   124,    49,    85,    86,
     125,   121,   126,   178,   227,   252,   179,   127,   128,   183,
     129,   130,   131,   132,   188,    15,    16,   103,   104,   243,
     283,   275,   264,   276,   205,    17,    18,    36,   160,    37,
      19,    28,    78,    79,    80,   117,    81,   164,   269,   270,
     279,   271,   215,    20,    21,    22,    41,    42,    76,   161,
     246,   108
  };

  const short
  parser::yytable_[] =
  {
     210,   192,    90,   226,   166,   174,   187,   140,   176,   193,
     180,   254,   182,   142,   223,    83,     1,     2,    23,     3,
      25,     3,    29,   185,   245,   167,  -108,   168,    43,   -91,
      84,   143,    32,   118,   218,   119,   177,   219,   235,   -21,
     147,   169,     4,   148,   149,     5,     6,     5,     6,   194,
     195,   196,   198,   255,   209,     7,   199,     7,   211,   224,
     225,   231,    33,   170,   171,   172,    38,     8,  -108,     8,
       9,    44,     9,   228,   229,   230,    47,   265,   260,   261,
     150,   151,   265,   189,   190,   249,   250,   167,   213,   168,
     220,   221,   150,   151,    35,    30,   154,    39,    40,   155,
     156,    53,   274,   169,   266,   267,   240,   268,   216,   266,
     254,    77,   268,   220,   221,   236,   237,   214,   222,    68,
      34,   241,    71,   258,   257,   170,   171,   172,   157,    67,
    -133,   112,   113,   242,   114,   214,    88,   118,   115,   119,
     248,    69,   259,    35,    50,    73,   272,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,   -19,   220,   221,
      61,    62,    63,   110,    64,    70,   111,   286,   236,   237,
     287,   281,   282,    74,    75,    77,    82,    87,    88,    92,
      91,   109,    93,    94,   105,    96,   106,    97,   133,   107,
     139,   140,   162,   159,   177,   201,   134,   202,   203,   211,
     204,   206,   153,   207,   208,   232,   247,   175,   217,   234,
     238,   239,   244,   262,   253,   263,   254,   277,   284,   288,
     221,   237,   290,   278,    46,    24,   200,   158,   251,   120,
     186,   184,   181,   285,   273,   280,    72,   163,   165,     0,
     291,     0,   212,     0,     0,     0,     0,     0,     0,     0,
       0,   289,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   116
  };

  const short
  parser::yycheck_[] =
  {
     160,   136,    52,     1,     1,   121,   131,    23,    37,    20,
     126,    24,   128,    20,   174,     1,     0,     1,     0,     3,
       1,     3,     0,   130,    23,    22,    24,    24,    15,    15,
      16,    38,     1,    25,   169,    27,    65,   172,   196,    20,
      20,    38,    26,    23,    24,    29,    30,    29,    30,    60,
      61,    62,    20,    66,    70,    39,    24,    39,    57,   175,
     176,   186,    31,    60,    61,    62,    24,    51,    66,    51,
      54,    20,    54,   180,   181,   182,    65,    20,   236,   237,
      60,    61,    20,   133,   134,   220,   221,    22,    42,    24,
      36,    37,    60,    61,    24,    14,     1,    55,    56,     4,
       5,     6,   262,    38,    47,    48,     1,    50,    44,    47,
      24,    20,    50,    36,    37,    36,    37,    71,    64,    32,
       1,    16,    52,   234,    38,    60,    61,    62,    33,    28,
      35,    40,    41,    28,    43,    71,    24,    25,    47,    27,
      63,    52,    63,    24,     1,    69,   257,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    36,    37,
      17,    18,    19,    66,    21,    24,    69,    46,    36,    37,
      49,    67,    68,    53,    58,    20,    15,    24,    24,    22,
      24,    71,    24,    20,    24,    23,    69,    23,    37,    24,
      24,    23,    59,    24,    65,    20,    37,    24,    24,    57,
      24,    24,    35,    24,    24,    23,    71,    37,    64,    62,
      24,    24,    24,    24,    66,    34,    24,    65,    24,    24,
      37,    37,    20,    45,    27,     2,   138,   104,   223,    84,
     131,   128,   126,   277,    63,    63,    36,   111,   114,    -1,
      50,    -1,   162,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    80
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     0,     1,     3,    26,    29,    30,    39,    51,    54,
      73,    75,    76,    79,    81,   127,   128,   137,   138,   142,
     155,   156,   157,     0,    75,     1,    82,    74,   143,     0,
      14,    86,     1,    31,     1,    24,   139,   141,    24,    55,
      56,   158,   159,    15,    20,    80,    73,    65,   106,   109,
       1,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    17,    18,    19,    21,    84,    87,    28,    32,    52,
      24,    52,   141,    69,    53,    58,   160,    20,   144,   145,
     146,   148,    15,     1,    16,   110,   111,    24,    24,   102,
     107,    24,    22,    24,    20,    83,    23,    23,    96,    90,
      91,    92,   100,   129,   130,    24,    69,    24,   163,    71,
      66,    69,    40,    41,    43,    47,   145,   147,    25,    27,
     112,   113,   101,   107,   108,   112,   114,   119,   120,   122,
     123,   124,   125,    37,    37,    85,    88,    89,    95,    24,
      23,    77,    20,    38,    97,    98,    99,    20,    23,    24,
      60,    61,    78,    35,     1,     4,     5,    33,    84,    24,
     140,   161,    59,   148,   149,   149,     1,    22,    24,    38,
      60,    61,    62,   103,   108,    37,    37,    65,   115,   118,
     108,   123,   108,   121,   122,   118,   114,   125,   126,   107,
     107,    93,   103,    20,    60,    61,    62,   105,    20,    24,
      78,    20,    24,    24,    24,   136,    24,    24,    24,    70,
      77,    57,   161,    42,    71,   154,    44,    64,   103,   103,
      36,    37,    64,    77,   108,   108,     1,   116,   118,   118,
     118,   125,    23,    94,    62,   105,    36,    37,    24,    24,
       1,    16,    28,   131,    24,    23,   162,    71,    63,   103,
     103,   115,   117,    66,    24,    66,   104,    38,   104,    63,
     105,   105,    24,    34,   134,    20,    47,    48,    50,   150,
     151,   153,   104,    63,    77,   133,   135,    65,    45,   152,
      63,    67,    68,   132,    24,   153,    46,    49,    24,    66,
      20,    50
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    72,    73,    74,    73,    73,    73,    73,    75,    75,
      75,    75,    75,    76,    76,    77,    77,    78,    78,    79,
      80,    82,    81,    83,    83,    85,    84,    86,    86,    87,
      87,    87,    87,    87,    88,    87,    89,    87,    87,    87,
      87,    87,    90,    87,    91,    87,    92,    87,    87,    87,
      93,    93,    94,    95,    95,    95,    95,    96,    96,    96,
      97,    97,    98,    98,    99,    99,   100,   100,   100,   100,
     100,   101,   101,   102,   102,   103,   103,   103,   103,   103,
     103,   103,   103,   104,   105,   105,   105,   105,   105,   105,
     105,   106,   107,   108,   109,   109,   110,   110,   110,   111,
     112,   112,   112,   113,   113,   114,   115,   115,   116,   116,
     117,   117,   118,   118,   119,   119,   120,   120,   120,   121,
     122,   122,   123,   124,   124,   124,   125,   125,   126,   127,
     127,   128,   128,   129,   130,   130,   130,   130,   130,   130,
     131,   132,   132,   133,   133,   134,   135,   135,   136,   136,
     136,   137,   138,   138,   139,   139,   140,   140,   141,   143,
     142,   144,   144,   144,   144,   145,   146,   146,   147,   147,
     148,   148,   148,   148,   149,   149,   150,   150,   151,   151,
     152,   152,   152,   153,   154,   154,   155,   155,   156,   157,
     157,   158,   159,   159,   160,   161,   161,   162,   163,   163
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     3,     1,     2,     2,     1,     1,
       1,     1,     1,     4,     3,     0,     1,     1,     1,     2,
       1,     0,     3,     0,     2,     0,     4,     0,     2,     2,
       2,     2,     1,     2,     0,     4,     0,     4,     3,     3,
       2,     2,     0,     3,     0,     3,     0,     3,     2,     1,
       0,     2,     1,     0,     2,     2,     2,     0,     2,     3,
       0,     3,     0,     3,     0,     2,     0,     2,     2,     2,
       2,     3,     3,     3,     3,     1,     1,     1,     1,     2,
       3,     3,     3,     1,     4,     5,     3,     3,     3,     1,
       1,     1,     1,     1,     0,     2,     2,     2,     1,     5,
       3,     1,     3,     0,     1,     1,     3,     3,     0,     2,
       0,     1,     0,     1,     0,     1,     1,     2,     2,     2,
       3,     3,     1,     1,     2,     2,     2,     2,     2,     7,
       3,     1,     1,     1,     0,     2,     3,     3,     3,     2,
       3,     1,     1,     0,     3,     2,     0,     2,     0,     2,
       4,     1,     3,     3,     2,     3,     1,     3,     5,     0,
       5,     0,     1,     3,     2,     2,     1,     2,     3,     3,
       2,     1,     2,     2,     0,     2,     1,     1,     1,     1,
       0,     3,     3,     2,     6,     3,     3,     2,     1,     2,
       2,     1,     0,     3,     3,     0,     2,     1,     0,     4
  };


#if HOAYYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"HOA:\"",
  "\"States:\"", "\"Start:\"", "\"AP:\"", "\"Alias:\"", "\"Acceptance:\"",
  "\"acc-name:\"", "\"controllable-AP:\"", "\"tool:\"", "\"name:\"",
  "\"properties:\"", "\"--BODY--\"", "\"--END--\"", "\"State:\"",
  "\"spot.highlight.edges:\"", "\"spot.highlight.states:\"",
  "\"spot.state-player:\"", "\"identifier\"", "\"header name\"",
  "\"alias name\"", "\"string\"", "\"integer\"", "'['", "\"#line\"", "BDD",
  "\"end of DSTAR automaton\"", "\"DRA\"", "\"DSA\"", "\"v2\"",
  "\"explicit\"", "\"Acceptance-Pairs:\"", "\"Acc-Sig:\"", "\"---\"",
  "'|'", "'&'", "'!'", "\"never\"", "\"skip\"", "\"if\"", "\"fi\"",
  "\"do\"", "\"od\"", "\"->\"", "\"goto\"", "\"false\"", "\"atomic\"",
  "\"assert\"", "\"boolean formula\"", "\"start of PGSolver game\"",
  "\"end of PGSolver game\"", "\"-1\"", "\"LBTT header\"",
  "\"state acceptance\"", "\"acceptance sets for empty automaton\"",
  "\"acceptance set\"", "\"state number\"", "\"destination number\"",
  "'t'", "'f'", "'('", "')'", "']'", "'{'", "'}'", "'+'", "'-'", "';'",
  "','", "':'", "$accept", "aut", "$@1", "aut-1", "hoa", "string_opt",
  "BOOLEAN", "header", "version", "format-version", "$@2",
  "controllable-aps", "aps", "$@3", "header-items", "header-item", "$@4",
  "$@5", "$@6", "$@7", "$@8", "ap-names", "ap-name", "acc-spec",
  "properties", "highlight-edges", "highlight-states", "state-player",
  "header-spec", "state-conj-2", "init-state-conj-2", "label-expr",
  "acc-set", "acceptance-cond", "body", "state-num", "checked-state-num",
  "states", "state", "state-name", "label", "state-label_opt",
  "trans-label", "acc-sig", "acc-sets", "state-acc_opt", "trans-acc_opt",
  "labeled-edges", "some-labeled-edges", "incorrectly-unlabeled-edge",
  "labeled-edge", "state-conj-checked", "unlabeled-edges",
  "unlabeled-edge", "incorrectly-labeled-edge", "dstar", "dstar_type",
  "dstar_header", "dstar_sizes", "dstar_state_id", "sign", "dstar_accsigs",
  "dstar_state_accsig", "dstar_transitions", "dstar_states", "pgamestart",
  "pgame", "pgame_nodes", "pgame_succs", "pgame_node", "never", "$@9",
  "nc-states", "nc-one-ident", "nc-ident-list", "nc-transition-block",
  "nc-state", "nc-transitions", "nc-formula-or-ident", "nc-formula",
  "nc-opt-dest", "nc-src-dest", "nc-transition", "lbtt",
  "lbtt-header-states", "lbtt-header", "lbtt-body", "lbtt-states",
  "lbtt-state", "lbtt-acc", "lbtt-guard", "lbtt-transitions", YY_NULLPTR
  };
#endif


#if HOAYYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   367,   367,   371,   371,   372,   373,   374,   381,   382,
     383,   384,   385,   391,   392,   394,   395,   396,   396,   398,
     745,   752,   752,   754,   755,   762,   761,   814,   815,   816,
     834,   839,   843,   844,   849,   849,   866,   865,   899,   903,
     908,   912,   914,   913,   917,   916,   920,   919,   928,   937,
     939,   940,   941,   969,   970,   971,   972,   976,   977,   998,
    1013,  1014,  1018,  1019,  1024,  1025,  1032,  1033,  1034,  1035,
    1039,  1044,  1048,  1056,  1060,  1066,  1070,  1074,  1103,  1118,
    1124,  1131,  1138,  1144,  1162,  1190,  1208,  1212,  1218,  1224,
    1228,  1237,  1348,  1358,  1401,  1402,  1446,  1447,  1470,  1482,
    1512,  1520,  1521,  1527,  1528,  1550,  1577,  1588,  1592,  1596,
    1604,  1608,  1619,  1623,  1637,  1638,  1639,  1640,  1641,  1642,
    1663,  1683,  1698,  1713,  1714,  1715,  1716,  1749,  1782,  1793,
    1794,  1799,  1811,  1824,  1848,  1849,  1850,  1868,  1881,  1885,
    1887,  1927,  1928,  1931,  1935,  1960,  1962,  1963,  1972,  1973,
    1974,  1984,  1994,  2006,  2011,  2012,  2014,  2016,  2022,  2072,
    2071,  2102,  2103,  2104,  2105,  2107,  2118,  2128,  2146,  2150,
    2156,  2166,  2167,  2168,  2184,  2185,  2195,  2195,  2197,  2227,
    2233,  2237,  2241,  2248,  2267,  2271,  2280,  2321,  2327,  2340,
    2345,  2350,  2351,  2352,  2354,  2373,  2374,  2383,  2423,  2424
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // HOAYYDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,     2,     2,     2,    37,     2,
      62,    63,     2,    67,    70,    68,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    71,    69,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    25,     2,    64,     2,     2,     2,     2,     2,     2,
       2,     2,    61,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    60,     2,     2,     2,
       2,     2,     2,    65,    36,    66,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59
    };
    // Last valid token kind.
    const int code_max = 310;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // hoayy
#line 4349 "parseaut.cc"

#line 2443 "parseaut.yy"


static void fill_guards(result_& r)
{
  unsigned nap = r.ap.size();

  int* vars = new int[nap];
  for (unsigned i = 0; i < nap; ++i)
    vars[i] = r.ap[nap - 1 - i];

  // build the 2^nap possible guards
  r.guards.reserve(1U << nap);
  for (size_t i = 0; i < (1U << nap); ++i)
    r.guards.push_back(bdd_ibuildcube(i, nap, vars));
  r.cur_guard = r.guards.begin();

  delete[] vars;
}

void
hoayy::parser::error(const location_type& location,
		     const std::string& message)
{
  res.h->errors.emplace_back(location, message);
}

static spot::acc_cond::acc_code
fix_acceptance_aux(spot::acc_cond& acc,
		   spot::acc_cond::acc_code in, unsigned pos,
		   spot::acc_cond::mark_t onlyneg,
		   spot::acc_cond::mark_t both,
		   unsigned base)
{
  auto& w = in[pos];
  switch (w.sub.op)
    {
    case spot::acc_cond::acc_op::And:
      {
	unsigned sub = pos - w.sub.size;
	--pos;
	auto c = fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	pos -= in[pos].sub.size;
	while (sub < pos)
	  {
	    --pos;
	    c &= fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	    pos -= in[pos].sub.size;
	  }
	return c;
      }
    case spot::acc_cond::acc_op::Or:
      {
	unsigned sub = pos - w.sub.size;
	--pos;
	auto c = fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	pos -= in[pos].sub.size;
	while (sub < pos)
	  {
	    --pos;
	    c |= fix_acceptance_aux(acc, in, pos, onlyneg, both, base);
	    pos -= in[pos].sub.size;
	  }
	return c;
      }
    case spot::acc_cond::acc_op::Inf:
      return acc.inf(in[pos - 1].mark);
    case spot::acc_cond::acc_op::Fin:
      return acc.fin(in[pos - 1].mark);
    case spot::acc_cond::acc_op::FinNeg:
      {
	auto m = in[pos - 1].mark;
	auto c = acc.fin(onlyneg & m);
	spot::acc_cond::mark_t tmp = {};
	for (auto i: both.sets())
	  {
	    if (m.has(i))
	      tmp.set(base);
	    ++base;
	  }
	if (tmp)
	  c |= acc.fin(tmp);
	return c;
      }
    case spot::acc_cond::acc_op::InfNeg:
      {
	auto m = in[pos - 1].mark;
	auto c = acc.inf(onlyneg & m);
	spot::acc_cond::mark_t tmp = {};
	for (auto i: both.sets())
	  {
	    if (m.has(i))
	      tmp.set(base);
	    ++base;
	  }
	if (tmp)
	  c &= acc.inf(tmp);
	return c;
      }
    }
  SPOT_UNREACHABLE();
  return {};
}

static void fix_acceptance(result_& r)
{
  if (r.opts.want_kripke)
    return;
  auto& acc = r.h->aut->acc();

  // If a set x appears only as Inf(!x), we can complement it so that
  // we work with Inf(x) instead.
  auto onlyneg = r.neg_acc_sets - r.pos_acc_sets;
  if (onlyneg)
    {
      for (auto& t: r.h->aut->edges())
	t.acc ^= onlyneg;
    }

  // However if set x is used elsewhere, for instance in
  //   Inf(!x) & Inf(x)
  // complementing x would be wrong.  We need to create a
  // new set, y, that is the complement of x, and rewrite
  // this as Inf(y) & Inf(x).
  auto both = r.neg_acc_sets & r.pos_acc_sets;
  unsigned base = 0;
  if (both)
    {
      base = acc.add_sets(both.count());
      for (auto& t: r.h->aut->edges())
        {
          unsigned i = 0;
	  if ((t.acc & both) != both)
            for (unsigned v : both.sets())
              {
                if (!t.acc.has(v))
                  t.acc |= acc.mark(base + i);
                i++;
              }
        }
    }

  if (onlyneg || both)
    {
      auto& acc = r.h->aut->acc();
      auto code = acc.get_acceptance();
      r.h->aut->set_acceptance(acc.num_sets(),
			       fix_acceptance_aux(acc, code, code.size() - 1,
						  onlyneg, both, base));
    }
}

// Spot only supports a single initial state.
//
// If the input file does not declare any initial state (this is valid
// in the HOA format) add one nonetheless.
//
// If the input file has multiple initial states we have to merge
// them.
//
//   1) In the non-alternating case, this is as simple as making a new
//   initial state using the union of all the outgoing transitions of
//   the declared initial states.  Note that if one of the original
//   initial state has no incoming transition, then we can use it as
//   initial state, avoiding the creation of a new state.
//
//   2) In the alternating case, the input may have several initial
//   states that are conjuncts.  We have to reduce the conjuncts to a
//   single state first.

static void fix_initial_state(result_& r)
{
  std::vector<std::vector<unsigned>> start;
  start.reserve(r.start.size());
  unsigned ssz = r.info_states.size();
  for (auto& p : r.start)
    {
      std::vector<unsigned> v;
      v.reserve(p.second.size());
      for (unsigned s: p.second)
        // Ignore initial states without declaration
        // (They have been diagnosed already.)
        if (s < ssz && r.info_states[s].declared)
          v.emplace_back(s);
      if (!v.empty())
        start.push_back(v);
    }

  // If no initial state has been declared, add one.
  if (start.empty())
    {
      if (r.opts.want_kripke)
	r.h->ks->set_init_state(r.h->ks->new_state(bddfalse));
      else
	r.h->aut->set_init_state(r.h->aut->new_state());
      return;
    }

  // Remove any duplicate initial state.
  std::sort(start.begin(), start.end());
  auto res = std::unique(start.begin(), start.end());
  start.resize(std::distance(start.begin(), res));

  assert(start.size() >= 1);
  if (start.size() == 1)
    {
      if (r.opts.want_kripke)
	r.h->ks->set_init_state(start.front().front());
      else
	r.h->aut->set_univ_init_state(start.front().begin(),
                                      start.front().end());
    }
  else
    {
      if (r.opts.want_kripke)
	{
	  r.h->errors.emplace_front(r.start.front().first,
				    "Kripke structure only support "
				    "a single initial state");
	  return;
	}
      auto& aut = r.h->aut;
      // Multiple initial states.  We might need to add a fake one,
      // unless one of the actual initial state has no incoming edge.
      std::vector<unsigned char> has_incoming(aut->num_states(), 0);
      for (auto& t: aut->edges())
        for (unsigned ud: aut->univ_dests(t))
          has_incoming[ud] = 1;

      bool found = false;
      unsigned init = 0;
      bool init_alternation = false;
      for (auto& pp: start)
        if (pp.size() == 1)
          {
            unsigned p = pp.front();
            if (!has_incoming[p])
              {
                init = p;
                found = true;
              }
          }
        else
          {
            init_alternation = true;
            break;
          }

      if (!found || init_alternation)
	// We do need a fake initial state
	init = aut->new_state();
      else
        // Modifying one existing initial state may turn an incomplete
        // automaton into a complete one.  For instance if the state
        // that we elected as the future initial state was the only
        // incomplete state of the automaton.  Similarly this could
        // also turn a non-deterministic automaton into a
        // deterministic one, but we don't have to deal with this are
        // automata with multiple initial states have prop_universal()
        // set to maybe() already in prevision of what this function
        // will do.
        if (aut->prop_complete().is_false())
          aut->prop_complete(spot::trival::maybe());

      aut->set_init_state(init);

      // The non-alternating case is the easiest, we simply declare
      // the outgoing transitions of all "original initial states"
      // into the only one initial state.
      if (!init_alternation)
        {
          for (auto& pp: start)
            {
              unsigned p = pp.front();
              if (p == init)
                continue;
              if (!has_incoming[p])
                {
                  // If p has no incoming edge, we can simply take
                  // out its outgoing edges and "re-source" them on init.
                  // This will avoid creating new edges.
                  for (auto& t: aut->out(p))
                    t.src = init;
                  auto& gr = aut->get_graph();
                  auto& ps = gr.state_storage(p);
                  auto& is = gr.state_storage(init);
                  gr.edge_storage(is.succ_tail).next_succ = ps.succ;
                  is.succ_tail = ps.succ_tail;
                  ps.succ = ps.succ_tail = 0;
                  // we just created a state without successors
                  aut->prop_complete(false);
                }
              else
                {
                  // duplicate all edges
                  for (auto& t: aut->out(p))
                    aut->new_edge(init, t.dst, t.cond);
                }
            }
        }
      else
        {
          // In the alternating case, we merge outgoing transition of
          // the universal destination of conjunct initial states.
          // (Note that this loop would work for the non-alternating
          // case too, but it is more expansive, so we avoid it if we
          // can.)
          spot::outedge_combiner combiner(aut);
          bdd comb_or = bddfalse;
          for (auto& pp: start)
            {
              bdd comb_and = bddtrue;
              for (unsigned d: pp)
                comb_and &= combiner(d);
              comb_or |= comb_and;
            }
          combiner.new_dests(init, comb_or);
        }

      // Merging two states may break state-based acceptance
      // make sure all outgoing edges have the same color.
      if (aut->prop_state_acc().is_true())
        {
          bool first = true;
          spot::acc_cond::mark_t prev;
          for (auto& e: aut->out(init))
            if (first)
              {
                first = false;
                prev = e.acc;
              }
            else if (e.acc != prev)
              {
                e.acc = prev;
              }
        }
    }
}

static void fix_properties(result_& r)
{
  r.aut_or_ks->prop_universal(r.universal);
  r.aut_or_ks->prop_complete(r.complete);
  if (r.acc_style == State_Acc ||
      (r.acc_style == Mixed_Acc && !r.trans_acc_seen))
    r.aut_or_ks->prop_state_acc(true);
  if (r.aut_or_ks->acc().is_t() || r.aut_or_ks->acc().is_f())
    {
      r.aut_or_ks->prop_weak(true);
      unsigned ns;
      if (r.opts.want_kripke)
        ns = r.h->ks->num_states();
      else
        ns = r.h->aut->num_states();
      if (ns == 0 || ns == 1)
        r.aut_or_ks->prop_very_weak(true);
    }
}

static void check_version(const result_& r)
{
  if (r.h->type != spot::parsed_aut_type::HOA)
    return;
  auto& v = r.format_version;
  if (v.size() < 2 || v[0] != 'v' || v[1] < '1' || v[1] > '9')
    {
      r.h->errors.emplace_front(r.format_version_loc, "unknown HOA version");
      return;
    }
  const char* beg = &v[1];
  char* end = nullptr;
  long int vers = strtol(beg, &end, 10);
  if (vers != 1)
    {
      r.h->errors.emplace_front(r.format_version_loc,
				  "unsupported HOA version");
      return;
    }
  constexpr const char supported[] = "1";
  if (strverscmp(supported, beg) < 0 && !r.h->errors.empty())
    {
      std::ostringstream s;
      s << "we can read HOA v" << supported
	<< " but this file uses " << v << "; this might "
	"cause the following errors";
      r.h->errors.emplace_front(r.format_version_loc, s.str());
      return;
    }
}

namespace spot
{
  automaton_stream_parser::automaton_stream_parser(const std::string& name,
						   automaton_parser_options opt)
    : filename_(name), opts_(opt)
  {
    try
      {
        if (hoayyopen(name, &scanner_))
          throw std::runtime_error("Cannot open file "s + name);
      }
    catch (...)
      {
        hoayyclose(scanner_);
        throw;
      }
  }

  automaton_stream_parser::automaton_stream_parser(int fd,
						   const std::string& name,
						   automaton_parser_options opt)
    : filename_(name), opts_(opt)
  {
    try
      {
        if (hoayyopen(fd, &scanner_))
          throw std::runtime_error("Cannot open file "s + name);
      }
    catch (...)
      {
        hoayyclose(scanner_);
        throw;
      }
  }

  automaton_stream_parser::automaton_stream_parser(const char* data,
						   const std::string& filename,
						   automaton_parser_options opt)
    : filename_(filename), opts_(opt)
  {
    try
      {
        hoayystring(data, &scanner_);
      }
    catch (...)
      {
        hoayyclose(scanner_);
        throw;
      }
  }

  automaton_stream_parser::~automaton_stream_parser()
  {
    hoayyclose(scanner_);
  }

  static void raise_parse_error(const parsed_aut_ptr& pa)
  {
    if (pa->aborted)
      pa->errors.emplace_back(pa->loc, "parsing aborted");
    if (!pa->errors.empty())
      {
	std::ostringstream s;
	if (pa->format_errors(s))
	  throw parse_error(s.str());
      }
    // It is possible that pa->aut == nullptr if we reach the end of a
    // stream.  It is not necessarily an error.
  }

  parsed_aut_ptr
  automaton_stream_parser::parse(const bdd_dict_ptr& dict,
				 environment& env)
  {
  restart:
    result_ r;
    r.opts = opts_;
    r.h = std::make_shared<spot::parsed_aut>(filename_);
    if (opts_.want_kripke)
      r.aut_or_ks = r.h->ks = make_kripke_graph(dict);
    else
      r.aut_or_ks = r.h->aut = make_twa_graph(dict);
    r.env = &env;
    hoayy::parser parser(scanner_, r, last_loc);
    static bool env_debug = !!getenv("SPOT_DEBUG_PARSER");
    parser.set_debug_level(opts_.debug || env_debug);
    hoayyreset(scanner_);
    try
      {
	if (parser.parse())
	  {
	    r.h->aut = nullptr;
	    r.h->ks = nullptr;
	    r.aut_or_ks = nullptr;
	  }
      }
    catch (const spot::hoa_abort& e)
      {
	r.h->aborted = true;
	// Bison 3.0.2 lacks a += operator for locations.
	r.h->loc = r.h->loc + e.pos;
      }
    check_version(r);
    last_loc = r.h->loc;
    last_loc.step();
    filename_ = r.h->filename; // in case it was changed
    if (r.h->aborted)
      {
	if (opts_.ignore_abort)
	  goto restart;
	return r.h;
      }
    if (opts_.raise_errors)
      raise_parse_error(r.h);
    if (!r.aut_or_ks)
      return r.h;
    if (r.state_names)
      r.aut_or_ks->set_named_prop("state-names", r.state_names);
    if (r.highlight_edges)
      {
        // Update the highlight_edges map to deal with removed/added
        // edges.
        unsigned ems = r.edge_map.size();
        std::map<unsigned, unsigned> remap;
        for (auto [edgnum, color]: *r.highlight_edges)
          /* edge numbers outside of the actual number of edges read are
             not expected, but we can't trust input data */
          if (SPOT_LIKELY(edgnum > 0 && edgnum <= ems))
            if (unsigned newnum = r.edge_map[edgnum - 1]; newnum > 0)
              remap[newnum] = color;
        std::swap(remap, *r.highlight_edges);
        r.aut_or_ks->set_named_prop("highlight-edges", r.highlight_edges);
      }
    if (r.highlight_states)
      r.aut_or_ks->set_named_prop("highlight-states", r.highlight_states);
    if (r.state_player)
      r.aut_or_ks->set_named_prop("state-player", r.state_player);
    if (!r.alias_order.empty())
      {
        auto* p = new std::vector<std::pair<std::string, bdd>>();
        p->reserve(r.alias_order.size());
        auto end = r.alias_order.rend();
        for (auto i = r.alias_order.rbegin(); i != end; ++i)
          p->emplace_back(*r.alias.find(*i));
        r.aut_or_ks->set_named_prop("aliases", p);
      }
    fix_acceptance(r);
    fix_properties(r); // before fix_initial_state
    fix_initial_state(r);
    if (r.h->aut && !r.h->aut->is_existential())
      r.h->aut->merge_univ_dests();
    return r.h;
  }

  parsed_aut_ptr
  parse_aut(const std::string& filename, const bdd_dict_ptr& dict,
	    environment& env, automaton_parser_options opts)
  {
    auto localopts = opts;
    localopts.raise_errors = false;
    parsed_aut_ptr pa;
    try
      {
	automaton_stream_parser p(filename, localopts);
	pa = p.parse(dict, env);
      }
    catch (const std::runtime_error& e)
      {
	if (opts.raise_errors)
	  throw;
	parsed_aut_ptr pa = std::make_shared<spot::parsed_aut>(filename);
	pa->errors.emplace_back(spot::location(), e.what());
	return pa;
      }
    if (!pa->aut && !pa->ks && pa->errors.empty())
      pa->errors.emplace_back(pa->loc, "no automaton read (empty input?)");
    if (opts.raise_errors)
      raise_parse_error(pa);
    return pa;
  }


}

// Local Variables:
// mode: c++
// End:
