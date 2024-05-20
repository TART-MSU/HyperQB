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
 ** \file parsetl.hh
 ** Define the tlyy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_TLYY_PARSETL_HH_INCLUDED
# define YY_TLYY_PARSETL_HH_INCLUDED
// "%code requires" blocks.
#line 33 "parsetl.yy"

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



#line 207 "parsetl.hh"


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
#ifndef TLYYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define TLYYDEBUG 1
#  else
#   define TLYYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define TLYYDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined TLYYDEBUG */

namespace tlyy {
#line 350 "parsetl.hh"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef TLYYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define TLYYSTYPE in C++, use %define api.value.type"
# endif
    typedef TLYYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // sqbracketargs
      // gotoargs
      // starargs
      // fstarargs
      // equalargs
      // delayargs
      char dummy1[sizeof (minmax_t)];

      // atomprop
      // booleanatom
      // sere
      // bracedsere
      // parenthesedsubformula
      // boolformula
      // subformula
      // lbtformula
      char dummy2[sizeof (pnode)];

      // "(...) block"
      // "{...} block"
      // "{...}! block"
      // "atomic proposition"
      char dummy3[sizeof (std::string)];

      // "number for square bracket operator"
      // "SVA delay operator"
      // sqbkt_num
      char dummy4[sizeof (unsigned)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
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
        TLYYEMPTY = -2,
    TLYYEOF = 0,                   // "end of file"
    TLYYerror = 256,               // error
    TLYYUNDEF = 257,               // "invalid token"
    START_LTL = 258,               // "LTL start marker"
    START_LBT = 259,               // "LBT start marker"
    START_SERE = 260,              // "SERE start marker"
    START_BOOL = 261,              // "BOOLEAN start marker"
    PAR_OPEN = 262,                // "opening parenthesis"
    PAR_CLOSE = 263,               // "closing parenthesis"
    PAR_BLOCK = 264,               // "(...) block"
    BRA_BLOCK = 265,               // "{...} block"
    BRA_BANG_BLOCK = 266,          // "{...}! block"
    BRACE_OPEN = 267,              // "opening brace"
    BRACE_CLOSE = 268,             // "closing brace"
    BRACE_BANG_CLOSE = 269,        // "closing brace-bang"
    OP_OR = 270,                   // "or operator"
    OP_XOR = 271,                  // "xor operator"
    OP_AND = 272,                  // "and operator"
    OP_SHORT_AND = 273,            // "short and operator"
    OP_IMPLIES = 274,              // "implication operator"
    OP_EQUIV = 275,                // "equivalent operator"
    OP_U = 276,                    // "until operator"
    OP_R = 277,                    // "release operator"
    OP_W = 278,                    // "weak until operator"
    OP_M = 279,                    // "strong release operator"
    OP_F = 280,                    // "sometimes operator"
    OP_G = 281,                    // "always operator"
    OP_X = 282,                    // "next operator"
    OP_STRONG_X = 283,             // "strong next operator"
    OP_NOT = 284,                  // "not operator"
    OP_XREP = 285,                 // "X[.] operator"
    OP_FREP = 286,                 // "F[.] operator"
    OP_GREP = 287,                 // "G[.] operator"
    OP_STAR = 288,                 // "star operator"
    OP_BSTAR = 289,                // "bracket star operator"
    OP_BFSTAR = 290,               // "bracket fusion-star operator"
    OP_PLUS = 291,                 // "plus operator"
    OP_FPLUS = 292,                // "fusion-plus operator"
    OP_STAR_OPEN = 293,            // "opening bracket for star operator"
    OP_FSTAR_OPEN = 294,           // "opening bracket for fusion-star operator"
    OP_EQUAL_OPEN = 295,           // "opening bracket for equal operator"
    OP_GOTO_OPEN = 296,            // "opening bracket for goto operator"
    OP_SQBKT_CLOSE = 297,          // "closing bracket"
    OP_SQBKT_STRONG_CLOSE = 298,   // "closing !]"
    OP_SQBKT_NUM = 299,            // "number for square bracket operator"
    OP_UNBOUNDED = 300,            // "unbounded mark"
    OP_SQBKT_SEP = 301,            // "separator for square bracket operator"
    OP_UCONCAT = 302,              // "universal concat operator"
    OP_ECONCAT = 303,              // "existential concat operator"
    OP_UCONCAT_NONO = 304,         // "universal non-overlapping concat operator"
    OP_ECONCAT_NONO = 305,         // "existential non-overlapping concat operator"
    OP_FIRST_MATCH = 306,          // "first_match"
    ATOMIC_PROP = 307,             // "atomic proposition"
    OP_CONCAT = 308,               // "concat operator"
    OP_FUSION = 309,               // "fusion operator"
    CONST_TRUE = 310,              // "constant true"
    CONST_FALSE = 311,             // "constant false"
    END_OF_INPUT = 312,            // "end of formula"
    OP_POST_NEG = 313,             // "negative suffix"
    OP_POST_POS = 314,             // "positive suffix"
    OP_DELAY_N = 315,              // "SVA delay operator"
    OP_DELAY_OPEN = 316,           // "opening bracket for SVA delay operator"
    OP_DELAY_PLUS = 317,           // "##[+] operator"
    OP_DELAY_STAR = 318            // "##[*] operator"
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
        YYNTOKENS = 80, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_START_LTL = 3,                         // "LTL start marker"
        S_START_LBT = 4,                         // "LBT start marker"
        S_START_SERE = 5,                        // "SERE start marker"
        S_START_BOOL = 6,                        // "BOOLEAN start marker"
        S_PAR_OPEN = 7,                          // "opening parenthesis"
        S_PAR_CLOSE = 8,                         // "closing parenthesis"
        S_PAR_BLOCK = 9,                         // "(...) block"
        S_BRA_BLOCK = 10,                        // "{...} block"
        S_BRA_BANG_BLOCK = 11,                   // "{...}! block"
        S_BRACE_OPEN = 12,                       // "opening brace"
        S_BRACE_CLOSE = 13,                      // "closing brace"
        S_BRACE_BANG_CLOSE = 14,                 // "closing brace-bang"
        S_OP_OR = 15,                            // "or operator"
        S_OP_XOR = 16,                           // "xor operator"
        S_OP_AND = 17,                           // "and operator"
        S_OP_SHORT_AND = 18,                     // "short and operator"
        S_OP_IMPLIES = 19,                       // "implication operator"
        S_OP_EQUIV = 20,                         // "equivalent operator"
        S_OP_U = 21,                             // "until operator"
        S_OP_R = 22,                             // "release operator"
        S_OP_W = 23,                             // "weak until operator"
        S_OP_M = 24,                             // "strong release operator"
        S_OP_F = 25,                             // "sometimes operator"
        S_OP_G = 26,                             // "always operator"
        S_OP_X = 27,                             // "next operator"
        S_OP_STRONG_X = 28,                      // "strong next operator"
        S_OP_NOT = 29,                           // "not operator"
        S_OP_XREP = 30,                          // "X[.] operator"
        S_OP_FREP = 31,                          // "F[.] operator"
        S_OP_GREP = 32,                          // "G[.] operator"
        S_OP_STAR = 33,                          // "star operator"
        S_OP_BSTAR = 34,                         // "bracket star operator"
        S_OP_BFSTAR = 35,                        // "bracket fusion-star operator"
        S_OP_PLUS = 36,                          // "plus operator"
        S_OP_FPLUS = 37,                         // "fusion-plus operator"
        S_OP_STAR_OPEN = 38,                     // "opening bracket for star operator"
        S_OP_FSTAR_OPEN = 39,                    // "opening bracket for fusion-star operator"
        S_OP_EQUAL_OPEN = 40,                    // "opening bracket for equal operator"
        S_OP_GOTO_OPEN = 41,                     // "opening bracket for goto operator"
        S_OP_SQBKT_CLOSE = 42,                   // "closing bracket"
        S_OP_SQBKT_STRONG_CLOSE = 43,            // "closing !]"
        S_OP_SQBKT_NUM = 44,                     // "number for square bracket operator"
        S_OP_UNBOUNDED = 45,                     // "unbounded mark"
        S_OP_SQBKT_SEP = 46,                     // "separator for square bracket operator"
        S_OP_UCONCAT = 47,                       // "universal concat operator"
        S_OP_ECONCAT = 48,                       // "existential concat operator"
        S_OP_UCONCAT_NONO = 49,                  // "universal non-overlapping concat operator"
        S_OP_ECONCAT_NONO = 50,                  // "existential non-overlapping concat operator"
        S_OP_FIRST_MATCH = 51,                   // "first_match"
        S_ATOMIC_PROP = 52,                      // "atomic proposition"
        S_OP_CONCAT = 53,                        // "concat operator"
        S_OP_FUSION = 54,                        // "fusion operator"
        S_CONST_TRUE = 55,                       // "constant true"
        S_CONST_FALSE = 56,                      // "constant false"
        S_END_OF_INPUT = 57,                     // "end of formula"
        S_OP_POST_NEG = 58,                      // "negative suffix"
        S_OP_POST_POS = 59,                      // "positive suffix"
        S_OP_DELAY_N = 60,                       // "SVA delay operator"
        S_OP_DELAY_OPEN = 61,                    // "opening bracket for SVA delay operator"
        S_OP_DELAY_PLUS = 62,                    // "##[+] operator"
        S_OP_DELAY_STAR = 63,                    // "##[*] operator"
        S_64_ = 64,                              // '!'
        S_65_ = 65,                              // '&'
        S_66_ = 66,                              // '|'
        S_67_ = 67,                              // '^'
        S_68_i_ = 68,                            // 'i'
        S_69_e_ = 69,                            // 'e'
        S_70_X_ = 70,                            // 'X'
        S_71_F_ = 71,                            // 'F'
        S_72_G_ = 72,                            // 'G'
        S_73_U_ = 73,                            // 'U'
        S_74_V_ = 74,                            // 'V'
        S_75_R_ = 75,                            // 'R'
        S_76_W_ = 76,                            // 'W'
        S_77_M_ = 77,                            // 'M'
        S_78_t_ = 78,                            // 't'
        S_79_f_ = 79,                            // 'f'
        S_YYACCEPT = 80,                         // $accept
        S_result = 81,                           // result
        S_emptyinput = 82,                       // emptyinput
        S_enderror = 83,                         // enderror
        S_OP_SQBKT_SEP_unbounded = 84,           // OP_SQBKT_SEP_unbounded
        S_OP_SQBKT_SEP_opt = 85,                 // OP_SQBKT_SEP_opt
        S_error_opt = 86,                        // error_opt
        S_sqbkt_num = 87,                        // sqbkt_num
        S_sqbracketargs = 88,                    // sqbracketargs
        S_gotoargs = 89,                         // gotoargs
        S_kleen_star = 90,                       // kleen_star
        S_starargs = 91,                         // starargs
        S_fstarargs = 92,                        // fstarargs
        S_equalargs = 93,                        // equalargs
        S_delayargs = 94,                        // delayargs
        S_atomprop = 95,                         // atomprop
        S_booleanatom = 96,                      // booleanatom
        S_sere = 97,                             // sere
        S_bracedsere = 98,                       // bracedsere
        S_parenthesedsubformula = 99,            // parenthesedsubformula
        S_boolformula = 100,                     // boolformula
        S_subformula = 101,                      // subformula
        S_lbtformula = 102                       // lbtformula
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
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_sqbracketargs: // sqbracketargs
      case symbol_kind::S_gotoargs: // gotoargs
      case symbol_kind::S_starargs: // starargs
      case symbol_kind::S_fstarargs: // fstarargs
      case symbol_kind::S_equalargs: // equalargs
      case symbol_kind::S_delayargs: // delayargs
        value.move< minmax_t > (std::move (that.value));
        break;

      case symbol_kind::S_atomprop: // atomprop
      case symbol_kind::S_booleanatom: // booleanatom
      case symbol_kind::S_sere: // sere
      case symbol_kind::S_bracedsere: // bracedsere
      case symbol_kind::S_parenthesedsubformula: // parenthesedsubformula
      case symbol_kind::S_boolformula: // boolformula
      case symbol_kind::S_subformula: // subformula
      case symbol_kind::S_lbtformula: // lbtformula
        value.move< pnode > (std::move (that.value));
        break;

      case symbol_kind::S_PAR_BLOCK: // "(...) block"
      case symbol_kind::S_BRA_BLOCK: // "{...} block"
      case symbol_kind::S_BRA_BANG_BLOCK: // "{...}! block"
      case symbol_kind::S_ATOMIC_PROP: // "atomic proposition"
        value.move< std::string > (std::move (that.value));
        break;

      case symbol_kind::S_OP_SQBKT_NUM: // "number for square bracket operator"
      case symbol_kind::S_OP_DELAY_N: // "SVA delay operator"
      case symbol_kind::S_sqbkt_num: // sqbkt_num
        value.move< unsigned > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, minmax_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const minmax_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, pnode&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const pnode& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, unsigned&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const unsigned& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_sqbracketargs: // sqbracketargs
      case symbol_kind::S_gotoargs: // gotoargs
      case symbol_kind::S_starargs: // starargs
      case symbol_kind::S_fstarargs: // fstarargs
      case symbol_kind::S_equalargs: // equalargs
      case symbol_kind::S_delayargs: // delayargs
        value.template destroy< minmax_t > ();
        break;

      case symbol_kind::S_atomprop: // atomprop
      case symbol_kind::S_booleanatom: // booleanatom
      case symbol_kind::S_sere: // sere
      case symbol_kind::S_bracedsere: // bracedsere
      case symbol_kind::S_parenthesedsubformula: // parenthesedsubformula
      case symbol_kind::S_boolformula: // boolformula
      case symbol_kind::S_subformula: // subformula
      case symbol_kind::S_lbtformula: // lbtformula
        value.template destroy< pnode > ();
        break;

      case symbol_kind::S_PAR_BLOCK: // "(...) block"
      case symbol_kind::S_BRA_BLOCK: // "{...} block"
      case symbol_kind::S_BRA_BANG_BLOCK: // "{...}! block"
      case symbol_kind::S_ATOMIC_PROP: // "atomic proposition"
        value.template destroy< std::string > ();
        break;

      case symbol_kind::S_OP_SQBKT_NUM: // "number for square bracket operator"
      case symbol_kind::S_OP_DELAY_N: // "SVA delay operator"
      case symbol_kind::S_sqbkt_num: // sqbkt_num
        value.template destroy< unsigned > ();
        break;

      default:
        break;
    }

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
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, unsigned v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const unsigned& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    parser (spot::parse_error_list &error_list_yyarg, spot::environment &parse_environment_yyarg, spot::formula &result_yyarg);
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

#if TLYYDEBUG
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

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TLYYEOF (location_type l)
      {
        return symbol_type (token::TLYYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_TLYYEOF (const location_type& l)
      {
        return symbol_type (token::TLYYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TLYYerror (location_type l)
      {
        return symbol_type (token::TLYYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_TLYYerror (const location_type& l)
      {
        return symbol_type (token::TLYYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TLYYUNDEF (location_type l)
      {
        return symbol_type (token::TLYYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_TLYYUNDEF (const location_type& l)
      {
        return symbol_type (token::TLYYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_START_LTL (location_type l)
      {
        return symbol_type (token::START_LTL, std::move (l));
      }
#else
      static
      symbol_type
      make_START_LTL (const location_type& l)
      {
        return symbol_type (token::START_LTL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_START_LBT (location_type l)
      {
        return symbol_type (token::START_LBT, std::move (l));
      }
#else
      static
      symbol_type
      make_START_LBT (const location_type& l)
      {
        return symbol_type (token::START_LBT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_START_SERE (location_type l)
      {
        return symbol_type (token::START_SERE, std::move (l));
      }
#else
      static
      symbol_type
      make_START_SERE (const location_type& l)
      {
        return symbol_type (token::START_SERE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_START_BOOL (location_type l)
      {
        return symbol_type (token::START_BOOL, std::move (l));
      }
#else
      static
      symbol_type
      make_START_BOOL (const location_type& l)
      {
        return symbol_type (token::START_BOOL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PAR_OPEN (location_type l)
      {
        return symbol_type (token::PAR_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_PAR_OPEN (const location_type& l)
      {
        return symbol_type (token::PAR_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PAR_CLOSE (location_type l)
      {
        return symbol_type (token::PAR_CLOSE, std::move (l));
      }
#else
      static
      symbol_type
      make_PAR_CLOSE (const location_type& l)
      {
        return symbol_type (token::PAR_CLOSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PAR_BLOCK (std::string v, location_type l)
      {
        return symbol_type (token::PAR_BLOCK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_PAR_BLOCK (const std::string& v, const location_type& l)
      {
        return symbol_type (token::PAR_BLOCK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BRA_BLOCK (std::string v, location_type l)
      {
        return symbol_type (token::BRA_BLOCK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BRA_BLOCK (const std::string& v, const location_type& l)
      {
        return symbol_type (token::BRA_BLOCK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BRA_BANG_BLOCK (std::string v, location_type l)
      {
        return symbol_type (token::BRA_BANG_BLOCK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BRA_BANG_BLOCK (const std::string& v, const location_type& l)
      {
        return symbol_type (token::BRA_BANG_BLOCK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BRACE_OPEN (location_type l)
      {
        return symbol_type (token::BRACE_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_BRACE_OPEN (const location_type& l)
      {
        return symbol_type (token::BRACE_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BRACE_CLOSE (location_type l)
      {
        return symbol_type (token::BRACE_CLOSE, std::move (l));
      }
#else
      static
      symbol_type
      make_BRACE_CLOSE (const location_type& l)
      {
        return symbol_type (token::BRACE_CLOSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BRACE_BANG_CLOSE (location_type l)
      {
        return symbol_type (token::BRACE_BANG_CLOSE, std::move (l));
      }
#else
      static
      symbol_type
      make_BRACE_BANG_CLOSE (const location_type& l)
      {
        return symbol_type (token::BRACE_BANG_CLOSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_OR (location_type l)
      {
        return symbol_type (token::OP_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_OR (const location_type& l)
      {
        return symbol_type (token::OP_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_XOR (location_type l)
      {
        return symbol_type (token::OP_XOR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_XOR (const location_type& l)
      {
        return symbol_type (token::OP_XOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_AND (location_type l)
      {
        return symbol_type (token::OP_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_AND (const location_type& l)
      {
        return symbol_type (token::OP_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SHORT_AND (location_type l)
      {
        return symbol_type (token::OP_SHORT_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SHORT_AND (const location_type& l)
      {
        return symbol_type (token::OP_SHORT_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_IMPLIES (location_type l)
      {
        return symbol_type (token::OP_IMPLIES, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_IMPLIES (const location_type& l)
      {
        return symbol_type (token::OP_IMPLIES, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_EQUIV (location_type l)
      {
        return symbol_type (token::OP_EQUIV, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_EQUIV (const location_type& l)
      {
        return symbol_type (token::OP_EQUIV, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_U (location_type l)
      {
        return symbol_type (token::OP_U, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_U (const location_type& l)
      {
        return symbol_type (token::OP_U, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_R (location_type l)
      {
        return symbol_type (token::OP_R, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_R (const location_type& l)
      {
        return symbol_type (token::OP_R, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_W (location_type l)
      {
        return symbol_type (token::OP_W, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_W (const location_type& l)
      {
        return symbol_type (token::OP_W, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_M (location_type l)
      {
        return symbol_type (token::OP_M, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_M (const location_type& l)
      {
        return symbol_type (token::OP_M, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_F (location_type l)
      {
        return symbol_type (token::OP_F, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_F (const location_type& l)
      {
        return symbol_type (token::OP_F, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_G (location_type l)
      {
        return symbol_type (token::OP_G, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_G (const location_type& l)
      {
        return symbol_type (token::OP_G, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_X (location_type l)
      {
        return symbol_type (token::OP_X, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_X (const location_type& l)
      {
        return symbol_type (token::OP_X, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_STRONG_X (location_type l)
      {
        return symbol_type (token::OP_STRONG_X, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_STRONG_X (const location_type& l)
      {
        return symbol_type (token::OP_STRONG_X, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_NOT (location_type l)
      {
        return symbol_type (token::OP_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_NOT (const location_type& l)
      {
        return symbol_type (token::OP_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_XREP (location_type l)
      {
        return symbol_type (token::OP_XREP, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_XREP (const location_type& l)
      {
        return symbol_type (token::OP_XREP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_FREP (location_type l)
      {
        return symbol_type (token::OP_FREP, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_FREP (const location_type& l)
      {
        return symbol_type (token::OP_FREP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_GREP (location_type l)
      {
        return symbol_type (token::OP_GREP, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_GREP (const location_type& l)
      {
        return symbol_type (token::OP_GREP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_STAR (location_type l)
      {
        return symbol_type (token::OP_STAR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_STAR (const location_type& l)
      {
        return symbol_type (token::OP_STAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_BSTAR (location_type l)
      {
        return symbol_type (token::OP_BSTAR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_BSTAR (const location_type& l)
      {
        return symbol_type (token::OP_BSTAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_BFSTAR (location_type l)
      {
        return symbol_type (token::OP_BFSTAR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_BFSTAR (const location_type& l)
      {
        return symbol_type (token::OP_BFSTAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_PLUS (location_type l)
      {
        return symbol_type (token::OP_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_PLUS (const location_type& l)
      {
        return symbol_type (token::OP_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_FPLUS (location_type l)
      {
        return symbol_type (token::OP_FPLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_FPLUS (const location_type& l)
      {
        return symbol_type (token::OP_FPLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_STAR_OPEN (location_type l)
      {
        return symbol_type (token::OP_STAR_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_STAR_OPEN (const location_type& l)
      {
        return symbol_type (token::OP_STAR_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_FSTAR_OPEN (location_type l)
      {
        return symbol_type (token::OP_FSTAR_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_FSTAR_OPEN (const location_type& l)
      {
        return symbol_type (token::OP_FSTAR_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_EQUAL_OPEN (location_type l)
      {
        return symbol_type (token::OP_EQUAL_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_EQUAL_OPEN (const location_type& l)
      {
        return symbol_type (token::OP_EQUAL_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_GOTO_OPEN (location_type l)
      {
        return symbol_type (token::OP_GOTO_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_GOTO_OPEN (const location_type& l)
      {
        return symbol_type (token::OP_GOTO_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SQBKT_CLOSE (location_type l)
      {
        return symbol_type (token::OP_SQBKT_CLOSE, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SQBKT_CLOSE (const location_type& l)
      {
        return symbol_type (token::OP_SQBKT_CLOSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SQBKT_STRONG_CLOSE (location_type l)
      {
        return symbol_type (token::OP_SQBKT_STRONG_CLOSE, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SQBKT_STRONG_CLOSE (const location_type& l)
      {
        return symbol_type (token::OP_SQBKT_STRONG_CLOSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SQBKT_NUM (unsigned v, location_type l)
      {
        return symbol_type (token::OP_SQBKT_NUM, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SQBKT_NUM (const unsigned& v, const location_type& l)
      {
        return symbol_type (token::OP_SQBKT_NUM, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_UNBOUNDED (location_type l)
      {
        return symbol_type (token::OP_UNBOUNDED, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_UNBOUNDED (const location_type& l)
      {
        return symbol_type (token::OP_UNBOUNDED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SQBKT_SEP (location_type l)
      {
        return symbol_type (token::OP_SQBKT_SEP, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SQBKT_SEP (const location_type& l)
      {
        return symbol_type (token::OP_SQBKT_SEP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_UCONCAT (location_type l)
      {
        return symbol_type (token::OP_UCONCAT, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_UCONCAT (const location_type& l)
      {
        return symbol_type (token::OP_UCONCAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_ECONCAT (location_type l)
      {
        return symbol_type (token::OP_ECONCAT, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_ECONCAT (const location_type& l)
      {
        return symbol_type (token::OP_ECONCAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_UCONCAT_NONO (location_type l)
      {
        return symbol_type (token::OP_UCONCAT_NONO, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_UCONCAT_NONO (const location_type& l)
      {
        return symbol_type (token::OP_UCONCAT_NONO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_ECONCAT_NONO (location_type l)
      {
        return symbol_type (token::OP_ECONCAT_NONO, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_ECONCAT_NONO (const location_type& l)
      {
        return symbol_type (token::OP_ECONCAT_NONO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_FIRST_MATCH (location_type l)
      {
        return symbol_type (token::OP_FIRST_MATCH, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_FIRST_MATCH (const location_type& l)
      {
        return symbol_type (token::OP_FIRST_MATCH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ATOMIC_PROP (std::string v, location_type l)
      {
        return symbol_type (token::ATOMIC_PROP, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ATOMIC_PROP (const std::string& v, const location_type& l)
      {
        return symbol_type (token::ATOMIC_PROP, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_CONCAT (location_type l)
      {
        return symbol_type (token::OP_CONCAT, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_CONCAT (const location_type& l)
      {
        return symbol_type (token::OP_CONCAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_FUSION (location_type l)
      {
        return symbol_type (token::OP_FUSION, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_FUSION (const location_type& l)
      {
        return symbol_type (token::OP_FUSION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST_TRUE (location_type l)
      {
        return symbol_type (token::CONST_TRUE, std::move (l));
      }
#else
      static
      symbol_type
      make_CONST_TRUE (const location_type& l)
      {
        return symbol_type (token::CONST_TRUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST_FALSE (location_type l)
      {
        return symbol_type (token::CONST_FALSE, std::move (l));
      }
#else
      static
      symbol_type
      make_CONST_FALSE (const location_type& l)
      {
        return symbol_type (token::CONST_FALSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_END_OF_INPUT (location_type l)
      {
        return symbol_type (token::END_OF_INPUT, std::move (l));
      }
#else
      static
      symbol_type
      make_END_OF_INPUT (const location_type& l)
      {
        return symbol_type (token::END_OF_INPUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_POST_NEG (location_type l)
      {
        return symbol_type (token::OP_POST_NEG, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_POST_NEG (const location_type& l)
      {
        return symbol_type (token::OP_POST_NEG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_POST_POS (location_type l)
      {
        return symbol_type (token::OP_POST_POS, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_POST_POS (const location_type& l)
      {
        return symbol_type (token::OP_POST_POS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_DELAY_N (unsigned v, location_type l)
      {
        return symbol_type (token::OP_DELAY_N, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OP_DELAY_N (const unsigned& v, const location_type& l)
      {
        return symbol_type (token::OP_DELAY_N, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_DELAY_OPEN (location_type l)
      {
        return symbol_type (token::OP_DELAY_OPEN, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_DELAY_OPEN (const location_type& l)
      {
        return symbol_type (token::OP_DELAY_OPEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_DELAY_PLUS (location_type l)
      {
        return symbol_type (token::OP_DELAY_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_DELAY_PLUS (const location_type& l)
      {
        return symbol_type (token::OP_DELAY_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_DELAY_STAR (location_type l)
      {
        return symbol_type (token::OP_DELAY_STAR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_DELAY_STAR (const location_type& l)
      {
        return symbol_type (token::OP_DELAY_STAR, l);
      }
#endif


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

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

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
    static const unsigned char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if TLYYDEBUG
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
      yylast_ = 2385,     ///< Last index in yytable_.
      yynnts_ = 23,  ///< Number of nonterminal symbols.
      yyfinal_ = 79 ///< Termination state number.
    };


    // User arguments.
    spot::parse_error_list &error_list;
    spot::environment &parse_environment;
    spot::formula &result;

  };


} // tlyy
#line 2475 "parsetl.hh"




#endif // !YY_TLYY_PARSETL_HH_INCLUDED
