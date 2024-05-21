/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 8 "parser.y"

#include <string.h>
#include <stdarg.h>
#include <fstream>
#include <getopt.h>
#define IMPLEMENTSLIST /* Special for list template handling */
#include "slist.h"
#include "hashtbl.h"
#include "parser_.h"

   using namespace std;

   /* Definitions for storing and caching of identifiers */
#define inputTag  0
#define exprTag   1

   struct nodeData
   {
      nodeData(const nodeData &d) { tag=d.tag; name=sdup(d.name); val=d.val; }
      nodeData(int t, char *n, bdd v) { tag=t; name=n; val=v; }
      ~nodeData(void) { delete[] name; }
      int tag;
      char *name;
      bdd val;
   };

   typedef SList<nodeData> nodeLst;
   nodeLst inputs;
   hashTable names;

      /* Other */
   int linenum;

   bddgbchandler gbcHandler = bdd_default_gbchandler;

      /* Prototypes */
void actInit(token *nodes, token *cache);
void actInputs(void);
void actAddInput(token *id);
void actAssign(token *id, token *expr);
void actOpr2(token *res, token *left, token *right, int opr);
void actNot(token *res, token *right);
void actId(token *res, token *id);
void actConst(token *res, int);
void actSize(token *id);
void actDot(token *fname, token *id);
void actAutoreorder(token *times, token *method);
void actCache(void);
void actTautology(token *id);
void actExist(token *res, token *var, token *expr);
void actForall(token *res, token *var, token *expr);
void actQuantVar2(token *res, token *id, token *list);
void actQuantVar1(token *res, token *id);
void actPrint(token *id);


#line 128 "parser.c"

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

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_id = 258,                    /* T_id  */
    T_str = 259,                   /* T_str  */
    T_intval = 260,                /* T_intval  */
    T_true = 261,                  /* T_true  */
    T_false = 262,                 /* T_false  */
    T_initial = 263,               /* T_initial  */
    T_inputs = 264,                /* T_inputs  */
    T_actions = 265,               /* T_actions  */
    T_size = 266,                  /* T_size  */
    T_dumpdot = 267,               /* T_dumpdot  */
    T_autoreorder = 268,           /* T_autoreorder  */
    T_reorder = 269,               /* T_reorder  */
    T_win2 = 270,                  /* T_win2  */
    T_win2ite = 271,               /* T_win2ite  */
    T_sift = 272,                  /* T_sift  */
    T_siftite = 273,               /* T_siftite  */
    T_none = 274,                  /* T_none  */
    T_cache = 275,                 /* T_cache  */
    T_tautology = 276,             /* T_tautology  */
    T_print = 277,                 /* T_print  */
    T_lpar = 278,                  /* T_lpar  */
    T_rpar = 279,                  /* T_rpar  */
    T_equal = 280,                 /* T_equal  */
    T_semi = 281,                  /* T_semi  */
    T_dot = 282,                   /* T_dot  */
    T_exist = 283,                 /* T_exist  */
    T_forall = 284,                /* T_forall  */
    T_biimp = 285,                 /* T_biimp  */
    T_imp = 286,                   /* T_imp  */
    T_or = 287,                    /* T_or  */
    T_nor = 288,                   /* T_nor  */
    T_xor = 289,                   /* T_xor  */
    T_nand = 290,                  /* T_nand  */
    T_and = 291,                   /* T_and  */
    T_not = 292                    /* T_not  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define T_id 258
#define T_str 259
#define T_intval 260
#define T_true 261
#define T_false 262
#define T_initial 263
#define T_inputs 264
#define T_actions 265
#define T_size 266
#define T_dumpdot 267
#define T_autoreorder 268
#define T_reorder 269
#define T_win2 270
#define T_win2ite 271
#define T_sift 272
#define T_siftite 273
#define T_none 274
#define T_cache 275
#define T_tautology 276
#define T_print 277
#define T_lpar 278
#define T_rpar 279
#define T_equal 280
#define T_semi 281
#define T_dot 282
#define T_exist 283
#define T_forall 284
#define T_biimp 285
#define T_imp 286
#define T_or 287
#define T_nor 288
#define T_xor 289
#define T_nand 290
#define T_and 291
#define T_not 292

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_id = 3,                       /* T_id  */
  YYSYMBOL_T_str = 4,                      /* T_str  */
  YYSYMBOL_T_intval = 5,                   /* T_intval  */
  YYSYMBOL_T_true = 6,                     /* T_true  */
  YYSYMBOL_T_false = 7,                    /* T_false  */
  YYSYMBOL_T_initial = 8,                  /* T_initial  */
  YYSYMBOL_T_inputs = 9,                   /* T_inputs  */
  YYSYMBOL_T_actions = 10,                 /* T_actions  */
  YYSYMBOL_T_size = 11,                    /* T_size  */
  YYSYMBOL_T_dumpdot = 12,                 /* T_dumpdot  */
  YYSYMBOL_T_autoreorder = 13,             /* T_autoreorder  */
  YYSYMBOL_T_reorder = 14,                 /* T_reorder  */
  YYSYMBOL_T_win2 = 15,                    /* T_win2  */
  YYSYMBOL_T_win2ite = 16,                 /* T_win2ite  */
  YYSYMBOL_T_sift = 17,                    /* T_sift  */
  YYSYMBOL_T_siftite = 18,                 /* T_siftite  */
  YYSYMBOL_T_none = 19,                    /* T_none  */
  YYSYMBOL_T_cache = 20,                   /* T_cache  */
  YYSYMBOL_T_tautology = 21,               /* T_tautology  */
  YYSYMBOL_T_print = 22,                   /* T_print  */
  YYSYMBOL_T_lpar = 23,                    /* T_lpar  */
  YYSYMBOL_T_rpar = 24,                    /* T_rpar  */
  YYSYMBOL_T_equal = 25,                   /* T_equal  */
  YYSYMBOL_T_semi = 26,                    /* T_semi  */
  YYSYMBOL_T_dot = 27,                     /* T_dot  */
  YYSYMBOL_T_exist = 28,                   /* T_exist  */
  YYSYMBOL_T_forall = 29,                  /* T_forall  */
  YYSYMBOL_T_biimp = 30,                   /* T_biimp  */
  YYSYMBOL_T_imp = 31,                     /* T_imp  */
  YYSYMBOL_T_or = 32,                      /* T_or  */
  YYSYMBOL_T_nor = 33,                     /* T_nor  */
  YYSYMBOL_T_xor = 34,                     /* T_xor  */
  YYSYMBOL_T_nand = 35,                    /* T_nand  */
  YYSYMBOL_T_and = 36,                     /* T_and  */
  YYSYMBOL_T_not = 37,                     /* T_not  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_calc = 39,                      /* calc  */
  YYSYMBOL_initial = 40,                   /* initial  */
  YYSYMBOL_inputs = 41,                    /* inputs  */
  YYSYMBOL_inputSeq = 42,                  /* inputSeq  */
  YYSYMBOL_actions = 43,                   /* actions  */
  YYSYMBOL_actionSeq = 44,                 /* actionSeq  */
  YYSYMBOL_action = 45,                    /* action  */
  YYSYMBOL_assign = 46,                    /* assign  */
  YYSYMBOL_expr = 47,                      /* expr  */
  YYSYMBOL_quantifier = 48,                /* quantifier  */
  YYSYMBOL_varlist = 49,                   /* varlist  */
  YYSYMBOL_size = 50,                      /* size  */
  YYSYMBOL_dot = 51,                       /* dot  */
  YYSYMBOL_reorder = 52,                   /* reorder  */
  YYSYMBOL_method = 53,                    /* method  */
  YYSYMBOL_cache = 54,                     /* cache  */
  YYSYMBOL_tautology = 55,                 /* tautology  */
  YYSYMBOL_print = 56                      /* print  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
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


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   88

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  46
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  84

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   292


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    96,    96,   102,   106,   110,   111,   118,   122,   123,
     127,   128,   129,   130,   131,   132,   133,   137,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   157,   158,   162,   163,   168,   172,   176,   177,   181,
     182,   183,   184,   185,   189,   193,   197
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_id", "T_str",
  "T_intval", "T_true", "T_false", "T_initial", "T_inputs", "T_actions",
  "T_size", "T_dumpdot", "T_autoreorder", "T_reorder", "T_win2",
  "T_win2ite", "T_sift", "T_siftite", "T_none", "T_cache", "T_tautology",
  "T_print", "T_lpar", "T_rpar", "T_equal", "T_semi", "T_dot", "T_exist",
  "T_forall", "T_biimp", "T_imp", "T_or", "T_nor", "T_xor", "T_nand",
  "T_and", "T_not", "$accept", "calc", "initial", "inputs", "inputSeq",
  "actions", "actionSeq", "action", "assign", "expr", "quantifier",
  "varlist", "size", "dot", "reorder", "method", "cache", "tautology",
  "print", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-38)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -6,     7,    15,    14,    46,   -38,    49,    44,    50,   -38,
      -2,    36,   -38,   -38,   -38,   -38,    30,    74,    75,    73,
      51,   -38,    77,    78,    36,    56,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,    -3,   -38,    80,    51,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,    58,   -38,   -38,   -38,   -38,
      -3,    82,    82,    -3,    10,   -38,   -38,   -38,   -38,    29,
     -38,     8,    11,   -38,    -3,    -3,    -3,    -3,    -3,    -3,
      -3,   -38,   -38,    -3,    -3,   -26,    39,   -17,   -17,   -14,
     -38,   -38,    10,    10
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     1,     0,     0,     0,     6,
       0,     0,     2,     3,     5,     4,     0,     0,     0,     0,
       0,    44,     0,     0,     7,     0,    10,    11,    12,    13,
      14,    15,    16,     0,    35,     0,     0,    39,    40,    41,
      42,    43,    37,    45,    46,     0,     9,    27,    28,    29,
       0,     0,     0,     0,    17,    30,    36,    38,     8,     0,
      34,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    26,    33,     0,     0,    24,    23,    21,    22,    20,
      19,    18,    31,    32
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -38,   -38,   -38,   -38,   -38,   -38,   -38,    62,   -38,   -37,
     -38,    35,   -38,   -38,   -38,    52,   -38,   -38,   -38
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,     7,    10,    12,    24,    25,    26,    54,
      55,    61,    27,    28,    29,    42,    30,    31,    32
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      47,    14,     1,    48,    49,    65,    66,    67,    68,    69,
      70,    72,     4,    59,    72,     5,    63,    68,    69,    70,
      50,    69,    70,     6,    15,    51,    52,    75,    76,    77,
      78,    79,    80,    81,    53,    73,    82,    83,    74,    16,
      64,    65,    66,    67,    68,    69,    70,    17,    18,    19,
      20,     8,     9,    71,    11,    33,    21,    22,    23,    64,
      65,    66,    67,    68,    69,    70,    37,    38,    39,    40,
      41,    66,    67,    68,    69,    70,    13,    34,    36,    35,
      43,    44,    46,    56,    58,    60,    45,    62,    57
};

static const yytype_int8 yycheck[] =
{
       3,     3,     8,     6,     7,    31,    32,    33,    34,    35,
      36,     3,     5,    50,     3,     0,    53,    34,    35,    36,
      23,    35,    36,     9,    26,    28,    29,    64,    65,    66,
      67,    68,    69,    70,    37,    27,    73,    74,    27,     3,
      30,    31,    32,    33,    34,    35,    36,    11,    12,    13,
      14,     5,     3,    24,    10,    25,    20,    21,    22,    30,
      31,    32,    33,    34,    35,    36,    15,    16,    17,    18,
      19,    32,    33,    34,    35,    36,    26,     3,     5,     4,
       3,     3,    26,     3,    26,     3,    24,    52,    36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,    39,    40,     5,     0,     9,    41,     5,     3,
      42,    10,    43,    26,     3,    26,     3,    11,    12,    13,
      14,    20,    21,    22,    44,    45,    46,    50,    51,    52,
      54,    55,    56,    25,     3,     4,     5,    15,    16,    17,
      18,    19,    53,     3,     3,    45,    26,     3,     6,     7,
      23,    28,    29,    37,    47,    48,     3,    53,    26,    47,
       3,    49,    49,    47,    30,    31,    32,    33,    34,    35,
      36,    24,     3,    27,    27,    47,    47,    47,    47,    47,
      47,    47,    47,    47
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    40,    41,    42,    42,    43,    44,    44,
      45,    45,    45,    45,    45,    45,    45,    46,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    48,    48,    49,    49,    50,    51,    52,    52,    53,
      53,    53,    53,    53,    54,    55,    56
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     4,     3,     2,     1,     2,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     1,     1,     1,
       1,     4,     4,     2,     1,     2,     3,     2,     3,     1,
       1,     1,     1,     1,     1,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* initial: T_initial T_intval T_intval T_semi  */
#line 102 "parser.y"
                                      { actInit(&yyvsp[-2],&yyvsp[-1]); }
#line 1337 "parser.c"
    break;

  case 4: /* inputs: T_inputs inputSeq T_semi  */
#line 106 "parser.y"
                            { actInputs(); }
#line 1343 "parser.c"
    break;

  case 5: /* inputSeq: inputSeq T_id  */
#line 110 "parser.y"
                 { actAddInput(&yyvsp[0]); }
#line 1349 "parser.c"
    break;

  case 6: /* inputSeq: T_id  */
#line 111 "parser.y"
                 { actAddInput(&yyvsp[0]); }
#line 1355 "parser.c"
    break;

  case 17: /* assign: T_id T_equal expr  */
#line 137 "parser.y"
                     { actAssign(&yyvsp[-2],&yyvsp[0]); }
#line 1361 "parser.c"
    break;

  case 18: /* expr: expr T_and expr  */
#line 141 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_and); }
#line 1367 "parser.c"
    break;

  case 19: /* expr: expr T_nand expr  */
#line 142 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_nand); }
#line 1373 "parser.c"
    break;

  case 20: /* expr: expr T_xor expr  */
#line 143 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_xor); }
#line 1379 "parser.c"
    break;

  case 21: /* expr: expr T_or expr  */
#line 144 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_or); }
#line 1385 "parser.c"
    break;

  case 22: /* expr: expr T_nor expr  */
#line 145 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_nor); }
#line 1391 "parser.c"
    break;

  case 23: /* expr: expr T_imp expr  */
#line 146 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_imp); }
#line 1397 "parser.c"
    break;

  case 24: /* expr: expr T_biimp expr  */
#line 147 "parser.y"
                        { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_biimp); }
#line 1403 "parser.c"
    break;

  case 25: /* expr: T_not expr  */
#line 148 "parser.y"
                        { actNot(&yyval,&yyvsp[0]); }
#line 1409 "parser.c"
    break;

  case 26: /* expr: T_lpar expr T_rpar  */
#line 149 "parser.y"
                        { yyval.bval = yyvsp[-1].bval; }
#line 1415 "parser.c"
    break;

  case 27: /* expr: T_id  */
#line 150 "parser.y"
                        { actId(&yyval,&yyvsp[0]); }
#line 1421 "parser.c"
    break;

  case 28: /* expr: T_true  */
#line 151 "parser.y"
                        { yyval.bval = new bdd(bddtrue); }
#line 1427 "parser.c"
    break;

  case 29: /* expr: T_false  */
#line 152 "parser.y"
                        { yyval.bval = new bdd(bddfalse); }
#line 1433 "parser.c"
    break;

  case 30: /* expr: quantifier  */
#line 153 "parser.y"
                        { yyval.bval = yyvsp[0].bval; }
#line 1439 "parser.c"
    break;

  case 31: /* quantifier: T_exist varlist T_dot expr  */
#line 157 "parser.y"
                              { actExist(&yyval,&yyvsp[-2],&yyvsp[0]); }
#line 1445 "parser.c"
    break;

  case 32: /* quantifier: T_forall varlist T_dot expr  */
#line 158 "parser.y"
                                 { actForall(&yyval,&yyvsp[-2],&yyvsp[0]); }
#line 1451 "parser.c"
    break;

  case 33: /* varlist: varlist T_id  */
#line 162 "parser.y"
                { actQuantVar2(&yyval,&yyvsp[0],&yyvsp[-1]); }
#line 1457 "parser.c"
    break;

  case 34: /* varlist: T_id  */
#line 163 "parser.y"
                { actQuantVar1(&yyval,&yyvsp[0]); }
#line 1463 "parser.c"
    break;

  case 35: /* size: T_size T_id  */
#line 168 "parser.y"
               { actSize(&yyvsp[0]); }
#line 1469 "parser.c"
    break;

  case 36: /* dot: T_dumpdot T_str T_id  */
#line 172 "parser.y"
                        { actDot(&yyvsp[-1],&yyvsp[0]); }
#line 1475 "parser.c"
    break;

  case 37: /* reorder: T_reorder method  */
#line 176 "parser.y"
                                   { bdd_reorder(yyvsp[0].ival); }
#line 1481 "parser.c"
    break;

  case 38: /* reorder: T_autoreorder T_intval method  */
#line 177 "parser.y"
                                   { actAutoreorder(&yyvsp[-1],&yyvsp[0]); }
#line 1487 "parser.c"
    break;

  case 39: /* method: T_win2  */
#line 181 "parser.y"
                { yyval.ival = BDD_REORDER_WIN2; }
#line 1493 "parser.c"
    break;

  case 40: /* method: T_win2ite  */
#line 182 "parser.y"
                { yyval.ival = BDD_REORDER_WIN2ITE; }
#line 1499 "parser.c"
    break;

  case 41: /* method: T_sift  */
#line 183 "parser.y"
                { yyval.ival = BDD_REORDER_SIFT; }
#line 1505 "parser.c"
    break;

  case 42: /* method: T_siftite  */
#line 184 "parser.y"
                { yyval.ival = BDD_REORDER_SIFTITE; }
#line 1511 "parser.c"
    break;

  case 43: /* method: T_none  */
#line 185 "parser.y"
                { yyval.ival = BDD_REORDER_NONE; }
#line 1517 "parser.c"
    break;

  case 44: /* cache: T_cache  */
#line 189 "parser.y"
           { actCache(); }
#line 1523 "parser.c"
    break;

  case 45: /* tautology: T_tautology T_id  */
#line 193 "parser.y"
                    { actTautology(&yyvsp[0]); }
#line 1529 "parser.c"
    break;

  case 46: /* print: T_print T_id  */
#line 197 "parser.y"
                { actPrint(&yyvsp[0]); }
#line 1535 "parser.c"
    break;


#line 1539 "parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 199 "parser.y"

/*************************************************************************
   Main and more
*************************************************************************/

void usage(void)
{
   cerr << "USAGE: bddcalc [-hg] file\n";
   cerr << " -h : print this message\n";
   cerr << " -g : disable garbage collection info\n";
}


int main(int ac, char **av)
{
   int c;

   while ((c=getopt(ac, av, "hg")) != EOF)
   {
      switch (c)
      {
      case 'h':
	 usage();
	 break;
      case 'g':
	 gbcHandler = bdd_default_gbchandler;
	 break;
      }
   }

   if (optind >= ac)
      usage();

   yyin = fopen(av[optind],"r");
   if (!yyin)
   {
      cerr << "Could not open file: " << av[optind] << endl;
      exit(2);
   }

   linenum = 1;
   bdd_setcacheratio(2);
   yyparse();

   bdd_printstat();
   bdd_done();

   return 0;
}


void yyerror(const char *fmt, ...)
{
   va_list argp;
   va_start(argp,fmt);
   fprintf(stderr, "Parse error in (or before) line %d: ", linenum);
   vfprintf(stderr, fmt, argp);
   va_end(argp);
   exit(3);
}


/*************************************************************************
   Semantic actions
*************************************************************************/

void actInit(token *nodes, token *cache)
{
   bdd_init(nodes->ival, cache->ival);
   bdd_gbc_hook(gbcHandler);
   bdd_reorder_verbose(0);
}


void actInputs(void)
{
   bdd_setvarnum(inputs.size());

   int vnum=0;
   for (nodeLst::ite i=inputs.first() ; i.more() ; i++, vnum++)
   {
      if (names.exists((*i).name))
	 yyerror("Redefinition of input %s", (*i).name);

      (*i).val = bdd_ithvar(vnum);
      hashData hd((*i).name, 0, &(*i));
      names.add(hd);
   }

   bdd_varblockall();
}


void actAddInput(token *id)
{
   inputs.append( nodeData(inputTag,sdup(id->id),bddtrue) );
}


void actAssign(token *id, token *expr)
{
   if (names.exists(id->id))
      yyerror("Redefinition of %s", id->id);

   nodeData *d = new nodeData(exprTag, sdup(id->id), *expr->bval);
   hashData hd(d->name, 0, d);
   names.add(hd);
   delete expr->bval;
}


void actOpr2(token *res, token *left, token *right, int opr)
{
   res->bval = new bdd( bdd_apply(*left->bval, *right->bval, opr) );
   delete left->bval;
   delete right->bval;
}


void actNot(token *res, token *right)
{
   res->bval = new bdd( bdd_not(*right->bval) );
   delete right->bval;
   //printf("%5d -> %f\n", fixme, bdd_satcount(*res->bval));
}


void actId(token *res, token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      res->bval = new bdd( ((nodeData*)hd.def)->val );
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actExist(token *res, token *var, token *expr)
{
   res->bval = new bdd( bdd_exist(*expr->bval, *var->bval) );
   delete var->bval;
   delete expr->bval;
}


void actForall(token *res, token *var, token *expr)
{
   res->bval = new bdd( bdd_forall(*expr->bval, *var->bval) );
   delete var->bval;
   delete expr->bval;
}


void actQuantVar2(token *res, token *id, token *list)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (hd.type == inputTag)
      {
	 res->bval = list->bval;
	 *res->bval &= ((nodeData*)hd.def)->val;
      }
      else
	 yyerror("%s is not a variable", id->id);
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actQuantVar1(token *res, token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (hd.type == inputTag)
	 res->bval = new bdd( ((nodeData*)hd.def)->val );
      else
	 yyerror("%s is not a variable", id->id);
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actSize(token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      cout << "Number of nodes used for " << id->id << " = "
	   << bdd_nodecount(((nodeData*)hd.def)->val) << endl;
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actDot(token *fname, token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (bdd_fnprintdot(fname->str, ((nodeData*)hd.def)->val) < 0)
	 cout << "Could not open file: " << fname->str << endl;
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actAutoreorder(token *times, token *method)
{
   if (times->ival == 0)
      bdd_autoreorder(method->ival);
   else
      bdd_autoreorder_times(method->ival, times->ival);
}


void actCache(void)
{
   bdd_printstat();
}


void actTautology(token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (((nodeData*)hd.def)->val == bddtrue)
	 cout << "Formula " << id->id << " is a tautology!\n";
      else
	 cout << "Formula " << id->id << " is NOT a tautology!\n";
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actPrint(token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
      cout << id->id << " = " << bddset << ((nodeData*)hd.def)->val << endl;
   else
      yyerror("Unknown variable %s", id->id);
}

/* EOF */
