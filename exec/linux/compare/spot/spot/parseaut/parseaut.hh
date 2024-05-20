// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file parseaut.hh
 ** Define the hoayy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_HOAYY_PARSEAUT_HH_INCLUDED
# define YY_HOAYY_PARSEAUT_HH_INCLUDED
// "%code requires" blocks.
#line 32 "parseaut.yy"

#include "config.h"
#include <spot/misc/common.hh>
#include <spot/priv/robin_hood.hh>
#include <string>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <spot/twa/formula2bdd.hh>
#include <spot/parseaut/public.hh>
#include "spot/priv/accmap.hh"
#include <spot/tl/parse.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/game.hh>

using namespace std::string_literals;

#ifndef HAVE_STRVERSCMP
// If the libc does not have this, a version is compiled in lib/.
extern "C" int strverscmp(const char *s1, const char *s2);
#endif

// Work around Bison not letting us write
//  %lex-param { res.h->errors, res.fcache }
#define PARSE_ERROR_LIST res.h->errors, res.fcache

  inline namespace hoayy_support
  {
    typedef std::map<int, bdd> map_t;

    /* Cache parsed formulae.  Labels on arcs are frequently identical
       and it would be a waste of time to parse them to formula
       over and over, and to register all their atomic_propositions in
       the bdd_dict.  Keep the bdd result around so we can reuse
       it.  */
    typedef robin_hood::unordered_flat_map<std::string, bdd> formula_cache;

    typedef std::pair<int, std::string*> pair;
    typedef spot::twa_graph::namer<std::string> named_tgba_t;

    // Note: because this parser is meant to be used on a stream of
    // automata, it tries hard to recover from errors, so that we get a
    // chance to reach the end of the current automaton in order to
    // process the next one.  Several variables below are used to keep
    // track of various error conditions.
    enum label_style_t { Mixed_Labels, State_Labels, Trans_Labels,
			 Implicit_Labels };
    enum acc_style_t { Mixed_Acc, State_Acc, Trans_Acc };

    struct result_
    {
      struct state_info
      {
	bool declared = false;
	bool used = false;
	spot::location used_loc;
      };
      spot::parsed_aut_ptr h;
      spot::twa_ptr aut_or_ks;
      spot::automaton_parser_options opts;
      std::string format_version;
      spot::location format_version_loc;
      spot::environment* env;
      formula_cache fcache;
      named_tgba_t* namer = nullptr;
      spot::acc_mapper_int* acc_mapper = nullptr;
      std::vector<int> ap;
      std::vector<int> controllable_ap;
      bool has_controllable_ap = false;
      std::vector<spot::location> controllable_ap_loc;
      spot::location controllable_aps_loc;
      std::vector<bdd> guards;
      std::vector<bdd>::const_iterator cur_guard;
      // If "Alias: ..." occurs before "AP: ..." in the HOA format we
      // are in trouble because the parser would like to turn each
      // alias into a BDD, yet the atomic proposition have not been
      // declared yet.  We solve that by using arbitrary BDD variables
      // numbers (in fact we use the same number given in the Alias:
      // definition) and keeping track of the highest variable number
      // we have seen (unknown_ap_max).  Once AP: is encountered,
      // we can remap everything.  If AP: is never encountered an
      // unknown_ap_max is non-negative, then we can signal an error.
      int unknown_ap_max = -1;
      spot::location unknown_ap_max_location;
      bool in_alias = false;
      map_t dest_map;
      std::vector<state_info> info_states; // States declared and used.
      // Mapping of edges in the HOA file to edges in the automaton.
      // Edges are counted from 0 in the HOA file and from 1 in the
      // automaton.  Given edge #i in the HOA file, edge_map[i] gives
      // corresponding edge in the automaton, or 0 if that edge was
      // removed (because labeled by bddfalse).  This map is used to
      // update properties such as highlight_edges after the automaton
      // has been read.  Note that the parser may also introduce
      // unlisted edges, e.g., a bddfalse self-loop to hold the
      // acceptance of a state without declared outgoing edge.  Those
      // added edges are not a concern for this edge_map.
      std::vector<unsigned> edge_map;
      std::vector<std::pair<spot::location,
                            std::vector<unsigned>>> start; // Initial states;
      std::unordered_map<std::string, bdd> alias;
      std::vector<std::string> alias_order;
      struct prop_info
      {
	spot::location loc;
	bool val;
	operator bool() const
	{
	  return val;
	};
      };
      std::unordered_map<std::string, prop_info> props;
      spot::location states_loc;
      spot::location ap_loc;
      spot::location state_label_loc;
      spot::location accset_loc;
      spot::acc_cond::mark_t acc_state;
      spot::acc_cond::mark_t neg_acc_sets = {};
      spot::acc_cond::mark_t pos_acc_sets = {};
      int plus;
      int minus;
      std::vector<std::string>* state_names = nullptr;
      std::map<unsigned, unsigned>* highlight_edges = nullptr;
      std::map<unsigned, unsigned>* highlight_states = nullptr;
      std::map<unsigned, unsigned> states_map;
      std::vector<bool>* state_player = nullptr;
      spot::location state_player_loc;
      std::set<int> ap_set;
      unsigned cur_state;
      int states = -1;
      int ap_count = -1;
      int accset = -1;
      bdd state_label;
      bdd cur_label;
      bool has_state_label = false;
      bool ignore_more_ap = false; // Set to true after the first "AP:"
      // line has been read.
      bool ignore_acc = false;	// Set to true in case of missing
				// Acceptance: lines.
      bool ignore_acc_silent = false;
      bool ignore_more_acc = false; // Set to true after the first
				// "Acceptance:" line has been read.

      label_style_t label_style = Mixed_Labels;
      acc_style_t acc_style = Mixed_Acc;

      bool accept_all_needed = false;
      bool accept_all_seen = false;
      bool aliased_states = false;

      spot::trival universal = spot::trival::maybe();
      spot::trival existential = spot::trival::maybe();
      spot::trival complete = spot::trival::maybe();
      bool trans_acc_seen = false;

      std::map<std::string, spot::location> labels;

      prop_info prop_is_true(const std::string& p)
      {
	auto i = props.find(p);
	if (i == props.end())
	  return prop_info{spot::location(), false};
	return i->second;
      }

      prop_info prop_is_false(const std::string& p)
      {
	auto i = props.find(p);
	if (i == props.end())
	  return prop_info{spot::location(), false};
	return prop_info{i->second.loc, !i->second.val};
      }

      ~result_()
      {
	delete namer;
	delete acc_mapper;
      }
    };
  }

#line 232 "parseaut.hh"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif



#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef HOAYYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define HOAYYDEBUG 1
#  else
#   define HOAYYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define HOAYYDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined HOAYYDEBUG */

namespace hoayy {
#line 375 "parseaut.hh"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef HOAYYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define HOAYYSTYPE in C++, use %define api.value.type"
# endif
    typedef HOAYYSTYPE value_type;
#else
    /// Symbol semantic values.
    union value_type
    {
#line 221 "parseaut.yy"

  std::string* str;
  unsigned int num;
  int b;
  spot::acc_cond::mark_t mark;
  pair* p;
  std::list<pair>* list;
  spot::acc_cond::acc_code* code;
  std::vector<unsigned>* states;

#line 404 "parseaut.hh"

    };
#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef spot::location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        HOAYYEMPTY = -2,
    ENDOFFILE = 0,                 // "end of file"
    HOAYYerror = 256,              // error
    HOAYYUNDEF = 257,              // "invalid token"
    HOA = 258,                     // "HOA:"
    STATES = 259,                  // "States:"
    START = 260,                   // "Start:"
    AP = 261,                      // "AP:"
    ALIAS = 262,                   // "Alias:"
    ACCEPTANCE = 263,              // "Acceptance:"
    ACCNAME = 264,                 // "acc-name:"
    CONTROLLABLE_AP = 265,         // "controllable-AP:"
    TOOL = 266,                    // "tool:"
    NAME = 267,                    // "name:"
    PROPERTIES = 268,              // "properties:"
    BODY = 269,                    // "--BODY--"
    END = 270,                     // "--END--"
    STATE = 271,                   // "State:"
    SPOT_HIGHLIGHT_EDGES = 272,    // "spot.highlight.edges:"
    SPOT_HIGHLIGHT_STATES = 273,   // "spot.highlight.states:"
    SPOT_STATE_PLAYER = 274,       // "spot.state-player:"
    IDENTIFIER = 275,              // "identifier"
    HEADERNAME = 276,              // "header name"
    ANAME = 277,                   // "alias name"
    STRING = 278,                  // "string"
    INT = 279,                     // "integer"
    LINEDIRECTIVE = 280,           // "#line"
    BDD = 281,                     // BDD
    ENDDSTAR = 282,                // "end of DSTAR automaton"
    DRA = 283,                     // "DRA"
    DSA = 284,                     // "DSA"
    V2 = 285,                      // "v2"
    EXPLICIT = 286,                // "explicit"
    ACCPAIRS = 287,                // "Acceptance-Pairs:"
    ACCSIG = 288,                  // "Acc-Sig:"
    ENDOFHEADER = 289,             // "---"
    NEVER = 290,                   // "never"
    SKIP = 291,                    // "skip"
    IF = 292,                      // "if"
    FI = 293,                      // "fi"
    DO = 294,                      // "do"
    OD = 295,                      // "od"
    ARROW = 296,                   // "->"
    GOTO = 297,                    // "goto"
    FALSE = 298,                   // "false"
    ATOMIC = 299,                  // "atomic"
    ASSERT = 300,                  // "assert"
    FORMULA = 301,                 // "boolean formula"
    PGAME = 302,                   // "start of PGSolver game"
    ENDPGAME = 303,                // "end of PGSolver game"
    ENDAUT = 304,                  // "-1"
    LBTT = 305,                    // "LBTT header"
    INT_S = 306,                   // "state acceptance"
    LBTT_EMPTY = 307,              // "acceptance sets for empty automaton"
    ACC = 308,                     // "acceptance set"
    STATE_NUM = 309,               // "state number"
    DEST_NUM = 310                 // "destination number"
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 72, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_HOA = 3,                               // "HOA:"
        S_STATES = 4,                            // "States:"
        S_START = 5,                             // "Start:"
        S_AP = 6,                                // "AP:"
        S_ALIAS = 7,                             // "Alias:"
        S_ACCEPTANCE = 8,                        // "Acceptance:"
        S_ACCNAME = 9,                           // "acc-name:"
        S_CONTROLLABLE_AP = 10,                  // "controllable-AP:"
        S_TOOL = 11,                             // "tool:"
        S_NAME = 12,                             // "name:"
        S_PROPERTIES = 13,                       // "properties:"
        S_BODY = 14,                             // "--BODY--"
        S_END = 15,                              // "--END--"
        S_STATE = 16,                            // "State:"
        S_SPOT_HIGHLIGHT_EDGES = 17,             // "spot.highlight.edges:"
        S_SPOT_HIGHLIGHT_STATES = 18,            // "spot.highlight.states:"
        S_SPOT_STATE_PLAYER = 19,                // "spot.state-player:"
        S_IDENTIFIER = 20,                       // "identifier"
        S_HEADERNAME = 21,                       // "header name"
        S_ANAME = 22,                            // "alias name"
        S_STRING = 23,                           // "string"
        S_INT = 24,                              // "integer"
        S_25_ = 25,                              // '['
        S_LINEDIRECTIVE = 26,                    // "#line"
        S_BDD = 27,                              // BDD
        S_ENDDSTAR = 28,                         // "end of DSTAR automaton"
        S_DRA = 29,                              // "DRA"
        S_DSA = 30,                              // "DSA"
        S_V2 = 31,                               // "v2"
        S_EXPLICIT = 32,                         // "explicit"
        S_ACCPAIRS = 33,                         // "Acceptance-Pairs:"
        S_ACCSIG = 34,                           // "Acc-Sig:"
        S_ENDOFHEADER = 35,                      // "---"
        S_36_ = 36,                              // '|'
        S_37_ = 37,                              // '&'
        S_38_ = 38,                              // '!'
        S_NEVER = 39,                            // "never"
        S_SKIP = 40,                             // "skip"
        S_IF = 41,                               // "if"
        S_FI = 42,                               // "fi"
        S_DO = 43,                               // "do"
        S_OD = 44,                               // "od"
        S_ARROW = 45,                            // "->"
        S_GOTO = 46,                             // "goto"
        S_FALSE = 47,                            // "false"
        S_ATOMIC = 48,                           // "atomic"
        S_ASSERT = 49,                           // "assert"
        S_FORMULA = 50,                          // "boolean formula"
        S_PGAME = 51,                            // "start of PGSolver game"
        S_ENDPGAME = 52,                         // "end of PGSolver game"
        S_ENDAUT = 53,                           // "-1"
        S_LBTT = 54,                             // "LBTT header"
        S_INT_S = 55,                            // "state acceptance"
        S_LBTT_EMPTY = 56,                       // "acceptance sets for empty automaton"
        S_ACC = 57,                              // "acceptance set"
        S_STATE_NUM = 58,                        // "state number"
        S_DEST_NUM = 59,                         // "destination number"
        S_60_t_ = 60,                            // 't'
        S_61_f_ = 61,                            // 'f'
        S_62_ = 62,                              // '('
        S_63_ = 63,                              // ')'
        S_64_ = 64,                              // ']'
        S_65_ = 65,                              // '{'
        S_66_ = 66,                              // '}'
        S_67_ = 67,                              // '+'
        S_68_ = 68,                              // '-'
        S_69_ = 69,                              // ';'
        S_70_ = 70,                              // ','
        S_71_ = 71,                              // ':'
        S_YYACCEPT = 72,                         // $accept
        S_aut = 73,                              // aut
        S_74_1 = 74,                             // $@1
        S_75_aut_1 = 75,                         // aut-1
        S_hoa = 76,                              // hoa
        S_string_opt = 77,                       // string_opt
        S_BOOLEAN = 78,                          // BOOLEAN
        S_header = 79,                           // header
        S_version = 80,                          // version
        S_81_format_version = 81,                // format-version
        S_82_2 = 82,                             // $@2
        S_83_controllable_aps = 83,              // controllable-aps
        S_aps = 84,                              // aps
        S_85_3 = 85,                             // $@3
        S_86_header_items = 86,                  // header-items
        S_87_header_item = 87,                   // header-item
        S_88_4 = 88,                             // $@4
        S_89_5 = 89,                             // $@5
        S_90_6 = 90,                             // $@6
        S_91_7 = 91,                             // $@7
        S_92_8 = 92,                             // $@8
        S_93_ap_names = 93,                      // ap-names
        S_94_ap_name = 94,                       // ap-name
        S_95_acc_spec = 95,                      // acc-spec
        S_properties = 96,                       // properties
        S_97_highlight_edges = 97,               // highlight-edges
        S_98_highlight_states = 98,              // highlight-states
        S_99_state_player = 99,                  // state-player
        S_100_header_spec = 100,                 // header-spec
        S_101_state_conj_2 = 101,                // state-conj-2
        S_102_init_state_conj_2 = 102,           // init-state-conj-2
        S_103_label_expr = 103,                  // label-expr
        S_104_acc_set = 104,                     // acc-set
        S_105_acceptance_cond = 105,             // acceptance-cond
        S_body = 106,                            // body
        S_107_state_num = 107,                   // state-num
        S_108_checked_state_num = 108,           // checked-state-num
        S_states = 109,                          // states
        S_state = 110,                           // state
        S_111_state_name = 111,                  // state-name
        S_label = 112,                           // label
        S_113_state_label_opt = 113,             // state-label_opt
        S_114_trans_label = 114,                 // trans-label
        S_115_acc_sig = 115,                     // acc-sig
        S_116_acc_sets = 116,                    // acc-sets
        S_117_state_acc_opt = 117,               // state-acc_opt
        S_118_trans_acc_opt = 118,               // trans-acc_opt
        S_119_labeled_edges = 119,               // labeled-edges
        S_120_some_labeled_edges = 120,          // some-labeled-edges
        S_121_incorrectly_unlabeled_edge = 121,  // incorrectly-unlabeled-edge
        S_122_labeled_edge = 122,                // labeled-edge
        S_123_state_conj_checked = 123,          // state-conj-checked
        S_124_unlabeled_edges = 124,             // unlabeled-edges
        S_125_unlabeled_edge = 125,              // unlabeled-edge
        S_126_incorrectly_labeled_edge = 126,    // incorrectly-labeled-edge
        S_dstar = 127,                           // dstar
        S_dstar_type = 128,                      // dstar_type
        S_dstar_header = 129,                    // dstar_header
        S_dstar_sizes = 130,                     // dstar_sizes
        S_dstar_state_id = 131,                  // dstar_state_id
        S_sign = 132,                            // sign
        S_dstar_accsigs = 133,                   // dstar_accsigs
        S_dstar_state_accsig = 134,              // dstar_state_accsig
        S_dstar_transitions = 135,               // dstar_transitions
        S_dstar_states = 136,                    // dstar_states
        S_pgamestart = 137,                      // pgamestart
        S_pgame = 138,                           // pgame
        S_pgame_nodes = 139,                     // pgame_nodes
        S_pgame_succs = 140,                     // pgame_succs
        S_pgame_node = 141,                      // pgame_node
        S_never = 142,                           // never
        S_143_9 = 143,                           // $@9
        S_144_nc_states = 144,                   // nc-states
        S_145_nc_one_ident = 145,                // nc-one-ident
        S_146_nc_ident_list = 146,               // nc-ident-list
        S_147_nc_transition_block = 147,         // nc-transition-block
        S_148_nc_state = 148,                    // nc-state
        S_149_nc_transitions = 149,              // nc-transitions
        S_150_nc_formula_or_ident = 150,         // nc-formula-or-ident
        S_151_nc_formula = 151,                  // nc-formula
        S_152_nc_opt_dest = 152,                 // nc-opt-dest
        S_153_nc_src_dest = 153,                 // nc-src-dest
        S_154_nc_transition = 154,               // nc-transition
        S_lbtt = 155,                            // lbtt
        S_156_lbtt_header_states = 156,          // lbtt-header-states
        S_157_lbtt_header = 157,                 // lbtt-header
        S_158_lbtt_body = 158,                   // lbtt-body
        S_159_lbtt_states = 159,                 // lbtt-states
        S_160_lbtt_state = 160,                  // lbtt-state
        S_161_lbtt_acc = 161,                    // lbtt-acc
        S_162_lbtt_guard = 162,                  // lbtt-guard
        S_163_lbtt_transitions = 163             // lbtt-transitions
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value (std::move (that.value))
        , location (std::move (that.location))
      {}
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);
      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    YY_MOVE_REF (location_type) l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    YY_RVREF (value_type) v,
                    YY_RVREF (location_type) l);

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        Base::clear ();
      }

      /// The user-facing name of this symbol.
      std::string name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {};

    /// Build a parser object.
    parser (void* scanner_yyarg, result_& res_yyarg, spot::location initial_loc_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if HOAYYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name (symbol_kind_type yysymbol);



    class context
    {
    public:
      context (const parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const short yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);

    /// For a symbol, its name in clear.
    static const char* const yytname_[];


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const unsigned char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const unsigned char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const unsigned char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if HOAYYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 271,     ///< Last index in yytable_.
      yynnts_ = 92,  ///< Number of nonterminal symbols.
      yyfinal_ = 29 ///< Termination state number.
    };


    // User arguments.
    void* scanner;
    result_& res;
    spot::location initial_loc;

  };


} // hoayy
#line 1202 "parseaut.hh"




#endif // !YY_HOAYY_PARSEAUT_HH_INCLUDED
