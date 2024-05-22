/*========================================================================
	       Copyright (C) 1996-2004, 2021 by Jorn Lind-Nielsen
			    All rights reserved

    Permission is hereby granted, without written agreement and without
    license or royalty fees, to use, reproduce, prepare derivative
    works, distribute, and display this software and its documentation
    for any purpose, provided that (1) the above copyright notice and
    the following two paragraphs appear in all copies of the source code
    and (2) redistributions, including without limitation binaries,
    reproduce these notices in the supporting documentation. Substantial
    modifications to this software may be copyrighted by their authors
    and need not follow the licensing terms described here, provided
    that the new terms are clearly indicated in all files where they apply.

    IN NO EVENT SHALL JORN LIND-NIELSEN, OR DISTRIBUTORS OF THIS
    SOFTWARE BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
    INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS
    SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHORS OR ANY OF THE
    ABOVE PARTIES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    JORN LIND-NIELSEN SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
    BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS
    ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO
    OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
    MODIFICATIONS.
========================================================================*/

/*************************************************************************
  $Header: /Volumes/CVS/repository/spot/spot/buddy/src/bddop.c,v 1.9 2004/01/07 16:05:21 adl Exp $
  FILE:  bddop.c
  DESCR: BDD operators
  AUTH:  Jorn Lind
  DATE:  (C) nov 1997
*************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "kernel.h"
#include "cache.h"

#ifdef __has_attribute
#  if __has_attribute(fallthrough)
#    define BUDDY_FALLTHROUGH __attribute__ ((fallthrough))
#  endif
#endif
#ifndef BUDDY_FALLTHROUGH
#  define BUDDY_FALLTHROUGH while (0);
#endif

   /* Hash value modifiers to distinguish between entries in misccache */
#define CACHEID_CONSTRAIN   0x0
#define CACHEID_RESTRICT    0x1
#define CACHEID_SATCOU      0x2
#define CACHEID_SATCOULN    0x3
#define CACHEID_PATHCOU     0x4
#define CACHEID_SETXOR      0x5
#define CACHEID_SUPPORT     0x6
#define CACHEID_IMPLIES     0x7
#define CACHEID_COMMON      0x8
#define CACHEID_SHORTDIST   0x9
#define CACHEID_SHORTBDD    0xA

   /* Hash value modifiers for replace/compose */
#define CACHEID_REPLACE      0x0
#define CACHEID_COMPOSE      0x1
#define CACHEID_VECCOMPOSE   0x2

   /* Hash value modifiers for quantification */
#define CACHEID_EXIST        0x0
#define CACHEID_FORALL       0x1
#define CACHEID_UNIQUE       0x2
#define CACHEID_APPEX        0x3
#define CACHEID_APPAL        0x4
#define CACHEID_APPUN        0x5
#define CACHEID_EXISTC       0x6
#define CACHEID_FORALLC      0x7
#define CACHEID_UNIQUEC      0x8
#define CACHEID_APPEXC       0x9
#define CACHEID_APPALC       0xA
#define CACHEID_APPUNC       0xB


   /* Number of boolean operators */
#define OPERATOR_NUM    11

   /* Operator results - entry = left<<1 | right  (left,right in {0,1}) */
static int oprres[OPERATOR_NUM][4] =
{ {0,0,0,1},  /* and                       ( & )         */
  {0,1,1,0},  /* xor                       ( ^ )         */
  {0,1,1,1},  /* or                        ( | )         */
  {1,1,1,0},  /* nand                                    */
  {1,0,0,0},  /* nor                                     */
  {1,1,0,1},  /* implication               ( >> )        */
  {1,0,0,1},  /* bi-implication                          */
  {0,0,1,0},  /* difference /greater than  ( - ) ( > )   */
  {0,1,0,0},  /* less than                 ( < )         */
  {1,0,1,1},  /* inverse implication       ( << )        */
  {1,1,0,0}   /* not                       ( ! )         */
};


   /* Variables needed for the operators */
static int applyop;                 /* Current operator for apply */
static int appexop;                 /* Current operator for appex */
static int appexid;                 /* Current cache id for appex */
static int quantid;                 /* Current cache id for quantifications */
static int *quantvarset;            /* Current variable set for quant. */
static int quantvarsetcomp;         /* Should quantvarset be complemented?  */
static int quantvarsetID;           /* Current id used in quantvarset */
static int quantlast;               /* Current last variable to be quant. */
static int replaceid;               /* Current cache id for replace */
static int *replacepair;            /* Current replace pair */
static int replacelast;             /* Current last var. level to replace */
static int composelevel;            /* Current variable used for compose */
static int miscid;                  /* Current cache id for other results */
static int *varprofile;             /* Current variable profile */
static int supportID;               /* Current ID (true value) for support */
static int supportMin;              /* Min. used level in support calc. */
static int supportMax;              /* Max. used level in support calc. */
static int* supportSet;             /* The found support set */
static BddCache applycache;         /* Cache for apply results */
static BddCache itecache;           /* Cache for ITE results */
static BddCache quantcache;         /* Cache for exist/forall results */
static BddCache appexcache;         /* Cache for appex/appall results */
static BddCache replacecache;       /* Cache for replace results */
static BddCache misccache;          /* Cache for other results */
static int misccache_varnum;        /* if this changes, invalidate misccache */
static int cacheratio;
static BDD satPolarity;
static int firstReorder;            /* Used instead of local variable in order
				       to avoid compiler warning about 'first'
				       being clobbered by setjmp */

static signed char*     allsatProfile; /* Variable profile for bdd_allsat() */
static bddallsathandler allsatHandler; /* Callback handler for bdd_allsat() */

extern bddCacheStat bddcachestats;

   /* Internal prototypes */
static BDD    not_rec(BDD);  /* non-recursive implementation */
static BDD    apply_rec(BDD, BDD);  /* non-recursive implementation */
static BDD    ite_rec(BDD, BDD, BDD);  /* non-recursive implementation */
static int    simplify_rec(BDD, BDD);
static BDD    quant_rec(BDD);  /* non-recursive implementation */
static BDD    appquant_rec(BDD, BDD);  /* non-recursive implementation */
static BDD    restrict_rec(BDD);  /* non-recursive implementation */
static BDD    constrain_rec(BDD, BDD);
static BDD    replace_rec(BDD);
static BDD    bdd_correctify(int, BDD, BDD);
static BDD    compose_rec(BDD, BDD);  /* non-recursive implementation */
static BDD    veccompose_rec(BDD);
static void   support_rec(int, int*);  /* non-recursive implementation */
static BDD    satone_rec(BDD);  /* non-recursive implementation */
static BDD    satoneset_rec(BDD, BDD); /* non-recursive implementation */
static int    fullsatone_rec(int);
static BDD    satprefix_rec(BDD*);
static void   allsat_rec(BDD r);
static double satcount_rec(int);
static double satcountln_rec(int);
static void   varprofile_rec(int);
static double bdd_pathcount_rec(BDD);
static int    varset2vartable(BDD, int);
static int    varset2svartable(BDD);


   /* Hashvalues */
#define NOTHASH(r)           (r)
#define APPLYHASH(l,r,op)    (TRIPLE(l,r,op))
#define ITEHASH(f,g,h)       (TRIPLE(f,g,h))
#define SETXORHASH(l,r)      (TRIPLE(l,r,CACHEID_SETXOR))
#define IMPLIESHASH(l,r)     (TRIPLE(l,r,CACHEID_IMPLIES))
#define COMMONHASH(l,r)      (TRIPLE(l,r,CACHEID_COMMON))
#define RESTRHASH(r,var)     (PAIR(r,var))
#define CONSTRAINHASH(f,c)   (PAIR(f,c))
#define QUANTHASH(r)         (r)
#define REPLACEHASH(r)       (r)
#define VECCOMPOSEHASH(f)    (f)
#define COMPOSEHASH(f,g)     (PAIR(f,g))
#define SATCOUHASH(r)        (r)
#define PATHCOUHASH(r)       (r)
#define SUPPORTHASH(r)       (PAIR(r,CACHEID_SUPPORT))
#define APPEXHASH(l,r,op)    (PAIR(l,r))
#define SHORTDISTHASH(l,rev) (TRIPLE(l,rev,CACHEID_SHORTDIST))
#define SHORTBDDHASH(l,rev) (TRIPLE(l,rev,CACHEID_SHORTBDD))

#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

#define log1p(a) (log(1.0+a))

/* unsigned check */
#define INVARSET(a) ((quantvarset[a] == quantvarsetID) ^ quantvarsetcomp)
/* signed check */
#define INSVARSET(a) (abs(quantvarset[a]) == quantvarsetID)

/*************************************************************************
  Setup and shutdown
*************************************************************************/

int bdd_operator_init(int cachesize)
{
   if (BddCache_init(&applycache,cachesize) < 0)
      return bdd_error(BDD_MEMORY);

   if (BddCache_init(&itecache,cachesize) < 0)
      return bdd_error(BDD_MEMORY);

   if (BddCache_init(&quantcache,cachesize) < 0)
      return bdd_error(BDD_MEMORY);

   if (BddCache_init(&appexcache,cachesize) < 0)
      return bdd_error(BDD_MEMORY);

   if (BddCache_init(&replacecache,cachesize) < 0)
      return bdd_error(BDD_MEMORY);

   if (BddCache_init(&misccache,cachesize) < 0)
      return bdd_error(BDD_MEMORY);
   misccache_varnum = bddvarnum;

   quantvarsetID = 0;
   quantvarset = NULL;
   cacheratio = 0;
   supportSet = NULL;

   return 0;
}


void bdd_operator_done(void)
{
   if (quantvarset != NULL)
      free(quantvarset);

   BddCache_done(&applycache);
   BddCache_done(&itecache);
   BddCache_done(&quantcache);
   BddCache_done(&appexcache);
   BddCache_done(&replacecache);
   BddCache_done(&misccache);

   if (supportSet != NULL)
     free(supportSet);
}


void bdd_operator_reset(void)
{
   BddCache_reset(&applycache);
   BddCache_reset(&itecache);
   BddCache_reset(&quantcache);
   BddCache_reset(&appexcache);
   BddCache_reset(&replacecache);
   BddCache_reset(&misccache);
   misccache_varnum = bddvarnum;
}


void bdd_operator_varresize(void)
{
   if (quantvarset != NULL)
      free(quantvarset);

   if ((quantvarset=NEW(int,bddvarnum)) == NULL)
      bdd_error(BDD_MEMORY);
   else
     memset(quantvarset, 0, sizeof(int)*bddvarnum);
   quantvarsetID = 0;
}


static void bdd_operator_noderesize(void)
{
   if (cacheratio > 0)
   {
      int newcachesize = bddnodesize / cacheratio;
      BddCache_resize(&applycache, newcachesize);
      BddCache_resize(&itecache, newcachesize);
      BddCache_resize(&quantcache, newcachesize);
      BddCache_resize(&appexcache, newcachesize);
      BddCache_resize(&replacecache, newcachesize);
      BddCache_resize(&misccache, newcachesize);
      bddcachesize = misccache.tablesize;
   }
}


/*************************************************************************
  Other
*************************************************************************/

/*
NAME    {* bdd\_setcacheratio *}
SECTION {* kernel *}
SHORT   {* Sets the cache ratio for the operator caches *}
PROTO   {* int bdd_setcacheratio(int r) *}
DESCR   {* The ratio between the number of nodes in the nodetable
	   and the number of entries in the operator cachetables is called
	   the cache ratio. So a cache ratio of say, four, allocates one cache
	   entry for each four unique node entries. This value can be set with
	   {\tt bdd\_setcacheratio} to any positive value. When this is done
	   the caches are resized instantly to fit the new ratio.
	   The default is a fixed cache size determined at
	   initialization time. *}
RETURN  {* The previous cache ratio or a negative number on error. *}
ALSO    {* bdd\_init *}
*/
int bdd_setcacheratio(int r)
{
   int old = cacheratio;

   if (r <= 0)
      return bdd_error(BDD_RANGE);
   if (bddnodesize == 0)
      return old;

   cacheratio = r;
   bdd_operator_noderesize();
   return old;
}


/*************************************************************************
  Operators
*************************************************************************/

static void checkresize(void)
{
   if (bddresized)
      bdd_operator_noderesize();
   bddresized = 0;
}


/*=== BUILD A CUBE =====================================================*/

/*
NAME    {* bdd\_buildcube *}
EXTRA   {* bdd\_ibuildcube *}
SECTION {* operator *}
SHORT   {* build a cube from an array of variables *}
PROTO   {* BDD bdd_buildcube(int value, int width, BDD *var)
BDD bdd_ibuildcube(int value, int width, int *var)*}
DESCR   {* This function builds a cube from the variables in {\tt
	   var}. It does so by interpreting the {\tt width} low order
	   bits of {\tt value} as a bit mask--a set bit indicates that the
	   variable should be added in it's positive form, and a cleared
	   bit the opposite. The most significant bits are encoded with
	   the first variables in {\tt var}. Consider as an example
	   the call {\tt bdd\_buildcube(0xB, 4, var)}. This corresponds
	   to the expression: $var[0] \conj \neg var[1] \conj var[2]
	   \conj var[3]$. The first version of the function takes an array
	   of BDDs, whereas the second takes an array of variable numbers
	   as used in {\tt bdd\_ithvar}. *}
RETURN  {* The resulting cube *}
ALSO    {* bdd\_ithvar, fdd\_ithvar *}
*/
BDD bdd_buildcube(int value, int width, BDD *variables)
{
   BDD result = BDDONE;
   int z;

   for (z=0 ; z<width ; z++, value>>=1)
   {
      BDD tmp;
      BDD v;

      if (value & 0x1)
	 v = bdd_addref( variables[width-z-1] );
      else
	 v = bdd_addref( bdd_not(variables[width-z-1]) );

      bdd_addref(result);
      tmp = bdd_apply(result,v,bddop_and);
      bdd_delref(result);
      bdd_delref(v);

      result = tmp;
   }

   return result;
}


BDD bdd_ibuildcube(int value, int width, int *variables)
{
   BDD result = BDDONE;
   int z;

   for (z=0 ; z<width ; z++, value>>=1)
   {
      BDD tmp;
      BDD v;

      if (value & 0x1)
	 v = bdd_ithvar(variables[width-z-1]);
      else
	 v = bdd_nithvar(variables[width-z-1]);

      bdd_addref(result);
      tmp = bdd_apply(result,v,bddop_and);
      bdd_delref(result);

      result = tmp;
   }

   return result;
}

BDD bdd_ibuildcube2(int width, const int *variables)
{
   if (width == 0)
     return BDDONE;

   BDD result;
   {
     int vn = variables[width - 1];
      if (vn >= 0)
        result = bdd_ithvar(vn);
      else
        result = bdd_nithvar(~vn);
   }

   for (int z = width - 2 ; z >= 0 ; --z)
   {
      BDD v;
      int vn = variables[z];
      if (vn >= 0)
        v = bdd_ithvar(vn);
      else
        v = bdd_nithvar(~vn);
      int lv = LEVEL(v);
      if (__likely(LEVEL(result) > lv))
        {
          PUSHREF(result);
          if (vn >= 0)
            result = bdd_makenode(lv, BDDZERO, result);
          else
            result = bdd_makenode(lv, result, BDDZERO);
          POPREF(1);
        }
      else
        {
          bdd_addref(result);
          BDD tmp = bdd_apply(result, v, bddop_and);
          bdd_delref(result);
          result = tmp;
        }
   }

   return result;
}

mintermEnumerator* bdd_init_minterm(BDD fun, BDD vars)
{
  mintermEnumerator* me = malloc(sizeof(mintermEnumerator));
  if (__unlikely(me == NULL))
    {
      bdd_error(BDD_MEMORY);
      return NULL;
    }
  int* varsptr = malloc(sizeof(int)*bddvarnum);
  if (__unlikely(varsptr == NULL))
    {
      free(me);
      bdd_error(BDD_MEMORY);
      return NULL;
    }
  int i = 0;
  while (!ISCONST(vars))
    {
      varsptr[i++] = bddlevel2var[LEVEL(vars)];
      vars = HIGH(vars);
    }
  int* stack = malloc(sizeof(int)*i*2);
  if (__unlikely(stack == NULL))
    {
      free(varsptr);
      free(me);
      bdd_error(BDD_MEMORY);
      return NULL;
    }
  bdd_addref(fun);
  me->vars = varsptr;
  me->stacktop = stack;
  me->stack = stack;
  me->fun = fun;
  me->nvars = i;
  return me;
}

void bdd_free_minterm(mintermEnumerator* me)
{
  bdd_delref(me->fun);
  free(me->stack);
  free(me->vars);
  free(me);
}

static int reset_minterm(mintermEnumerator*me,
                         BDD sub, int var)
{
  int nvars = me->nvars;
  int* vars = me->vars;
  int ls = LEVEL(sub);
  for (int i = var; i < nvars; ++i)
    {
      int v = vars[i];
      int neg = v < 0;
      if (neg)
        v = ~v;
      int lv = bddvar2level[v];
      if (ls == lv)            /* variable used in fun */
        {
          BDD low = LOW(sub);
          if (low == BDDZERO)
            {
              sub = HIGH(sub);
            }
          else
            {
              if (HIGH(sub) != BDDZERO)
                {
                  *me->stacktop++ = sub;
                  *me->stacktop++ = i;
                }
              v = ~v;
              sub = low;
            }
          ls = LEVEL(sub);
        }
      else
        {
          *me->stacktop++ = sub;
          *me->stacktop++ = i;
          v = ~v;
        }
      vars[i] = v;
    }
  return 1;
}


int bdd_first_minterm(mintermEnumerator* me)
{
  if (__unlikely(me->fun == BDDZERO))
    return 0;
  me->stacktop = me->stack;
  return reset_minterm(me, me->fun, 0);
}

bdd bdd_next_minterm(mintermEnumerator* me)
{
  if (me->stacktop == me->stack) // end of iteration
    return 0;
  int *vars = me->vars;
  // switch polarity of variable of last possible branching
  int lastbranch = *--me->stacktop;
  BDD lastsub = *--me->stacktop;
  int v = ~vars[lastbranch];
  if (LEVEL(lastsub) == bddvar2level[v])
    lastsub = HIGH(lastsub);
  vars[lastbranch] = v;
  // reset everything below to negative polarity
  return reset_minterm(me, lastsub, lastbranch + 1);
}


/*=== NOT ==============================================================*/

/*
NAME    {* bdd\_not *}
SECTION {* operator *}
SHORT   {* negates a bdd *}
PROTO   {* BDD bdd_not(BDD r) *}
DESCR   {* Negates the BDD {\tt r} by exchanging
	   all references to the zero-terminal with references to the
	   one-terminal and vice versa. *}
RETURN  {* The negated bdd. *}
*/
BDD bdd_not(BDD r)
{
   BDD res;
   firstReorder = 1;
   CHECKa(r, bddfalse);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      INITREF;

      if (__likely(firstReorder))
	{
	  res = not_rec(r);
	}
      else
	{
	  bdd_disable_reorder();
	  res = not_rec(r);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();
      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}


static BDD not_rec(BDD r)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;
   do
     {
       index = POPINT_();
       if (index < 0)
         {
           r = POPINT_();
         work:;
           /* C: NULL --- */
           /* I: r --- */
           /* R: --- r */
           if (ISCONST(r))
             {
               PUSHREF_(ISZERO(r) ? BDDONE : BDDZERO);
             }
           else
             {
               BddCacheData *entry2 =
                 BddCache_index(&applycache, NOTHASH(r), index);
               if (entry2->i.a == r && entry2->i.c == bddop_not)
                 {
#ifdef CACHESTATS
                   bddcachestats.opHit++;
#endif
                   PUSHREF_(entry2->i.res);
                 }
               else
                 {
#ifdef CACHESTATS
                   bddcachestats.opMiss++;
#endif
                   /* C: -1 r ---  (-1 lr) -1 rr index r */
                   PUSH4INT_(r, index, HIGH(r), -1);
                   r = LOW(r);
                   goto work;
                 }
             }
         }
       else
         {
           /* C: -1 r --- */
           /* R: rres lres --- res */
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           BDD r = POPINT_();
           SYNC_REC_STACKS;
           BDD res = bdd_makenode(LEVEL(r), lres, rres);
           POPREF_(2);
           PUSHREF_(res);
           BddCacheData* entry = applycache.table + index;
           entry->i.a = r;
           entry->i.c = bddop_not;
           entry->i.res = res;
         }
     }
   while (NONEMPTY_REC_STACK);

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/*=== APPLY ============================================================*/

/*
NAME    {* bdd\_apply *}
SECTION {* operator *}
SHORT   {* basic bdd operations *}
PROTO   {* BDD bdd_apply(BDD left, BDD right, int opr) *}
DESCR   {* The {\tt bdd\_apply} function performs all of the basic
	   bdd operations with two operands, such as AND, OR etc.
	   The {\tt left} argument is the left bdd operand and {\tt right}
	   is the right operand. The {\tt opr} argument is the requested
	   operation and must be one of the following\\

   \begin{tabular}{lllc}
     {\bf Identifier}    & {\bf Description} & {\bf Truth table}
	& {\bf C++ opr.} \\
     {\tt bddop\_and}    & logical and    ($A \wedge B$)         & [0,0,0,1]
	& \verb%&% \\
     {\tt bddop\_xor}    & logical xor    ($A \oplus B$)         & [0,1,1,0]
	& \verb%^% \\
     {\tt bddop\_or}     & logical or     ($A \vee B$)           & [0,1,1,1]
	& \verb%|% \\
     {\tt bddop\_nand}   & logical not-and                       & [1,1,1,0] \\
     {\tt bddop\_nor}    & logical not-or                        & [1,0,0,0] \\
     {\tt bddop\_imp}    & implication    ($A \Rightarrow B$)    & [1,1,0,1]
	& \verb%>>% \\
     {\tt bddop\_biimp}  & bi-implication ($A \Leftrightarrow B$)& [1,0,0,1] \\
     {\tt bddop\_diff}   & set difference ($A \setminus B$)      & [0,0,1,0]
	& \verb%-% \\
     {\tt bddop\_less}   & less than      ($A < B$)              & [0,1,0,0]
	& \verb%<% \\
     {\tt bddop\_invimp} & reverse implication ($A \Leftarrow B$)& [1,0,1,1]
	& \verb%<<% \\
   \end{tabular}
   *}
   RETURN  {* The result of the operation. *}
   ALSO    {* bdd\_ite *}
*/
BDD bdd_apply(BDD l, BDD r, int op)
{
   BDD res;
   firstReorder = 1;

   CHECKa(l, bddfalse);
   CHECKa(r, bddfalse);

#ifndef NDEBUG
   if (op<0 || op>bddop_invimp)
   {
      bdd_error(BDD_OP);
      return bddfalse;
   }
#endif

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      INITREF;
      applyop = op;

      if (__likely(firstReorder))
	{
	  res = apply_rec(l, r);
	}
      else
	{
	  bdd_disable_reorder();
	  res = apply_rec(l, r);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}

#define RETURN(val) { PUSHREF_(val); continue; }
#define APPLY_SHORTCUTS(op, rec)                \
   switch (op)                                  \
     {                                          \
     case bddop_and:                            \
       if (l == r)                              \
         RETURN(rec(l));                        \
       if (l > r)                               \
         {                                      \
           BDD tmp = l;                         \
           l = r;                               \
           r = tmp;                             \
         }                                      \
       if (ISCONST(l))                          \
         RETURN(ISONE(l) ? rec(r) : 0);         \
       break;                                   \
     case bddop_or:                             \
       if (l == r)                              \
         RETURN(rec(l));                        \
       if (l > r)                               \
         {                                      \
           BDD tmp = l;                         \
           l = r;                               \
           r = tmp;                             \
         }                                      \
       if (ISCONST(l))                          \
         RETURN(ISZERO(l) ? rec(r) : 1);        \
       break;                                   \
     case bddop_xor:                            \
       if (l == r)                              \
         RETURN(0);                             \
       if (l > r)                               \
         {                                      \
           BDD tmp = l;                         \
           l = r;                               \
           r = tmp;                             \
         }                                      \
       if (ISZERO(l))                           \
         RETURN(rec(r));                        \
       break;                                   \
     case bddop_nand:                           \
       if (l > r)                               \
         {                                      \
           BDD tmp = l;                         \
           l = r;                               \
           r = tmp;                             \
         }                                      \
       if (ISZERO(l))                           \
         RETURN(1);                             \
       break;                                   \
     case bddop_nor:                            \
       if (l > r)                               \
         {                                      \
           BDD tmp = l;                         \
           l = r;                               \
           r = tmp;                             \
         }                                      \
       if (ISONE(l) || ISONE(r))                \
         RETURN(0);                             \
       break;                                   \
     case bddop_invimp: /* l << r = r >> l */   \
       {                                        \
         BDD tmp = l;                           \
         l = r;                                 \
         r = tmp;                               \
         op = bddop_imp;                        \
       }                                        \
       BUDDY_FALLTHROUGH;                       \
     case bddop_imp:                            \
       if (ISONE(r) || ISZERO(l) || l == r)     \
         RETURN(1);                             \
       if (ISONE(l))                            \
         RETURN(rec(r));                        \
       break;                                   \
     case bddop_biimp:                          \
       if (l == r)                              \
         RETURN(1);                             \
       if (l > r)                               \
         {                                      \
           BDD tmp = l;                         \
           l = r;                               \
           r = tmp;                             \
         }                                      \
       if (ISONE(l))                            \
         RETURN(rec(r));                        \
       break;                                   \
     case bddop_less:  /* l < r = r - l */      \
       {                                        \
         BDD tmp = l;                           \
         l = r;                                 \
         r = tmp;                               \
         op = bddop_diff;                       \
       }                                        \
       BUDDY_FALLTHROUGH;                       \
     case bddop_diff:  /* l - r = l &! r */     \
       if (l == r || ISONE(r))                  \
         RETURN(0);                             \
       if (ISZERO(r))                           \
         RETURN(rec(l));                        \
       break;                                   \
     }

//__attribute__((optimize("no-tree-vectorize")))
static BDD apply_rec(BDD l, BDD r)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;

   do
     {
       index = POPINT_();
       if (index < 0)
         {
           l = POPINT_();
           r = POPINT_();
         work:
           /* empty macro arguments are undefined in ISO C90 and
              ISO C++98, so use + when we do not want to call any
              function.*/
           APPLY_SHORTCUTS(applyop, +);

           if (__unlikely(ISCONST(l) && ISCONST(r)))
             /* C: -1 l r ---     */
             /* R:        --- res */
             RETURN(oprres[applyop][l<<1 | r]);

           BddCacheData *entry2 =
             BddCache_index(&applycache, APPLYHASH(l,r,applyop), index);
           /* Check entry2->b last, because not_rec() does not
              initialize it. */
           if (entry2->i.a == l && entry2->i.c == applyop
               && entry2->i.b == r)
             {
#ifdef CACHESTATS
               bddcachestats.opHit++;
#endif
               /* C: -1 l r ---     */
               /* R:        --- res */
               RETURN(entry2->i.res);
             }
#ifdef CACHESTATS
           bddcachestats.opMiss++;
#endif
           /* C: -1 l r --- (-1 ll rl) -1 lr rr index l r */
           /* The element in parenthesis are not pushed, as they would
              be popped right away.  We jump to "work" instead.*/
           int lvl_l = LEVEL(l);
           int lvl_r = LEVEL(r);
           if (lvl_l == lvl_r)
             {
               PUSH4INT_(r, l, lvl_l, index);
               PUSH3INT_(HIGH(r), HIGH(l), -1);
               r = LOW(r);
               l = LOW(l);
             }
           else if (lvl_l < lvl_r)
             {
               PUSH4INT_(r, l, lvl_l, index);
               PUSH3INT_(r, HIGH(l), -1);
               l = LOW(l);
             }
           else /* (lvl_l > lvl_r) */
             {
               PUSH4INT_(r, l, lvl_r, index);
               PUSH3INT_(HIGH(r), l, -1);
               r = LOW(r);
             }
           goto work;
         }
       else
         {
           /* C: index lvl l r ---     */
           /* R: rres lres     --- res */
           /* res=(lvl, lres, rres) is the result of applyop(l,r) */
           /* and it should be stored in *entry.                     */
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           int lvl = POPINT_();
           BDD l = POPINT_();
           BDD r = POPINT_();
           SYNC_REC_STACKS;
           BDD res = bdd_makenode(lvl, lres, rres);
           POPREF_(2);
           PUSHREF_(res);
           BddCacheData* entry = applycache.table + index;
           entry->i.a = l;
           entry->i.c = applyop;
           entry->i.b = r;
           entry->i.res = res;
         }
     }
   while (NONEMPTY_REC_STACK);
   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/*
NAME    {* bdd\_and *}
SECTION {* operator *}
SHORT   {* The logical 'and' of two BDDs *}
PROTO   {* BDD bdd_and(BDD l, BDD r) *}
DESCR   {* This a wrapper that calls {\tt bdd\_apply(l,r,bddop\_and)}. *}
RETURN  {* The logical 'and' of {\tt l} and {\tt r}. *}
ALSO    {* bdd\_apply, bdd\_or, bdd\_xor *}
*/
BDD bdd_and(BDD l, BDD r)
{
   return bdd_apply(l,r,bddop_and);
}


/*
NAME    {* bdd\_or *}
SECTION {* operator *}
SHORT   {* The logical 'or' of two BDDs *}
PROTO   {* BDD bdd_or(BDD l, BDD r) *}
DESCR   {* This a wrapper that calls {\tt bdd\_apply(l,r,bddop\_or)}. *}
RETURN  {* The logical 'or' of {\tt l} and {\tt r}. *}
ALSO    {* bdd\_apply, bdd\_xor, bdd\_and *}
*/
BDD bdd_or(BDD l, BDD r)
{
   return bdd_apply(l,r,bddop_or);
}


/*
NAME    {* bdd\_xor *}
SECTION {* operator *}
SHORT   {* The logical 'xor' of two BDDs *}
PROTO   {* BDD bdd_xor(BDD l, BDD r) *}
DESCR   {* This a wrapper that calls {\tt bdd\_apply(l,r,bddop\_xor)}. *}
RETURN  {* The logical 'xor' of {\tt l} and {\tt r}. *}
ALSO    {* bdd\_apply, bdd\_or, bdd\_and *}
*/
BDD bdd_xor(BDD l, BDD r)
{
   return bdd_apply(l,r,bddop_xor);
}


/*
NAME    {* bdd\_imp *}
SECTION {* operator *}
SHORT   {* The logical 'implication' between two BDDs *}
PROTO   {* BDD bdd_imp(BDD l, BDD r) *}
DESCR   {* This a wrapper that calls {\tt bdd\_apply(l,r,bddop\_imp)}. *}
RETURN  {* The logical 'implication' of {\tt l} and {\tt r} ($l \Rightarrow r$). *}
ALSO    {* bdd\_apply, bdd\_biimp *}
*/
BDD bdd_imp(BDD l, BDD r)
{
   return bdd_apply(l,r,bddop_imp);
}


/*
NAME    {* bdd\_biimp *}
SECTION {* operator *}
SHORT   {* The logical 'bi-implication' between two BDDs *}
PROTO   {* BDD bdd_biimp(BDD l, BDD r) *}
DESCR   {* This a wrapper that calls {\tt bdd\_apply(l,r,bddop\_biimp)}. *}
RETURN  {* The logical 'bi-implication' of {\tt l} and {\tt r} ($l \Leftrightarrow r$). *}
ALSO    {* bdd\_apply, bdd\_imp *}
*/
BDD bdd_biimp(BDD l, BDD r)
{
   return bdd_apply(l,r,bddop_biimp);
}

/*=== bdd_setxor =========================================================*/

/*
NAME    {* bdd\_setxor *}
SECTION {* operator *}
SHORT   {* xor operator for variable sets *}
PROTO   {* BDD bdd_setxor(BDD l, BDD r) *}
DESCR   {* Given to set $l$ and $r$ represented as conjuctions of variables.
           (positive variables are present in the set, negative or absent
	   variables are not in the set), this computes
	   $(l\setminus r)\cup(r\setminus l)$.  The resulting set will be
	   represented as a conjunction in which all variables occur
	   (positive variables are present in the set, negative variables
	   are not in the set) *}
RETURN  {* The BDD for $(l\setminus r)\cup(r\setminus l)$ *}
*/
BDD bdd_setxor(BDD l, BDD r)
{
  BddCacheData *entry;
  BDD res;

  if (ISONE(l))
    return r;
  if (ISONE(r))
    return l;

   entry = BddCache_lookup(&misccache, SETXORHASH(l,r));
   if (entry->i.a == l &&  entry->i.b == r  && entry->i.c == CACHEID_SETXOR)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

  if (LEVEL(l) == LEVEL(r))
    {
      if (ISZERO(LOW(l)))
	{
	  if (ISZERO(LOW(r)))
	    {
	      PUSHREF(bdd_setxor(HIGH(l), HIGH(r)));
	      res = bdd_makenode(LEVEL(l), READREF(1), 0);
	    }
	  else
	    {
	      PUSHREF(bdd_setxor(HIGH(l), LOW(r)));
	      res = bdd_makenode(LEVEL(l), 0, READREF(1));
	    }
	}
      else
	{
	  if (ISZERO(LOW(r)))
	    {
	      PUSHREF(bdd_setxor(LOW(l), HIGH(r)));
	      res = bdd_makenode(LEVEL(l), 0, READREF(1));
	    }
	  else
	    {
	      PUSHREF(bdd_setxor(LOW(l), LOW(r)));
	      res = bdd_makenode(LEVEL(l), READREF(1), 0);
	    }
	}
    }
  else if (LEVEL(l) < LEVEL(r))
    {
      if (ISZERO(LOW(l)))
	{
	  PUSHREF(bdd_setxor(HIGH(l), r));
	  res = bdd_makenode(LEVEL(l), 0, READREF(1));
	}
      else
	{
	  PUSHREF(bdd_setxor(LOW(l), r));
	  res = bdd_makenode(LEVEL(l), READREF(1), 0);
	}
    }
  else /* (LEVEL(l) > LEVEL(r)) */
    {
      if (ISZERO(LOW(r)))
	{
	  PUSHREF(bdd_setxor(l, HIGH(r)));
	  res = bdd_makenode(LEVEL(r), 0, READREF(1));
	}
      else
	{
	  PUSHREF(bdd_setxor(l, LOW(r)));
	  res = bdd_makenode(LEVEL(r), READREF(1), 0);
	}
    }

  POPREF(1);

  entry->i.a = l;
  entry->i.c = CACHEID_SETXOR;
  entry->i.b = r;
  entry->i.res = res;

  return res;
}


/*=== bdd_implies =========================================================*/

/*
NAME    {* bdd\_implies *}
SECTION {* operator *}
SHORT   {* check whether one BDD implies another *}
PROTO   {* int bdd_implies(BDD l, BDD r) *}
DESCR   {* Check whether $l$ implies $r$, or whether $r$ contains $l$. *}
RETURN  {* 1 if $l$ implies $r$, 0 otherwise *}
*/
int bdd_implies(BDD l, BDD r)
{
  BddCacheData *entry;
  int res;

  if ((l == r) || ISZERO(l) || ISONE(r))
    return 1;
  if (ISONE(l) || ISZERO(r))
    return 0;

  entry = BddCache_lookup(&misccache, IMPLIESHASH(l,r));
  /* Check entry->b last, because not_rec() does not initialize it. */
  if (entry->i.a == l && entry->i.c == CACHEID_IMPLIES && entry->i.b == r)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

  if (LEVEL(l) == LEVEL(r))
    {
      int hl = HIGH(l);
      int hr = HIGH(r);
      /* Avoid the recursion if the second implication will trivially
         fail. */
      if ((ISONE(hl) || ISZERO(hr)) && (hl != hr))
        res = 0;
      else
        res = bdd_implies(LOW(l), LOW(r)) && bdd_implies(hl, hr);
    }
  else if (LEVEL(l) < LEVEL(r))
    {
      int hl = HIGH(l);
      if (ISONE(hl) && (hl != r))
        res = 0;
      else
        res = bdd_implies(LOW(l), r) && bdd_implies(HIGH(l), r);
    }
  else /* LEVEL(l) > LEVEL(r) */
    {
      int hr = HIGH(r);
      if (ISZERO(hr) && (hr != r))
        res = 0;
      else
        res = bdd_implies(l, LOW(r)) && bdd_implies(l, HIGH(r));
    }

  entry->i.a = l;
  entry->i.c = CACHEID_IMPLIES;
  entry->i.b = r;
  entry->i.res = res;

  return res;
}



/*=== ITE ==============================================================*/

/*
NAME    {* bdd\_ite *}
SECTION {* operator *}
SHORT   {* if-then-else operator *}
PROTO   {* BDD bdd_ite(BDD f, BDD g, BDD h) *}
DESCR   {* Calculates the BDD for the expression
	   $(f \conj g) \disj (\neg f \conj h)$ more efficiently than doing
	   the three operations separately. {\tt bdd\_ite} can also be used
	   for conjunction, disjunction and any other boolean operator, but
	   is not as efficient for the binary and unary operations. *}
RETURN  {* The BDD for $(f \conj g) \disj (\neg f \conj h)$ *}
ALSO    {* bdd\_apply *}
*/
BDD bdd_ite(BDD f, BDD g, BDD h)
{
   BDD res;
   firstReorder = 1;

   CHECKa(f, bddfalse);
   CHECKa(g, bddfalse);
   CHECKa(h, bddfalse);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      INITREF;

      if (__likely(firstReorder))
	{
	  res = ite_rec(f,g,h);
	}
      else
	{
	  bdd_disable_reorder();
	  res = ite_rec(f,g,h);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}

static BDD ite_rec(BDD f, BDD g, BDD h)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;
   do
     {
       index = POPINT_();
       if (index < 0)
         {
           f = POPINT_();
           g = POPINT_();
           h = POPINT_();
         work:
           if (ISONE(f))
             RETURN(g);
           if (ISZERO(f))
             RETURN(h);
           if (g == h)
             RETURN(g);
           if (ISONE(g) && ISZERO(h))
             RETURN(f);
           if (ISZERO(g) && ISONE(h))
             {
               SYNC_REC_STACKS;
               int res = not_rec(f);
               RETURN(res);
             }

           BddCacheData *entry2 =
             BddCache_index(&itecache, ITEHASH(f, g, h), index);
           if (entry2->i.a == f && entry2->i.c == h && entry2->i.b == g)
             {
#ifdef CACHESTATS
               bddcachestats.opHit++;
#endif
               /* C: -1 f g h  ---     */
               /* R:           --- res */
               RETURN(entry2->i.res);
             }
#ifdef CACHESTATS
           bddcachestats.opMiss++;
#endif
           /* C: -1 f g h --- (-1 fl gl hl) -1 fr gr hr index lvl f g h */
           int lvl_f = LEVEL(f);
           int lvl_g = LEVEL(g);
           int lvl_h = LEVEL(h);
           if (lvl_f == lvl_g)
             {
               if (lvl_g == lvl_h)
                 {
                   PUSH4INT_(h, g, f, lvl_f);
                   PUSHINT_(index);
                   PUSH4INT_(HIGH(h), HIGH(g), HIGH(f), -1);
                   h = LOW(h);
                   g = LOW(g);
                   f = LOW(f);
                 }
               else if (lvl_f < lvl_h)
                 {
                   PUSH4INT_(h, g, f, lvl_f);
                   PUSHINT_(index);
                   PUSH4INT_(h, HIGH(g), HIGH(f), -1);
                   g = LOW(g);
                   f = LOW(f);
                 }
               else /* lvl_f > lvl_h */
                 {
                   PUSH4INT_(h, g, f, lvl_h);
                   PUSHINT_(index);
                   PUSH4INT_(HIGH(h), g, f, -1);
                   h = LOW(h);
                 }
             }
           else if (lvl_f < lvl_g)
             {
               if (lvl_f == lvl_h)
                 {
                   PUSH4INT_(h, g, f, lvl_f);
                   PUSHINT_(index);
                   PUSH4INT_(HIGH(h), g, HIGH(f), -1);
                   h = LOW(h);
                   f = LOW(f);
                 }
               else if (lvl_f < lvl_h)
                 {
                   PUSH4INT_(h, g, f, lvl_f);
                   PUSHINT_(index);
                   PUSH4INT_(h, g, HIGH(f), -1);
                   f = LOW(f);
                 }
               else /* lvl_f > lvl_h */
                 {
                   PUSH4INT_(h, g, f, lvl_h);
                   PUSHINT_(index);
                   PUSH4INT_(HIGH(h), g, f, -1);
                   h = LOW(h);
                 }
             }
           else /* lvl_f > lvl_g */
             {
               if (lvl_g == lvl_h)
                 {
                   PUSH4INT_(h, g, f, lvl_g);
                   PUSHINT_(index);
                   PUSH4INT_(HIGH(h), HIGH(g), f, -1);
                   h = LOW(h);
                   g = LOW(g);
                 }
               else if (lvl_g < lvl_h)
                 {
                   PUSH4INT_(h, g, f, lvl_g);
                   PUSHINT_(index);
                   PUSH4INT_(h, HIGH(g), f, -1);
                   g = LOW(g);
                 }
               else /* lvl_g > lvl_h */
                 {
                   PUSH4INT_(h, g, f, lvl_h);
                   PUSHINT_(index);
                   PUSH4INT_(HIGH(h), g, f, -1);
                   h = LOW(h);
                 }
             }
           goto work;
         }
       else
         {
           /* C: index lvl f g h ---     */
           /* R: rres lres       --- res */
           /* res=(lvl, lres, rres) is the result of ite(f,g,h) */
           /* and it should be stored in *entry.                   */
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           int lvl = POPINT_();
           SYNC_REC_STACKS;
           BDD res = bdd_makenode(lvl, lres, rres);
           POPREF_(2);
           PUSHREF_(res);
           BddCacheData* entry = itecache.table + index;
           entry->i.a = POPINT_(); // f
           entry->i.b = POPINT_(); // g
           entry->i.c = POPINT_(); // h
           entry->i.res = res;
         }
     }
   while (NONEMPTY_REC_STACK);

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}

/*=== RESTRICT =========================================================*/

/*
NAME    {* bdd\_restrict *}
SECTION {* operator *}
SHORT   {* restric a set of variables to constant values *}
PROTO   {* BDD bdd_restrict(BDD r, BDD var) *}
DESCR   {* This function restricts the variables in {\tt r} to constant
	   true or false. How this is done
	   depends on how the variables are included in the variable set
	   {\tt var}. If they
	   are included in their positive form then they are restricted to
	   true and vice versa. Unfortunately it is not possible to
	   insert variables in their negated form using {\tt bdd\_makeset},
	   so the variable set has to be build manually as a
	   conjunction of the variables. Example: Assume variable 1 should be
	   restricted to true and variable 3 to false.
	   \begin{verbatim}
  bdd X = make_user_bdd();
  bdd R1 = bdd_ithvar(1);
  bdd R2 = bdd_nithvar(3);
  bdd R = bdd_addref( bdd_apply(R1,R2, bddop_and) );
  bdd RES = bdd_addref( bdd_restrict(X,R) );
\end{verbatim}
	   *}
RETURN  {* The restricted bdd. *}
ALSO    {* bdd\_makeset, bdd\_exist, bdd\_forall *}
*/
BDD bdd_restrict(BDD r, BDD var)
{
   BDD res;
   firstReorder = 1;

   CHECKa(r,bddfalse);
   CHECKa(var,bddfalse);

   if (var < 2)  /* Empty set */
      return r;

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      if (varset2svartable(var) < 0)
	 return bddfalse;

      INITREF;
      miscid = (var << 3) | CACHEID_RESTRICT;

      if (__likely(firstReorder))
	{
	  res = restrict_rec(r);
	}
      else
	{
	  bdd_disable_reorder();
	  res = restrict_rec(r);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}


static BDD restrict_rec(BDD r)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;
   do
     {
       index = POPINT_();
       if (index < 0)
         {
           r = POPINT_();
         work:
           /* C: -1 r ---   */
           /* R:      --- r */
           if (ISCONST(r)  ||  LEVEL(r) > quantlast)
             RETURN(r);

           BddCacheData *entry2 =
             BddCache_index(&misccache, RESTRHASH(r, miscid), index);
           if (entry2->i.a == r && entry2->i.c == miscid)
             {
#ifdef CACHESTATS
               bddcachestats.opHit++;
#endif
               /* C: -1 r --- */
               /* R: --- res */
               RETURN(entry2->i.res);
             }
#ifdef CACHESTATS
           bddcachestats.opMiss++;
#endif
           if (INSVARSET(LEVEL(r)))
             {
               /* C: -1 r --- (-1 r') */
               if (quantvarset[LEVEL(r)] > 0)
                 r = HIGH(r);
               else
                 r = LOW(r);
               goto work;
             }
           else
             {
               /* C: -1 r ---  (-1 lr) -1 rr index r */
               PUSH4INT_(r, index, HIGH(r), -1);
               r = LOW(r);
             }
           goto work;
         }
       else
         {
           /* C: index r   ---     */
           /* R: rres lres --- res */
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           BDD r = POPINT_();
           SYNC_REC_STACKS;
           BDD res = bdd_makenode(LEVEL(r), lres, rres);
           POPREF_(2);
           BddCacheData* entry = misccache.table + index;
           entry->i.a = r;
           entry->i.c = quantid;
           entry->i.res = res;
           PUSHREF_(res);
         }
     }
   while (NONEMPTY_REC_STACK);

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}

/*=== GENERALIZED COFACTOR =============================================*/

/*
NAME    {* bdd\_constrain *}
SECTION {* operator *}
SHORT   {* generalized cofactor *}
PROTO   {* BDD bdd_constrain(BDD f, BDD c) *}
DESCR   {* Computes the generalized cofactor of {\tt f} with respect to
	   {\tt c}. *}
RETURN  {* The constrained BDD *}
ALSO    {* bdd\_restrict, bdd\_simplify *}
*/
BDD bdd_constrain(BDD f, BDD c)
{
   BDD res;
   firstReorder = 1;

   CHECKa(f,bddfalse);
   CHECKa(c,bddfalse);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      INITREF;
      miscid = CACHEID_CONSTRAIN;

      if (__likely(firstReorder))
	{
	  res = constrain_rec(f, c);
	}
      else
	{
	  bdd_disable_reorder();
	  res = constrain_rec(f, c);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}


static BDD constrain_rec(BDD f, BDD c)
{
   BddCacheData *entry;
   BDD res;

   if (ISONE(c))
      return f;
   if (ISCONST(f))
      return f;
   if (c == f)
      return BDDONE;
   if (ISZERO(c))
      return BDDZERO;

   entry = BddCache_lookup(&misccache, CONSTRAINHASH(f,c));
   if (entry->i.a == f  &&  entry->i.b == c  &&  entry->i.c == miscid)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

   if (LEVEL(f) == LEVEL(c))
   {
      if (ISZERO(LOW(c)))
	 res = constrain_rec(HIGH(f), HIGH(c));
      else if (ISZERO(HIGH(c)))
	 res = constrain_rec(LOW(f), LOW(c));
      else
      {
	 PUSHREF( constrain_rec(LOW(f), LOW(c)) );
	 PUSHREF( constrain_rec(HIGH(f), HIGH(c)) );
	 res = bdd_makenode(LEVEL(f), READREF(2), READREF(1));
	 POPREF(2);
      }
   }
   else
   if (LEVEL(f) < LEVEL(c))
   {
      PUSHREF( constrain_rec(LOW(f), c) );
      PUSHREF( constrain_rec(HIGH(f), c) );
      res = bdd_makenode(LEVEL(f), READREF(2), READREF(1));
      POPREF(2);
   }
   else
   {
      if (ISZERO(LOW(c)))
	 res = constrain_rec(f, HIGH(c));
      else if (ISZERO(HIGH(c)))
	 res = constrain_rec(f, LOW(c));
      else
      {
	 PUSHREF( constrain_rec(f, LOW(c)) );
	 PUSHREF( constrain_rec(f, HIGH(c)) );
	 res = bdd_makenode(LEVEL(c), READREF(2), READREF(1));
	 POPREF(2);
      }
   }

   entry->i.a = f;
   entry->i.b = c;
   entry->i.c = miscid;
   entry->i.res = res;

   return res;
}


/*=== REPLACE ==========================================================*/

/*
NAME    {* bdd\_replace *}
SECTION {* operator *}
SHORT   {* replaces variables with other variables *}
PROTO   {* BDD bdd_replace(BDD r, bddPair *pair) *}
DESCR   {* Replaces all variables in the BDD {\tt r} with the variables
	   defined by {\tt pair}. Each entry in {\tt pair} consists of a
	   old and a new variable. Whenever the old variable is found in
	   {\tt r} then a new node with the new variable is inserted instead.
	*}
ALSO   {* bdd\_newpair, bdd\_setpair, bdd\_setpairs *}
RETURN {* The result of the operation. *}
*/
BDD bdd_replace(BDD r, bddPair *pair)
{
   BDD res;
   firstReorder = 1;

   CHECKa(r, bddfalse);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      INITREF;
      replacepair = pair->result;
      replacelast = pair->last;
      replaceid = (pair->id << 2) | CACHEID_REPLACE;

      if (__likely(firstReorder))
	{
	  res = replace_rec(r);
	}
      else
	{
	  bdd_disable_reorder();
	  res = replace_rec(r);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}


static BDD replace_rec(BDD r)
{
   BddCacheData *entry;
   BDD res;

   if (ISCONST(r)  ||  LEVEL(r) > replacelast)
      return r;

   entry = BddCache_lookup(&replacecache, REPLACEHASH(r));
   if (entry->i.a == r  &&  entry->i.c == replaceid)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

   PUSHREF( replace_rec(LOW(r)) );
   PUSHREF( replace_rec(HIGH(r)) );

   res = bdd_correctify(LEVEL(replacepair[LEVEL(r)]), READREF(2), READREF(1));
   POPREF(2);

   entry->i.a = r;
   entry->i.c = replaceid;
   entry->i.res = res;

   return res;
}


static BDD bdd_correctify(int level, BDD l, BDD r)
{
   BDD res;

   if (level < LEVEL(l)  &&  level < LEVEL(r))
      return bdd_makenode(level, l, r);

#ifndef NDEBUG
   if (level == LEVEL(l)  ||  level == LEVEL(r))
   {
      bdd_error(BDD_REPLACE);
      return 0;
   }
#endif

   if (LEVEL(l) == LEVEL(r))
   {
      PUSHREF( bdd_correctify(level, LOW(l), LOW(r)) );
      PUSHREF( bdd_correctify(level, HIGH(l), HIGH(r)) );
      res = bdd_makenode(LEVEL(l), READREF(2), READREF(1));
   }
   else
   if (LEVEL(l) < LEVEL(r))
   {
      PUSHREF( bdd_correctify(level, LOW(l), r) );
      PUSHREF( bdd_correctify(level, HIGH(l), r) );
      res = bdd_makenode(LEVEL(l), READREF(2), READREF(1));
   }
   else
   {
      PUSHREF( bdd_correctify(level, l, LOW(r)) );
      PUSHREF( bdd_correctify(level, l, HIGH(r)) );
      res = bdd_makenode(LEVEL(r), READREF(2), READREF(1));
   }
   POPREF(2);

   return res; /* FIXME: cache ? */
}


/*=== COMPOSE ==========================================================*/

/*
NAME    {* bdd\_compose *}
SECTION {* operator *}
SHORT   {* functional composition *}
PROTO   {* BDD bdd_compose(BDD f, BDD g, int var) *}
DESCR   {* Substitutes the variable {\tt var} with the BDD {\tt g} in
	   the BDD {\tt f}: result $= f[g/var]$. *}
RETURN  {* The composed BDD *}
ALSO    {* bdd\_veccompose, bdd\_replace, bdd\_restrict *}
*/
BDD bdd_compose(BDD f, BDD g, int var)
{
   BDD res;
   firstReorder = 1;

   CHECKa(f, bddfalse);
   CHECKa(g, bddfalse);

#ifndef NDEBUG
   if (var < 0 || var >= bddvarnum)
   {
      bdd_error(BDD_VAR);
      return bddfalse;
   }
#endif

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE) ||
       setjmp(bddexception) == 0)
   {
      INITREF;
      composelevel = bddvar2level[var];
      replaceid = (composelevel << 2) | CACHEID_COMPOSE;

      if (__likely(firstReorder))
	{
	  res = compose_rec(f, g);
	}
      else
	{
	  bdd_disable_reorder();
	  res = compose_rec(f, g);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}

static BDD compose_rec(BDD l, BDD r)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;
   do
     {
       index = POPINT_();
       if (index < 0)
         {
           l = POPINT_();
           r = POPINT_();
         work:;
           int lvl_l = LEVEL(l);
           if (lvl_l > composelevel)
             RETURN(l);

           BddCacheData *entry2 =
             BddCache_index(&replacecache, COMPOSEHASH(l, r), index);
           if (entry2->i.a == l && entry2->i.b == r && entry2->i.c == replaceid)
             {
#ifdef CACHESTATS
               bddcachestats.opHit++;
#endif
               /* C: -1 l r  --- */
               /* R: --- res */
               RETURN(entry2->i.res);
             }
#ifdef CACHESTATS
           bddcachestats.opMiss++;
#endif
           if (lvl_l < composelevel)
             {
               /* C: -1 l r --- (-1 ll rl) -1 lr rr index lvl l r */
               int lvl_r = LEVEL(r);
               if (lvl_l == lvl_r)
                 {
                   PUSH4INT_(r, l, lvl_l, index);
                   PUSH3INT_(HIGH(r), HIGH(l), -1);
                   r = LOW(r);
                   l = LOW(l);
                 }
               else if (lvl_l < lvl_r)
                 {
                   PUSH4INT_(r, l, lvl_l, index);
                   PUSH3INT_(r, HIGH(l), -1);
                   l = LOW(l);
                 }
               else /* (lvl_l > lvl_r) */
                 {
                   PUSH4INT_(r, l, lvl_r, index);
                   PUSH3INT_(HIGH(r), l, -1);
                   r = LOW(r);
                 }
               goto work;
             }
           else
             {
               /* C: -1 l r ---      */
               /* R:        ---  res */
               SYNC_REC_STACKS;
               BDD res = ite_rec(r, HIGH(l), LOW(l));
               PUSHREF_(res);
               entry2->i.a = l;
               entry2->i.c = replaceid;
               entry2->i.b = r;
               entry2->i.res = res;
             }
         }
       else
         {
           /* C: index lvl l r ---     */
           /* R: rres lres     --- res */
           /* res=(lvl, lres, rres) is the result of applyop(l,r) */
           /* and it should be stored in *entry.                     */
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           int lvl = POPINT_();
           BDD l = POPINT_();
           BDD r = POPINT_();
           SYNC_REC_STACKS;
           BDD res = bdd_makenode(lvl, lres, rres);
           POPREF_(2);
           BddCacheData* entry = replacecache.table + index;
           entry->i.a = l;
           entry->i.c = replaceid;
           entry->i.b = r;
           entry->i.res = res;
           PUSHREF_(res);
         }
     }
   while (NONEMPTY_REC_STACK);

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/*
NAME    {* bdd\_veccompose *}
SECTION {* operator *}
SHORT   {* simultaneous functional composition *}
PROTO   {* BDD bdd_veccompose(BDD f, bddPair *pair) *}
DESCR   {* Uses the pairs of variables and BDDs in {\tt pair} to make
	   the simultaneous substitution: $f[g_1/V_1, \ldots, g_n/V_n]$.
	   In this way one or more BDDs
	   may be substituted in one step. The BDDs in
	   {\tt pair} may depend on the variables they are substituting.
	   {\tt bdd\_compose} may be used instead of
	   {\tt bdd\_replace} but is not as efficient when $g_i$ is a
	   single variable, the same applies to {\tt bdd\_restrict}.
	   Note that simultaneous substitution is not necessarily the same
	   as repeated substitution. Example:
	   $(x_1 \disj x_2)[x_3/x_1,x_4/x_3] = (x_3 \disj x_2) \neq
	   ((x_1 \disj x_2)[x_3/x_1])[x_4/x_3] = (x_4 \disj x_2)$. *}
RETURN  {* The composed BDD *}
ALSO    {* bdd\_compose, bdd\_replace, bdd\_restrict *}
*/
BDD bdd_veccompose(BDD f, bddPair *pair)
{
   BDD res;
   firstReorder = 1;

   CHECKa(f, bddfalse);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE) ||
       setjmp(bddexception) == 0)
   {
      INITREF;
      replacepair = pair->result;
      replaceid = (pair->id << 2) | CACHEID_VECCOMPOSE;
      replacelast = pair->last;

      if (__likely(firstReorder))
	{
	  res = veccompose_rec(f);
	}
      else
	{
	  bdd_disable_reorder();
	  res = veccompose_rec(f);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}


static BDD veccompose_rec(BDD f)
{
   BddCacheData *entry;
   register BDD res;

   if (LEVEL(f) > replacelast)
      return f;

   entry = BddCache_lookup(&replacecache, VECCOMPOSEHASH(f));
   if (entry->i.a == f  &&  entry->i.c == replaceid)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

   PUSHREF( veccompose_rec(LOW(f)) );
   PUSHREF( veccompose_rec(HIGH(f)) );
   res = ite_rec(replacepair[LEVEL(f)], READREF(1), READREF(2));
   POPREF(2);

   entry->i.a = f;
   entry->i.c = replaceid;
   entry->i.res = res;

   return res;
}


/*=== SIMPLIFY =========================================================*/

/*
NAME    {* bdd\_simplify *}
SECTION {* operator *}
SHORT   {* coudert and Madre's restrict function *}
PROTO   {* BDD bdd_simplify(BDD f, BDD d) *}
DESCR   {* Tries to simplify the BDD {\tt f} by restricting it to the
	   domain covered by {\tt d}. No checks are done to see if the
	   result is actually smaller than the input. This can be done
	   by the user with a call to {\tt bdd\_nodecount}. *}
ALSO    {* bdd\_restrict *}
RETURN  {* The simplified BDD *}
*/
BDD bdd_simplify(BDD f, BDD d)
{
   BDD res;
   firstReorder = 1;

   CHECKa(f, bddfalse);
   CHECKa(d, bddfalse);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      INITREF;
      applyop = bddop_or;

      if (__likely(firstReorder))
	{
	  res = simplify_rec(f, d);
	}
      else
	{
	  bdd_disable_reorder();
	  res = simplify_rec(f, d);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}


static BDD simplify_rec(BDD f, BDD d)
{
   BddCacheData *entry;
   BDD res;

   if (ISONE(d)  ||  ISCONST(f))
      return f;
   if (d == f)
      return BDDONE;
   if (ISZERO(d))
      return BDDZERO;

   entry = BddCache_lookup(&applycache, APPLYHASH(f,d,bddop_simplify));

   /* Check entry->b last, because not_rec() does not initialize it. */
   if (entry->i.a == f && entry->i.c == bddop_simplify && entry->i.b == d)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

   if (LEVEL(f) == LEVEL(d))
   {
      if (ISZERO(LOW(d)))
	 res = simplify_rec(HIGH(f), HIGH(d));
      else
      if (ISZERO(HIGH(d)))
	 res = simplify_rec(LOW(f), LOW(d));
      else
      {
	 PUSHREF( simplify_rec(LOW(f),	LOW(d)) );
	 PUSHREF( simplify_rec(HIGH(f), HIGH(d)) );
	 res = bdd_makenode(LEVEL(f), READREF(2), READREF(1));
	 POPREF(2);
      }
   }
   else
   if (LEVEL(f) < LEVEL(d))
   {
      PUSHREF( simplify_rec(LOW(f), d) );
      PUSHREF( simplify_rec(HIGH(f), d) );
      res = bdd_makenode(LEVEL(f), READREF(2), READREF(1));
      POPREF(2);
   }
   else /* LEVEL(d) < LEVEL(f) */
   {
      PUSHREF( apply_rec(LOW(d), HIGH(d)) ); /* Exist quant */
      res = simplify_rec(f, READREF(1));
      POPREF(1);
   }

   entry->i.a = f;
   entry->i.c = bddop_simplify;
   entry->i.b = d;
   entry->i.res = res;

   return res;
}


/*=== QUANTIFICATION ===================================================*/

static BDD quantify(BDD r, BDD var, int op, int comp, int id)
{
   BDD res;
   firstReorder = 1;

   CHECKa(r, bddfalse);
   CHECKa(var, bddfalse);

   if (var < 2 && !comp)  /* Empty set */
      return r;

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      if (varset2vartable(var, comp) < 0)
	 return bddfalse;

      INITREF;
      quantid = (var << 4) | id; /* FIXME: range */
      applyop = op;

      if (__likely(firstReorder))
	{
	  res = quant_rec(r);
	}
      else
	{
	  bdd_disable_reorder();
	  res = quant_rec(r);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}

/*
NAME    {* bdd\_exist *}
SECTION {* operator *}
SHORT   {* existential quantification of variables *}
PROTO   {* BDD bdd_exist(BDD r, BDD var) *}
DESCR   {* Removes all occurences in {\tt r} of variables in the set
	   {\tt var} by existential quantification. *}
ALSO    {* bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The quantified BDD. *}
*/
BDD bdd_exist(BDD r, BDD var)
{
   return quantify(r, var, bddop_or, 0, CACHEID_EXIST);
}

/*
NAME    {* bdd\_existcomp *}
SECTION {* operator *}
SHORT   {* existential quantification of other variables *}
PROTO   {* BDD bdd_existcomp(BDD r, BDD var) *}
DESCR   {* Removes all occurences in {\tt r} of variables {\bf not} in the set
	   {\tt var} by existential quantification. *}
ALSO    {* bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The quantified BDD. *}
*/
BDD bdd_existcomp(BDD r, BDD var)
{
   return quantify(r, var, bddop_or, 1, CACHEID_EXISTC);
}


/*
NAME    {* bdd\_forall *}
SECTION {* operator *}
SHORT   {* universal quantification of variables *}
PROTO   {* BDD bdd_forall(BDD r, BDD var) *}
DESCR   {* Removes all occurences in {\tt r} of variables in the set
	   {\tt var} by universal quantification. *}
ALSO    {* bdd\_exist, bdd\_existcomp, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The quantified BDD. *}
*/
BDD bdd_forall(BDD r, BDD var)
{
   return quantify(r, var, bddop_and, 0, CACHEID_FORALL);
}

/*
NAME    {* bdd\_forallcomp *}
SECTION {* operator *}
SHORT   {* universal quantification of other variables *}
PROTO   {* BDD bdd_forallcomp(BDD r, BDD var) *}
DESCR   {* Removes all occurences in {\tt r} of variables {\bf not} in the set
	   {\tt var} by universal quantification. *}
ALSO    {* bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The quantified BDD. *}
*/
BDD bdd_forallcomp(BDD r, BDD var)
{
   return quantify(r, var, bddop_and, 1, CACHEID_FORALLC);
}


/*
NAME    {* bdd\_unique *}
SECTION {* operator *}
SHORT   {* unique quantification of variables *}
PROTO   {* BDD bdd_unique(BDD r, BDD var) *}
DESCR   {* Removes all occurences in {\tt r} of variables in the set
	   {\tt var} by unique quantification. This type of quantification
	   uses a XOR operator instead of an OR operator as in the
	   existential quantification, and an AND operator as in the
	   universal quantification. *}
ALSO    {* bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The quantified BDD. *}
*/
BDD bdd_unique(BDD r, BDD var)
{
   return quantify(r, var, bddop_xor, 0, CACHEID_UNIQUE);
}

/*
NAME    {* bdd\_uniquecomp *}
SECTION {* operator *}
SHORT   {* unique quantification of other variables *}
PROTO   {* BDD bdd_uniquecomp(BDD r, BDD var) *}
DESCR   {* Removes all occurences in {\tt r} of variables now {\bf not} in
           the set {\tt var} by unique quantification.  *}
ALSO    {* bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The quantified BDD. *}
*/
BDD bdd_uniquecomp(BDD r, BDD var)
{
   return quantify(r, var, bddop_xor, 1, CACHEID_UNIQUEC);
}


static int quant_rec(BDD r)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;
   do
     {
       index = POPINT_();
       if (index < 0)
         {
           r = POPINT_();
         work:;
           /* C: -1 r ---   */
           /* R:      --- r */
           if (ISCONST(r) || LEVEL(r) > quantlast)
             {
               PUSHREF_(r);
             }
           else
             {
               BddCacheData *entry2 =
                 BddCache_index(&quantcache, QUANTHASH(r), index);
               if (entry2->i.a == r && entry2->i.c == quantid)
                 {
#ifdef CACHESTATS
                   bddcachestats.opHit++;
#endif
                   /* C: -1 r ---     */
                   /* R:      --- res */
                   PUSHREF_(entry2->i.res);
                 }
               else
                 {
#ifdef CACHESTATS
                   bddcachestats.opMiss++;
#endif
                   /* C: -1 r --- (-1 lr) -1 rr index r */
                   PUSH4INT_(r, index, HIGH(r), -1)
                   r = LOW(r);
                   goto work;
                 }
             }
         }
       else
         {
           /* C: index r ---     */
           /* R: rres lres --- res */
           BDD r = POPINT_();
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           BDD res;
           int lvl = LEVEL(r);
           SYNC_REC_STACKS;
           if (INVARSET(lvl))
             res = apply_rec(lres, rres);
           else
             res = bdd_makenode(lvl, lres, rres);
           POPREF_(2);
           PUSHREF_(res);
           BddCacheData* entry = quantcache.table + index;
           entry->i.a = r;
           entry->i.c = quantid;
           entry->i.res = res;
         }
     }
   while (NONEMPTY_REC_STACK);

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/*=== APPLY & QUANTIFY =================================================*/

static BDD appquantify(BDD l, BDD r, int opr, BDD var,
		       int qop, int comp, int qid)
{
   BDD res;
   firstReorder = 1;

   CHECKa(l, bddfalse);
   CHECKa(r, bddfalse);
   CHECKa(var, bddfalse);

#ifndef NDEBUG
   if (opr<0 || opr>bddop_invimp)
   {
      bdd_error(BDD_OP);
      return bddfalse;
   }
#endif

   if (var < 2 && !comp)  /* Empty set */
      return bdd_apply(l,r,opr);

 again:
   if (__likely(bddreordermethod == BDD_REORDER_NONE)
       || setjmp(bddexception) == 0)
   {
      if (varset2vartable(var, comp) < 0)
	 return bddfalse;

      INITREF;
      applyop = qop;
      appexop = opr;
      appexid = (var << 5) | (appexop << 1); /* FIXME: range! */
      quantid = (appexid << 4) | qid;

      if (__likely(firstReorder))
	{
	  res = appquant_rec(l, r);
	}
      else
	{
	  bdd_disable_reorder();
	  res = appquant_rec(l, r);
	  bdd_enable_reorder();
	}
   }
   else
   {
      bdd_checkreorder();

      if (firstReorder-- == 1)
	 goto again;
      res = BDDZERO;  /* avoid warning about res being uninitialized */
   }

   checkresize();
   return res;
}

/*
NAME    {* bdd\_appex *}
SECTION {* operator *}
SHORT   {* apply operation and existential quantification *}
PROTO   {* BDD bdd_appex(BDD left, BDD right, int opr, BDD var) *}
DESCR   {* Applies the binary operator {\tt opr} to the arguments
	   {\tt left} and {\tt right} and then performs an existential
	   quantification of the variables from the variable set
	   {\tt var}. This is done in a bottom up manner such that both the
	   apply and quantification is done on the lower nodes before
	   stepping up to the higher nodes. This makes the {\tt bdd\_appex}
	   function much more efficient than an apply operation followed
	   by a quantification. If the operator is a conjunction then this
	   is similar to the relational product of the two BDDs.
	   \index{relational product} *}
ALSO    {* bdd\_appexcomp, bdd\_appall, bdd\_appallcomp, bdd\_appuni, bdd\_appunicomp, bdd\_apply, bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_appex(BDD l, BDD r, int opr, BDD var)
{
   return appquantify(l, r, opr, var, bddop_or, 0, CACHEID_APPEX);
}


/*
NAME    {* bdd\_appexcomp *}
SECTION {* operator *}
SHORT   {* apply operation and existential (complemented) quantification *}
PROTO   {* BDD bdd_appexcomp(BDD left, BDD right, int opr, BDD var) *}
DESCR   {* Applies the binary operator {\tt opr} to the arguments
	   {\tt left} and {\tt right} and then performs an existential
	   quantification of the variables which are {\bf not} in the variable
	   set {\tt var}. This is done in a bottom up manner such that both the
	   apply and quantification is done on the lower nodes before
	   stepping up to the higher nodes. This makes the {\tt bdd\_appexcomp}
	   function much more efficient than an apply operation followed
	   by a quantification.  *}
ALSO    {* bdd\_appex, bdd\_appall, bdd\_appallcomp, bdd\_appuni, bdd\_appunicomp, bdd\_apply, bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_appexcomp(BDD l, BDD r, int opr, BDD var)
{
   return appquantify(l, r, opr, var, bddop_or, 1, CACHEID_APPEXC);
}


/*
NAME    {* bdd\_appall *}
SECTION {* operator *}
SHORT   {* apply operation and universal quantification *}
PROTO   {* BDD bdd_appall(BDD left, BDD right, int opr, BDD var) *}
DESCR   {* Applies the binary operator {\tt opr} to the arguments
	   {\tt left} and {\tt right} and then performs a universal
	   quantification of the variables from the variable set
	   {\tt var}. This is done in a bottom up manner such that both the
	   apply and quantification is done on the lower nodes before
	   stepping up to the higher nodes. This makes the {\tt bdd\_appall}
	   function much more efficient than an apply operation followed
	   by a quantification. *}
ALSO    {* bdd\_appex, bdd\_appexcomp, bdd\_appallcomp, bdd\_appuni, bdd\_appunicomp, bdd\_apply, bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_appall(BDD l, BDD r, int opr, BDD var)
{
   return appquantify(l, r, opr, var, bddop_and, 0, CACHEID_APPAL);
}


/*
NAME    {* bdd\_appallcomp *}
SECTION {* operator *}
SHORT   {* apply operation and universal (complemented) quantification *}
PROTO   {* BDD bdd_appall(BDD left, BDD right, int opr, BDD var) *}
DESCR   {* Applies the binary operator {\tt opr} to the arguments
	   {\tt left} and {\tt right} and then performs a universal
	   quantification of the variables which are {\bf not} in the variable
	   set {\tt var}. This is done in a bottom up manner such that both the
	   apply and quantification is done on the lower nodes before
	   stepping up to the higher nodes. This makes the
	   {\tt bdd\_appallcomp} function much more efficient than an
	   apply operation followed by a quantification. *}
ALSO    {* bdd\_appex, bdd\_appexcomp, bdd\_appall, bdd\_appuni, bdd\_appunicomp, bdd\_apply, bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_appallcomp(BDD l, BDD r, int opr, BDD var)
{
   return appquantify(l, r, opr, var, bddop_and, 1, CACHEID_APPALC);
}


/*
NAME    {* bdd\_appuni *}
SECTION {* operator *}
SHORT   {* apply operation and unique quantification *}
PROTO   {* BDD bdd_appuni(BDD left, BDD right, int opr, BDD var) *}
DESCR   {* Applies the binary operator {\tt opr} to the arguments
	   {\tt left} and {\tt right} and then performs a unique
	   quantification of the variables from the variable set
	   {\tt var}. This is done in a bottom up manner such that both the
	   apply and quantification is done on the lower nodes before
	   stepping up to the higher nodes. This makes the {\tt bdd\_appuni}
	   function much more efficient than an apply operation followed
	   by a quantification. *}
ALSO    {* bdd\_appex, bdd\_appexcomp, bdd\_appall, bdd\_appallcomp, bdd\_appuni, bdd\_appunicomp, bdd\_apply, bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_appuni(BDD l, BDD r, int opr, BDD var)
{
   return appquantify(l, r, opr, var, bddop_xor, 0, CACHEID_APPUN);
}


/*
NAME    {* bdd\_appunicomp *}
SECTION {* operator *}
SHORT   {* apply operation and unique (complemented) quantification *}
PROTO   {* BDD bdd_appunicomp(BDD left, BDD right, int opr, BDD var) *}
DESCR   {* Applies the binary operator {\tt opr} to the arguments
	   {\tt left} and {\tt right} and then performs a unique
	   quantification of the variables which are {\bf not} in the variable
	   set {\tt var}. This is done in a bottom up manner such that both the
	   apply and quantification is done on the lower nodes before
	   stepping up to the higher nodes. This makes the
	   {\tt bdd\_appunicomp} function much more efficient than an
	   apply operation followed by a quantification. *}
ALSO    {* bdd\_appex, bdd\_appexcomp, bdd\_appall, bdd\_appallcomp, bdd\_appuni, bdd\_apply, bdd\_exist, bdd\_existcomp, bdd\_forall, bdd\_forallcomp, bdd\_unique, bdd\_uniquecomp, bdd\_makeset *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_appunicomp(BDD l, BDD r, int opr, BDD var)
{
   return appquantify(l, r, opr, var, bddop_xor, 1, CACHEID_APPUNC);
}


static BDD appquant_rec(BDD l, BDD r)
{
   LOCAL_REC_STACKS;
   int index;

   goto work;
   do
     {
       index = POPINT_();
       if (index < 0)
         {
           l = POPINT_();
           r = POPINT_();
         work:
           /* C: -1 l r ---    */
           /* R:        --- res */
           if (ISCONST(l) && ISCONST(r))
             RETURN(oprres[appexop][l<<1 | r]);

           if (LEVEL(l) > quantlast && LEVEL(r) > quantlast)
             {
               int oldop = applyop;
               applyop = appexop;
               SYNC_REC_STACKS;
               int res = apply_rec(l,r);
               PUSHREF_(res);
               applyop = oldop;
               continue;
             }

           /* empty macro arguments are undefined in ISO C90 and
              ISO C++98, so use + when we do not want to call any
              function. */
           SYNC_REC_STACKS;
           APPLY_SHORTCUTS(appexop, quant_rec);

           BddCacheData *entry2 =
             BddCache_index(&appexcache, APPEXHASH(l,r,appexop), index);
           /* Check entry2->b last, because not_rec() does not
              initialize it. */
           if (entry2->i.a == l  && entry2->i.c == appexid
               && entry2->i.b == r)
             {
#ifdef CACHESTATS
               bddcachestats.opHit++;
#endif
               /* C: -1 l r ---     */
               /* R:        --- res */
               RETURN(entry2->i.res);
             }
#ifdef CACHESTATS
           bddcachestats.opMiss++;
#endif
           /* C: -1 l r --- (-1 ll rl) -1 lr rr index lvl l r */
           int lvl_l = LEVEL(l);
           int lvl_r = LEVEL(r);
           if (lvl_l == lvl_r)
             {
               PUSH4INT_(r, l, lvl_l, index);
               PUSH3INT_(HIGH(r), HIGH(l), -1);
               r = LOW(r);
               l = LOW(l);
             }
           else if (lvl_l < lvl_r)
             {
               PUSH4INT_(r, l, lvl_l, index);
               PUSH3INT_(r, HIGH(l), -1);
               l = LOW(l);
             }
           else /* lvl_l > lvl_r */
             {
               PUSH4INT_(r, l, lvl_r, index);
               PUSH3INT_(HIGH(r), l, -1);
               r = LOW(r);
             }
           goto work;
         }
       else
         {
           /* C: index lvl l r ---     */
           /* R: rres lres     --- res */
           BDD rres = READREF_(1);
           BDD lres = READREF_(2);
           int lvl = POPINT_();
           BDD l = POPINT_();
           BDD r = POPINT_();
           BDD res;
           SYNC_REC_STACKS;
           if (INVARSET(lvl))
             res = apply_rec(lres, rres);
           else
             res = bdd_makenode(lvl, lres, rres);
           POPREF_(2);
           PUSHREF_(res);
           BddCacheData* entry = appexcache.table + index;
           entry->i.a = l;
           entry->i.c = appexid;
           entry->i.b = r;
           entry->i.res = res;
         }
     }
   while (NONEMPTY_REC_STACK);

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/*************************************************************************
  Informational functions
*************************************************************************/

/*=== SUPPORT ==========================================================*/

/*
NAME    {* bdd\_support *}
SECTION {* info *}
SHORT   {* returns the variable support of a BDD *}
PROTO   {* BDD bdd_support(BDD r) *}
DESCR   {* Finds all the variables that {\tt r} depends on. That is
	   the support of {\tt r}. *}
ALSO    {* bdd\_makeset *}
RETURN  {* A BDD variable set. *}
*/
BDD bdd_support(BDD r)
{
   BddCacheData *entry;
   static int  supportSize = 0;
   int n;
   int res=1;

   CHECKa(r, bddfalse);

   /* Variable sets are conjunctions, so the empty support is bddtrue.  */
   if (r < 2)
      return bddtrue;

   entry = BddCache_lookup(&misccache, SUPPORTHASH(r));
   if (entry->i.a == r && entry->i.c == CACHEID_SUPPORT)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

      /* On-demand allocation of support set */
   if (__unlikely(supportSize < bddvarnum))
   {
     if (__likely(supportSet))
       free(supportSet);
     if (__unlikely((supportSet=(int*)malloc(bddvarnum*sizeof(int))) == NULL))
     {
       bdd_error(BDD_MEMORY);
       return bddfalse;
     }
     memset(supportSet, 0, bddvarnum*sizeof(int));
     supportSize = bddvarnum;
     supportID = 0;
   }

      /* Update global variables used to speed up bdd_support()
       * - instead of always memsetting support to zero, we use
       *   a change counter.
       * - and instead of reading the whole array afterwards, we just
       *   look from 'min' to 'max' used BDD variables.
       */
   if (__unlikely(supportID == 0x0FFFFFFF))
   {
	/* We probably don't get here -- but let's just be sure */
     memset(supportSet, 0, bddvarnum*sizeof(int));
     supportID = 0;
   }
   ++supportID;
   supportMin = LEVEL(r);
   supportMax = supportMin;

   support_rec(r, supportSet);
   bdd_unmark(r);

   bdd_disable_reorder();

   for (n=supportMax ; n>=supportMin ; --n)
      if (supportSet[n] == supportID)
      {
	 register BDD tmp;
	 bdd_addref(res);
	 tmp = bdd_makenode(n, 0, res);
	 bdd_delref(res);
	 res = tmp;
      }

   bdd_enable_reorder();

   entry->i.a = r;
   entry->i.c = CACHEID_SUPPORT;
   entry->i.res = res;

   return res;
}


static void support_rec(int r, int* support)
{
  // Reuse bddrefstack as temporary stack
  int* bot = bddrefstacktop;
  int* top = bot;
  goto work;
  while (top > bot)
    {
      r = *--top;
    work:
      if (r < 2)
        continue;
      BddNode *node = &bddnodes[r];
      if (LEVELp(node) & MARKON || LOWp(node) == -1)
        continue;
      support[LEVELp(node)] = supportID;
      if (LEVELp(node) > supportMax)
        supportMax = LEVELp(node);
      LEVELp(node) |= MARKON;
      *top++ = LOWp(node);
      r = HIGHp(node);
      goto work;
    }
}


/*=== ONE SATISFYING VARIABLE ASSIGNMENT ===============================*/

/*
NAME    {* bdd\_satone *}
SECTION {* operator *}
SHORT   {* finds one satisfying variable assignment *}
PROTO   {* BDD bdd_satone(BDD r) *}
DESCR   {* Finds a BDD with at most one variable at each level. This BDD
	   implies {\tt r} and is not false unless {\tt r} is
	   false. *}
ALSO    {* bdd\_allsat, bdd\_satoneset, bdd\_satprefix, bdd\_fullsatone, bdd\_satcount, bdd\_satcountln *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_satone(BDD r)
{
   BDD res;

   CHECKa(r, bddfalse);
   if (r < 2)
      return r;

   bdd_disable_reorder();

   INITREF;
   res = satone_rec(r);

   bdd_enable_reorder();

   checkresize();
   return res;
}


static BDD satone_rec(BDD r)
{
   LOCAL_REC_STACKS;

   while (ISNONCONST(r))
     {
       BDD low = LOW(r);
       int iszero = ISZERO(low);
       int lvl = LEVEL(r);
       if (iszero)
         {
           PUSHINT_(lvl);
           r = HIGH(r);
         }
       else
         {
           PUSHINT_(~lvl);
           r = low;
         }
     }
   PUSHREF_(r);
   while (NONEMPTY_REC_STACK)
     {
       /* C: lvl --- */
       /* R: r   --- res */
       BDD r = READREF_(1);
       int lvl = POPINT_();
       BDD res;
       SYNC_REC_STACKS;
       if (lvl >= 0)
         res = bdd_makenode(lvl, BDDZERO, r);
       else
         res = bdd_makenode(~lvl, r, BDDZERO);
       POPREF_(1);
       PUSHREF_(res);
     }

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/*
NAME    {* bdd\_satprefix *}
SECTION {* operator *}
SHORT   {* quickly remove a conjunction common to all satisfying assignments *}
PROTO   {* BDD bdd_satprefix(BDD* r) *}
DESCR   {* Recursively descend into the top of the BDD and return the
           prefix common to all satisfying paths.  Adjust r to point to
	   the rest of the BDD. *}
ALSO    {* bdd\_allsat, bdd\_satone, bdd\_satoneset, bdd\_fullsatone, bdd\_satcount, bdd\_satcountln *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_satprefix(BDD* r)
{
   BDD res;

   CHECKa(*r, bddfalse);
   if (__unlikely(*r < 2))
      return *r;

   bdd_disable_reorder();

   INITREF;
   res = satprefix_rec(r);

   bdd_enable_reorder();

   checkresize();
   return res;

}

static BDD satprefix_rec(BDD* r)
{
   if (ISCONST(*r))
      return *r;

   if (ISZERO(LOW(*r)))
     {
       int lr = LEVEL(*r);
       *r = HIGH(*r);
       return PUSHREF(bdd_makenode(lr, BDDZERO, satprefix_rec(r)));
     }
   else if (ISZERO(HIGH(*r)))
     {
       int lr = LEVEL(*r);
       *r = LOW(*r);
       return PUSHREF(bdd_makenode(lr, satprefix_rec(r), BDDZERO));
     }
   else
     {
       return BDDONE;
     }
}

/*
NAME    {* bdd\_satoneset *}
SECTION {* operator *}
SHORT   {* finds one satisfying variable assignment *}
PROTO   {* BDD bdd_satoneset(BDD r, BDD var, BDD pol) *}
DESCR   {* Finds a minterm in {\tt r}. The {\tt var} argument is a
	   variable set that defines a set of variables that {\em must} be
	   mentioned in the result. The polarity of these variables in
	   result---in case they are undefined in {\tt r}---are defined
	   by the {\tt pol} parameter. If {\tt pol} is the false BDD then
	   the variables will be in negative form, and otherwise they will
	   be in positive form. *}
ALSO    {* bdd\_allsat, bdd\_satone, bdd\_satprefix, bdd\_fullsatone, bdd\_satcount, bdd\_satcountln *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_satoneset(BDD r, BDD var, BDD pol)
{
   BDD res;

   CHECKa(r, bddfalse);
   if (ISZERO(r))
      return r;
#ifndef NDEBUG
   if (!ISCONST(pol))
   {
      bdd_error(BDD_ILLBDD);
      return bddfalse;
   }
#endif
   bdd_disable_reorder();

   INITREF;
   satPolarity = pol;
   res = satoneset_rec(r, var);

   bdd_enable_reorder();

   checkresize();
   return res;
}

static BDD satoneset_rec(BDD r, BDD var)
{
   LOCAL_REC_STACKS;

   while (!(ISCONST(r) && ISCONST(var)))
     {
       int lvl_r = LEVEL(r);
       int lvl_v = LEVEL(var);
       if (lvl_r <= lvl_v)
         {
           int low_r = LOW(r);
           if (ISZERO(low_r))
             {
               PUSHINT_(lvl_r);
               r = HIGH(r);
             }
           else
             {
               PUSHINT_(~lvl_r);
               r = low_r;
             }
           if (lvl_r == lvl_v)
             var = HIGH(var);
         }
       else /* lvl_v < lvl_r */
         {
           if (satPolarity == BDDONE)
             PUSHINT_(lvl_v);
           else
             PUSHINT_(~lvl_v);
           var = HIGH(var);
         }
     }
   PUSHREF_(r);
   while (NONEMPTY_REC_STACK)
     {
       BDD r = READREF_(1);
       int lvl = POPINT_();
       SYNC_REC_STACKS;
       BDD res;
       if (lvl >= 0)
         res = bdd_makenode(lvl, BDDZERO, r);
       else
         res = bdd_makenode(~lvl, r, BDDZERO);
       POPREF_(1);
       PUSHREF_(res);
     }

   BDD res = READREF_(1);
   POPREF_(1);
   SYNC_REC_STACKS;
   return res;
}


/* Compute the smallest distance associated to a satisfying path of f,
   but without actually building the corresponding cube.  This is done
   by computing the distance recursively for all sub BDDs, and storing
   those in a hashtable, with the intention of sharing that across
   all calls of bdd_satoneshortest with the same weights.

   The hash table used for cache stores triples of the form (bdd, rev,
   dist) where dist is the distance for bdd, and rev is incremented
   everytime bdd_satoneshortest is called with a different set of
   weights.  One should therefore check that rev from the cache lookup
   matches the current rev stored in shortest_rev.  bdd_satoneshortest
   is in charge of incrementing shortest_rev.
 */
static unsigned wlow_ref = 0;
static unsigned whigh_ref = 0;
static unsigned wdc_ref = 0;
static int shortest_rev = 0;

/* This computes the shortest distance, recursively.
   Populating the cache. */
static unsigned satoneshortest_rec(BDD f)
{
  if (ISONE(f))
    return 0;
  if (ISZERO(f))
    return -1U;
  int rev = shortest_rev;
  BddCacheData *entry = BddCache_lookup(&misccache, SHORTDISTHASH(f,rev));
  if (entry->i.a == f
      && entry->i.b == rev
      && entry->i.c == CACHEID_SHORTDIST)
  {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return (unsigned) entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

   int lv_next = LEVEL(f) + 1;
   int low = LOW(f);
   int lv_low = LEVEL(low);
   unsigned dist_low = satoneshortest_rec(low);
   if (dist_low != -1U)
     dist_low += wlow_ref + wdc_ref * (lv_low - lv_next);
   int high = HIGH(f);
   int lv_high = LEVEL(high);
   unsigned dist_high = satoneshortest_rec(high);
   if (dist_high != -1U)
     dist_high += whigh_ref + wdc_ref * (lv_high - lv_next);

   if (dist_high < dist_low)
     dist_low = dist_high;

   entry->i.a = f;
   entry->i.b = rev;
   entry->i.c = CACHEID_SHORTDIST;
   entry->i.res = (int) dist_low;
   return dist_low;
}

/* This computes the BDD for the shortest distance, recursively.
   It calls satoneshortest_rec again, but this should be fast thanks to
   the cache.
*/
static bdd bdd_satoneshortest_rec(BDD f)
{
  if (ISONE(f))
    return bddtrue;
  if (ISZERO(f))
    return bddfalse;

  int rev = shortest_rev;
  BddCacheData *entry = BddCache_lookup(&misccache, SHORTBDDHASH(f,rev));
  if (entry->i.a == f
      && entry->i.b == rev
      && entry->i.c == CACHEID_SHORTDIST)
  {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

   int lv = LEVEL(f);
   int lv_next = lv + 1;
   int low = LOW(f);
   int lv_low = LEVEL(low);
   unsigned dist_low = satoneshortest_rec(low);
   if (dist_low != -1U)
     dist_low += wlow_ref + wdc_ref * (lv_low - lv_next);

   int high = HIGH(f);
   int lv_high = LEVEL(high);
   unsigned dist_high = satoneshortest_rec(high);
   if (dist_high != -1U)
     dist_high += whigh_ref + wdc_ref * (lv_high - lv_next);

   bdd res;
   if (dist_high < dist_low)
     {
       res = bdd_satoneshortest_rec(high);
       res = PUSHREF(bdd_makenode(lv, 0, res));
     }
   else
     {
       res = bdd_satoneshortest_rec(low);
       res = PUSHREF(bdd_makenode(lv, res, 0));
     }

   entry->i.a = f;
   entry->i.b = rev;
   entry->i.c = CACHEID_SHORTBDD;
   entry->i.res = res;
   return res;
}

/*
 return a conjunction representing a satisfying path of f, chosen
 among all satisfying paths to minimize the sum of weights given for
 positive variables (whigh), negative variables (wlow), and don't care
 variables (wdc = Weight of Don't Care).
*/
BDD bdd_satoneshortest(BDD f, unsigned wlow, unsigned whigh, unsigned wdc)
{
  if (wlow != wlow_ref || whigh != whigh_ref || wdc != wdc_ref)
    {
      wlow_ref = wlow;
      whigh_ref = whigh;
      wdc_ref = wdc;
      ++shortest_rev;
    }
  bdd_disable_reorder();
  INITREF;
  bdd res = bdd_satoneshortest_rec(f);
  bdd_enable_reorder();
  checkresize();
  return res;
}



/*=== EXACTLY ONE SATISFYING VARIABLE ASSIGNMENT =======================*/

/*
NAME    {* bdd\_fullsatone *}
SECTION {* operator *}
SHORT   {* finds one satisfying variable assignment *}
PROTO   {* BDD bdd_fullsatone(BDD r) *}
DESCR   {* Finds a BDD with exactly one variable at all levels. This BDD
	   implies {\tt r} and is not false unless {\tt r} is
	   false. *}
ALSO    {* bdd\_allsat, bdd\_satone, bdd\_satprefix, bdd\_satoneset, bdd\_satcount, bdd\_satcountln *}
RETURN  {* The result of the operation. *}
*/
BDD bdd_fullsatone(BDD r)
{
   BDD res;
   int v;

   CHECKa(r, bddfalse);
   if (r == 0)
      return 0;

   bdd_disable_reorder();

   INITREF;
   res = fullsatone_rec(r);

   for (v=LEVEL(r)-1 ; v>=0 ; v--)
   {
      res = PUSHREF( bdd_makenode(v, res, 0) );
   }

   bdd_enable_reorder();

   checkresize();
   return res;
}


static int fullsatone_rec(int r)
{
   if (r < 2)
      return r;

   if (LOW(r) != 0)
   {
      int res = fullsatone_rec(LOW(r));
      int v;

      for (v=LEVEL(LOW(r))-1 ; v>LEVEL(r) ; v--)
      {
	 res = PUSHREF( bdd_makenode(v, res, 0) );
      }

      return PUSHREF( bdd_makenode(LEVEL(r), res, 0) );
   }
   else
   {
      int res = fullsatone_rec(HIGH(r));
      int v;

      for (v=LEVEL(HIGH(r))-1 ; v>LEVEL(r) ; v--)
      {
	 res = PUSHREF( bdd_makenode(v, res, 0) );
      }

      return PUSHREF( bdd_makenode(LEVEL(r), 0, res) );
   }
}


/*=== ALL SATISFYING VARIABLE ASSIGNMENTS ==============================*/

/*
NAME    {* bdd\_allsat *}
SECTION {* operator *}
SHORT   {* finds all satisfying variable assignments *}
PROTO   {* BDD bdd_allsat(BDD r, bddallsathandler handler) *}
DESCR   {* Iterates through all legal variable assignments (those
	   that make the BDD come true) for the  bdd {\tt r} and
	   calls the callback handler {\tt handler} for each of them.
	   The array passed to {\tt handler} contains one entry for
	   each of the globally defined variables. Each entry is either
	   0 if the variable is false, 1 if it is true, and -1 if it
	   is a don't care.

	   The following is an example of a callback handler that
	   prints 'X' for don't cares, '0' for zero, and '1' for one:
	   \begin{verbatim}
void allsatPrintHandler(signed char* varset, int size)
{
  for (int v=0; v<size ; ++v)
  {
    cout << (varset[v] < 0 ? 'X' : (char)('0' + varset[v]));
  }
  cout << endl;
}
\end{verbatim}

	   \noindent
	   The handler can be used like this:
	   {\tt bdd\_allsat(r, allsatPrintHandler); } *}

ALSO    {* bdd\_satone bdd\_satoneset, bdd\_fullsatone, bdd\_satcount, bdd\_satcountln *}
*/
void bdd_allsat(BDD r, bddallsathandler handler)
{
   int v;

   CHECKn(r);

   if (__unlikely((allsatProfile=(signed char*)malloc(bddvarnum)) == NULL))
   {
      bdd_error(BDD_MEMORY);
      return;
   }

   for (v=LEVEL(r)-1 ; v>=0 ; --v)
     allsatProfile[bddlevel2var[v]] = -1;

   allsatHandler = handler;
   INITREF;

   allsat_rec(r);

   free(allsatProfile);
}


static void allsat_rec(BDD r)
{
   if (ISONE(r))
   {
      allsatHandler(allsatProfile, bddvarnum);
      return;
   }

   if (ISZERO(r))
      return;

   if (!ISZERO(LOW(r)))
   {
      int v;

      allsatProfile[bddlevel2var[LEVEL(r)]] = 0;

      for (v=LEVEL(LOW(r))-1 ; v>LEVEL(r) ; --v)
      {
	 allsatProfile[bddlevel2var[v]] = -1;
      }

      allsat_rec(LOW(r));
   }

   if (!ISZERO(HIGH(r)))
   {
      int v;

      allsatProfile[bddlevel2var[LEVEL(r)]] = 1;

      for (v=LEVEL(HIGH(r))-1 ; v>LEVEL(r) ; --v)
      {
	 allsatProfile[bddlevel2var[v]] = -1;
      }

      allsat_rec(HIGH(r));
   }
}


/*=== COUNT NUMBER OF SATISFYING ASSIGNMENT ============================*/

/*
NAME    {* bdd\_satcount *}
EXTRA   {* bdd\_setcountset *}
SECTION {* info *}
SHORT   {* calculates the number of satisfying variable assignments *}
PROTO   {* double bdd_satcount(BDD r)
double bdd_satcountset(BDD r, BDD varset) *}
DESCR   {* Calculates how many possible variable assignments there exists
	   such that {\tt r} is satisfied (true). All defined
	   variables are considered in the first version. In the
	   second version, only the variables in the variable
	   set {\tt varset} are considered. This makes the function a
	   {\em lot} slower. *}
ALSO    {* bdd\_satone, bdd\_fullsatone, bdd\_satcountln *}
RETURN  {* The number of possible assignments. *}
*/
double bdd_satcount(BDD r)
{
   double size=1;

   CHECKa(r, 0.0);

   // Invalidate misccache if the number of variable changed since we
   // last used it.
   if (misccache_varnum != bddvarnum)
     {
       BddCache_reset(&misccache);
       misccache_varnum = bddvarnum;
     }

   miscid = CACHEID_SATCOU;
   size = pow(2.0, (double)LEVEL(r));

   return size * satcount_rec(r);
}


double bdd_satcountset(BDD r, BDD varset)
{
   double unused = bddvarnum;
   BDD n;

   if (ISZERO(r))
     return 0.0;
   if (ISCONST(varset)) /* empty set */
     return ISONE(r) ? 1.0 : 0.0;

   for (n=varset ; !ISCONST(n) ; n=HIGH(n))
      unused--;

   unused = bdd_satcount(r) / pow(2.0,unused);

   return unused >= 1.0 ? unused : 1.0;
}


static double satcount_rec(int root)
{
   BddCacheData *entry;
   BddNode *node;
   double size, s;

   if (root < 2)
      return root;

   entry = BddCache_lookup(&misccache, SATCOUHASH(root));
   if (entry->d.a == root && entry->d.c == miscid)
     return entry->d.res;

   node = &bddnodes[root];
   size = 0;
   s = 1;

   s *= pow(2.0, (float)(LEVEL(LOWp(node)) - LEVELp(node) - 1));
   size += s * satcount_rec(LOWp(node));

   s = 1;
   s *= pow(2.0, (float)(LEVEL(HIGHp(node)) - LEVELp(node) - 1));
   size += s * satcount_rec(HIGHp(node));

   entry->d.a = root;
   entry->d.c = miscid;
   entry->d.res = size;

   return size;
}


/*
NAME    {* bdd\_satcountln *}
EXTRA   {* bdd\_setcountlnset *}
SECTION {* info *}
SHORT   {* calculates the log. number of satisfying variable assignments *}
PROTO   {* double bdd_satcountln(BDD r)
double bdd_satcountlnset(BDD r, BDD varset)*}
DESCR   {* Calculates how many possible variable assignments there
	   exists such that {\tt r} is satisfied (true) and returns
	   the logarithm of this. The result is calculated in such a
	   manner that it is practically impossible to get an
	   overflow, which is very possible for {\tt bdd\_satcount} if
	   the number of defined variables is too large. All defined
	   variables are considered in the first version. In the
	   second version, only the variables in the variable
	   set {\tt varset} are considered. This makes the function
	   a {\em lot} slower! *}
ALSO    {* bdd\_satone, bdd\_fullsatone, bdd\_satcount *}
RETURN {* The logarithm of the number of possible assignments. *} */
double bdd_satcountln(BDD r)
{
   double size;

   CHECKa(r, 0.0);

   // Invalidate misccache if the number of variable changed since we
   // last used it.
   if (misccache_varnum != bddvarnum)
     {
       BddCache_reset(&misccache);
       misccache_varnum = bddvarnum;
     }

   miscid = CACHEID_SATCOULN;
   size = satcountln_rec(r);

   if (size >= 0.0)
      size += LEVEL(r);

   return size;
}


double bdd_satcountlnset(BDD r, BDD varset)
{
   double unused = bddvarnum;
   BDD n;

   if (ISCONST(varset)) /* empty set */
      return 0.0;

   for (n=varset ; !ISCONST(n) ; n=HIGH(n))
      unused--;

   unused = bdd_satcountln(r) - unused;

   return unused >= 0.0 ? unused : 0.0;
}


static double satcountln_rec(int root)
{
   BddCacheData *entry;
   BddNode *node;
   double size, s1,s2;

   if (root == 0)
      return -1.0;
   if (root == 1)
      return 0.0;

   entry = BddCache_lookup(&misccache, SATCOUHASH(root));
   if (entry->d.a == root && entry->d.c == miscid)
      return entry->d.res;

   node = &bddnodes[root];

   s1 = satcountln_rec(LOWp(node));
   if (s1 >= 0.0)
      s1 += LEVEL(LOWp(node)) - LEVELp(node) - 1;

   s2 = satcountln_rec(HIGHp(node));
   if (s2 >= 0.0)
      s2 += LEVEL(HIGHp(node)) - LEVELp(node) - 1;

   if (s1 < 0.0)
      size = s2;
   else if (s2 < 0.0)
      size = s1;
   else if (s1 < s2)
      size = s2 + log1p(pow(2.0,s1-s2)) / M_LN2;
   else
      size = s1 + log1p(pow(2.0,s2-s1)) / M_LN2;

   entry->d.a = root;
   entry->d.c = miscid;
   entry->d.res = size;

   return size;
}


/*=== COUNT NUMBER OF ALLOCATED NODES ==================================*/

/*
NAME    {* bdd\_nodecount *}
SECTION {* info *}
SHORT   {* counts the number of nodes used for a BDD *}
PROTO   {* int bdd_nodecount(BDD r) *}
DESCR   {* Traverses the BDD and counts all distinct nodes that are used
	   for the BDD. *}
RETURN  {* The number of nodes. *}
ALSO    {* bdd\_pathcount, bdd\_satcount, bdd\_anodecount *}
*/
int bdd_nodecount(BDD r)
{
   int num=0;

   CHECK(r);

   bdd_markcount(r, &num);
   bdd_unmark(r);

   return num;
}


/*
NAME    {* bdd\_anodecount *}
SECTION {* info *}
SHORT   {* counts the number of shared nodes in an array of BDDs *}
PROTO   {* int bdd_anodecount(BDD *r, int num) *}
DESCR   {* Traverses all of the BDDs in {\tt r} and counts all distinct nodes
	   that are used in the BDDs--if a node is used in more than one
	   BDD then it only counts once. The {\tt num} parameter holds the
	   size of the array. *}
RETURN  {* The number of nodes *}
ALSO    {* bdd\_nodecount *}
*/
int bdd_anodecount(BDD *r, int num)
{
   int n;
   int cou=0;

   for (n=0 ; n<num ; n++)
      bdd_markcount(r[n], &cou);

   for (n=0 ; n<num ; n++)
      bdd_unmark(r[n]);

   return cou;
}


/*=== NODE PROFILE =====================================================*/

/*
NAME    {* bdd\_varprofile *}
SECTION {* info *}
SHORT   {* returns a variable profile *}
PROTO   {* int *bdd_varprofile(BDD r) *}
DESCR   {* Counts the number of times each variable occurs in the
	   bdd {\tt r}. The result is stored and returned in an integer array
	   where the i'th position stores the number of times the i'th
	   variable occured in the BDD. It is the users responsibility to
	   free the array again using a call to {\tt free}. *}
RETURN  {* A pointer to an integer array with the profile or NULL if an
	   error occured. *}
*/
int *bdd_varprofile(BDD r)
{
   CHECKa(r, NULL);

   if (__unlikely((varprofile=(int*)malloc(sizeof(int)*bddvarnum)) == NULL))
   {
      bdd_error(BDD_MEMORY);
      return NULL;
   }

   memset(varprofile, 0, sizeof(int)*bddvarnum);
   varprofile_rec(r);
   bdd_unmark(r);
   return varprofile;
}


static void varprofile_rec(int r)
{
   BddNode *node;

   if (r < 2)
      return;

   node = &bddnodes[r];
   if (LEVELp(node) & MARKON)
      return;

   varprofile[bddlevel2var[LEVELp(node)]]++;
   LEVELp(node) |= MARKON;

   varprofile_rec(LOWp(node));
   varprofile_rec(HIGHp(node));
}


/*=== COUNT NUMBER OF PATHS ============================================*/

/*
NAME    {* bdd\_pathcount *}
SECTION {* info *}
SHORT   {* count the number of paths leading to the true terminal *}
PROTO   {* double bdd_pathcount(BDD r) *}
DESCR   {* Counts the number of paths from the root node {\tt r}
	   leading to the terminal true node. *}
RETURN  {* The number of paths *}
ALSO    {* bdd\_nodecount, bdd\_satcount *}
*/
double bdd_pathcount(BDD r)
{
   CHECKa(r, 0.0);

   miscid = CACHEID_PATHCOU;

   return bdd_pathcount_rec(r);
}


static double bdd_pathcount_rec(BDD r)
{
   BddCacheData *entry;
   double size;

   if (ISZERO(r))
      return 0.0;
   if (ISONE(r))
      return 1.0;

   entry = BddCache_lookup(&misccache, PATHCOUHASH(r));
   if (entry->d.a == r  &&  entry->d.c == miscid)
      return entry->d.res;

   size = bdd_pathcount_rec(LOW(r)) + bdd_pathcount_rec(HIGH(r));

   entry->d.a = r;
   entry->d.c = miscid;
   entry->d.res = size;

   return size;
}

static int bdd_have_common_assignment_(BDD left, BDD right)
{
#ifndef NDEBUG
  // arguments can't be constant
  if (ISCONST(left))
    return bdd_error(BDD_ILLBDD);
  if (ISCONST(right))
    return bdd_error(BDD_ILLBDD);
#endif

  // Always make "left" the smaller one to improve cache usage
  if (left > right)
    {
      BDD tmp = left;
      left = right;
      right = tmp;
    }

  BddCacheData *entry = BddCache_lookup(&misccache, COMMONHASH(left,right));
  /* Check entry->b last, because not_rec() does not initialize it. */
  if (entry->i.a == left && entry->i.c == CACHEID_COMMON && entry->i.b == right)
   {
#ifdef CACHESTATS
      bddcachestats.opHit++;
#endif
      return entry->i.res;
   }
#ifdef CACHESTATS
   bddcachestats.opMiss++;
#endif

  // Do they share the top variable?
  int vl = LEVEL(left);
  int vr = LEVEL(right);

  // Try avoiding as many recursive calls as possible
  int res;
  if (vl < vr)
    {
      // left has to "catch up"
      // We know that right is not constant
      BDD l_left = LOW(left);
      BDD h_left = HIGH(left);
      res = ISONE(l_left) || (l_left == right)
            || ISONE(h_left) || (h_left == right)
            || (!ISZERO(l_left) && bdd_have_common_assignment_(l_left, right))
            || (!ISZERO(h_left) && bdd_have_common_assignment_(h_left, right));
    }
  else if (vr < vl)
    {
      // right has to "catch up"
      // We know that left is not constant
      BDD l_right = LOW(right);
      BDD h_right = HIGH(right);
      res = ISONE(l_right) || (l_right == left)
            || ISONE(h_right) || (h_right == left)
            || (!ISZERO(l_right) && bdd_have_common_assignment_(left,
                                                                l_right))
            || (!ISZERO(h_right) && bdd_have_common_assignment_(left,
                                                                h_right));
    }
  else
    {
      // They evolve jointly
      BDD l_left = LOW(left);
      BDD h_left = HIGH(left);
      BDD l_right = LOW(right);
      BDD h_right = HIGH(right);

      res = ((!ISZERO(l_left) && !ISZERO(l_right))
            && (ISONE(l_left) || ISONE(l_right) || l_left == l_right
                || bdd_have_common_assignment_(l_left, l_right)))
            || ((!ISZERO(h_left) && !ISZERO(h_right))
                && (ISONE(h_left) || ISONE(h_right) || h_left == h_right
                    || bdd_have_common_assignment_(h_left, h_right)));
    }

  entry->i.a = left;
  entry->i.c = CACHEID_COMMON;
  entry->i.b = right;
  entry->i.res = res;
  return res;
}

/*=== DECIDE IF TWO BDDS SHARE AT LEAST ONE ASSIGNMENT ================*/

/*
NAME    {* bdd\_have\_common\_assignment *}
SECTION {* info *}
SHORT   {* Decide if two bdds share an satisfying assignement *}
PROTO   {* int bdd_have_common_assignment(BDD l, BDD r) *}
DESCR   {* Returns 1 iff l&r != bddfalse *}
RETURN  {* 0 or 1 *}
*/
int bdd_have_common_assignment(BDD left, BDD right)
{
  // If one of them is false -> false
  if (ISZERO(left) || ISZERO(right))
    return 0;

  // If one of them is true and the other is not false
  // or if they are identical -> true
  if (ISONE(left) || ISONE(right) || left == right)
    return 1;
  // Now both of them are not constant

  return bdd_have_common_assignment_(left, right);
}

/*=== DECIDE IF A BDD is a cube ================*/

/*
NAME    {* bdd\_is\_cube *}
SECTION {* info *}
SHORT   {* Decide if a bdd represents a cube *}
PROTO   {* int bdd_is_cube(BDD b) *}
DESCR   {* Returns 1 iff the given bdd is a conjunction of atomics and
           negations of atomics. Returns 0 if input is false, 1
           if input is true*}
RETURN  {* 0 or 1 *}
*/
int bdd_is_cube(BDD b)
{
  if (ISZERO(b))
    return 0;

  while (!ISONE(b))
    {
      BDD l = LOW(b);
      BDD h = HIGH(b);

      // Cube : high / low / do not care
      if (!ISZERO(l) && !ISZERO(h))
        return 0;

      if (!ISZERO(l))
        b = l;
      else
        b = h;
    }

  return 1;
}

/*************************************************************************
  Other internal functions
*************************************************************************/

static int varset2vartable(BDD r, int comp)
{
   BDD n;

#ifndef NDEBUG
   if (r < 2 && !comp)
      return bdd_error(BDD_VARSET);
#endif

   quantvarsetID++;
   quantvarsetcomp = comp;

   if (quantvarsetID == INT_MAX)
   {
      memset(quantvarset, 0, sizeof(int)*bddvarnum);
      quantvarsetID = 1;
   }

   for (n=r ; n > 1 ; n=HIGH(n))
   {
      quantvarset[LEVEL(n)] = quantvarsetID;
      quantlast = LEVEL(n);
   }

   /* When the varset is complemented, make sure all variables will
      be tested.  */
   if (comp)
      quantlast = bddvarnum;

   return 0;
}


static int varset2svartable(BDD r)
{
   BDD n;

#ifndef NDEBUG
   if (r < 2)
      return bdd_error(BDD_VARSET);
#endif

   quantvarsetID++;

   if (quantvarsetID == INT_MAX/2)
   {
      memset(quantvarset, 0, sizeof(int)*bddvarnum);
      quantvarsetID = 1;
   }

   for (n=r ; !ISCONST(n) ; )
   {
      if (ISZERO(LOW(n)))
      {
	 quantvarset[LEVEL(n)] = quantvarsetID;
	 n = HIGH(n);
      }
      else
      {
	 quantvarset[LEVEL(n)] = -quantvarsetID;
	 n = LOW(n);
      }
      quantlast = LEVEL(n);
   }

   return 0;
}


/* EOF */
