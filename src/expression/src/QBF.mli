open Expression
open Model
module Pr=Property

type quantifiers = AA | AE | EE | EA | EAA

type temporal = G | F | X | U | R

type semantics = OPT | PES | TER_OPT | TER_PES

type newencoding = YY | NN | YN | NY

type problem_desc =
  { init_A   : formula ;
    tr_A     : formula ;
    init_B   : formula ;
    tr_B     : formula ;
    init_C   : formula ;
    tr_C     : formula ;
    property : Pr.property ;
    quants   : quantifiers ;
    sem      : semantics;
    encode   : newencoding;
  }

type unrolled_problem_desc =
  { init_A   : formula ;
    tr_A     : formula ;
    init_B   : formula ;
    tr_B     : formula ;
    init_C   : formula ;
    tr_C     : formula ;
    property : formula ;
    quants   : quantifiers ;
    encode   : newencoding;
  }

(* A series of exists *)
type unrolled_ES_problem_desc =
  { init_list : formula list;
    tr_list   : formula list; 
    property  : formula ;
  }


(* A series of foralls *)
 type unrolled_AS_problem_desc =
  { init_list : formula list;
    tr_list   : formula list; 
    property  : formula ;
  } 
(* new encodings *)
type unrolled_YN_problem_desc = 
  { init_A     : formula ;
    helper_tr_A       : formula ;
    match_helper_A   : formula ;
    init_B     : formula ;
    tr_B       : formula ;
    property   : formula ;
    quants     : quantifiers ;
    encode     : newencoding;
  }

type unrolled_NY_problem_desc = 
  { init_A     : formula ;
    tr_A       : formula ;
    init_B     : formula ;
    helper_tr_B       : formula ;
    match_helper_B   : formula ;
    property   : formula ;
    quants     : quantifiers ;
    encode     : newencoding;
  }

type unrolled_YY_problem_desc = 
  { init_A     : formula ;
    helper_tr_A       : formula ;
    match_helper_A   : formula ;
    init_B     : formula ;
    helper_tr_B       : formula ;
    match_helper_B   : formula ;
    property   : formula ;
    quants     : quantifiers ;
    encode     : newencoding ;
  }    
    
val generate_quantified_formula      : problem_desc -> int -> quantified_formula  
val generate_quantified_formula_anon : problem_desc -> int  -> quantified_formula 

val generate_formula: problem_desc -> int -> formula
val generate_formula_anon: problem_desc -> int -> formula
val generate_unrolled_desc: problem_desc -> int -> unrolled_problem_desc
val generate_unrolled_desc_anon: problem_desc -> int -> unrolled_problem_desc 
val generate_problem: unrolled_problem_desc -> formula

val generate_quantified_formula      : problem_desc -> int -> quantified_formula

(* A series of exists *)
val generate_quantified_ES_formula      : problem_desc -> int -> int -> quantified_formula
val generate_ES_formula: problem_desc -> int -> int -> formula
val generate_ES_unrolled_desc: problem_desc -> int  -> int  -> unrolled_ES_problem_desc 
val generate_ES_problem: unrolled_ES_problem_desc -> int -> formula


(* A series of foralls *)
val generate_quantified_AS_formula      : problem_desc -> int -> int -> quantified_formula
val generate_AS_formula: problem_desc -> int -> int -> formula
val generate_AS_unrolled_desc: problem_desc -> int  -> int  -> unrolled_AS_problem_desc 
val generate_AS_problem: unrolled_AS_problem_desc -> int -> formula

(* newencoding YN *)
val generate_quantified_YN_formula      : problem_desc -> int -> quantified_formula
val generate_YN_formula: problem_desc -> int -> formula
val generate_unrolled_YN_desc: problem_desc -> int   -> unrolled_YN_problem_desc 
val generate_YN_encode_problem: unrolled_YN_problem_desc -> formula
val generate_YN_problem: unrolled_YN_problem_desc  -> formula

(* newencoding NY *)
val generate_quantified_NY_formula      : problem_desc -> int -> quantified_formula
val generate_NY_formula: problem_desc -> int -> formula
val generate_unrolled_NY_desc: problem_desc -> int   -> unrolled_NY_problem_desc 
val generate_NY_encode_problem: unrolled_NY_problem_desc -> formula
val generate_NY_problem: unrolled_NY_problem_desc  -> formula

(* newencoding YY *)
val generate_quantified_YY_formula      : problem_desc -> int -> quantified_formula
val generate_YY_formula: problem_desc -> int -> formula
val generate_unrolled_YY_desc: problem_desc -> int   -> unrolled_YY_problem_desc 
val generate_YY_encode_problem: unrolled_YY_problem_desc -> formula
val generate_YY_problem: unrolled_YY_problem_desc  -> formula