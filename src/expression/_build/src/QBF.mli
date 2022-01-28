open Expression
open Model
module Pr=Property

type quantifiers = AA | AE | EE | EA | EAA

type temporal = G | F | X | U | R

type semantics = OPT | PES | TER_OPT | TER_PES

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
    
val generate_quantified_formula      : problem_desc -> int -> quantified_formula  
val generate_quantified_formula_anon : problem_desc -> int  -> quantified_formula 

val generate_formula: problem_desc -> int -> formula
val generate_formula_anon: problem_desc -> int -> formula
val generate_unrolled_desc: problem_desc -> int -> unrolled_problem_desc
val generate_unrolled_desc_anon: problem_desc -> int -> unrolled_problem_desc 
val generate_problem: unrolled_problem_desc -> formula


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
