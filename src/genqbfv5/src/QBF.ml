open Printf
open Expression
module Pr=Property
open Model

module SetVar = Set.Make(String)

type temporal = G | F | X | U | R

type quantifiers = AA | AE | EE | EA | AAE | AAAE | EAA | EEA | AEA | EEE | AEE | EAAE | EAAEE | AEEE | AAEE | AAAEEE | EEEE

type semantics = OPT | PES | TER_OPT | TER_PES

type problem_desc =
  { init_A     : formula ;
    tr_A       : formula ;
    init_B     : formula ;
    tr_B       : formula ;
    init_C     : formula ;
    tr_C       : formula ;
    init_D     : formula ;
    tr_D       : formula ;
    init_E     : formula ;
    tr_E       : formula ;
    property   : Pr.property ;
    quants     : quantifiers ;
    sem        : semantics ;
  }

type unrolled_problem_desc =
  { init_A   : formula ;
    tr_A     : formula ;
    init_B   : formula ;
    tr_B     : formula ;
    init_C   : formula ;
    tr_C     : formula ;
    init_D   : formula ;
    tr_D     : formula ;
    init_E   : formula ;
    tr_E     : formula ;
    property : formula ;
    quants   : quantifiers ;
  }

(* A series of exists *)
type unrolled_ES_problem_desc =
  { init_list: formula list;
    tr_list: formula list;
    property : formula;
  }


(* A series of exists *)
type unrolled_AS_problem_desc =
  { init_list: formula list;
    tr_list: formula list;
    property : formula;
  }


(* TODO: check that init does not use primes *)

let generate_AA_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  build_implies (build_and m n) desc.property

let generate_AE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  build_implies m (build_and n desc.property)

let generate_EE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  build_and m (build_and n desc.property)

let generate_EA_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  build_and m (build_implies n desc.property)



let generate_EAA_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_and m (build_implies (build_and n o) desc.property) (* implication ??? *)

let generate_EEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_and (build_and m (build_and n o)) desc.property (* implication ??? *)


let generate_AEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_implies m (build_and (build_and n o) desc.property)

let generate_AEEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  build_implies m (build_and (build_and n (build_and o p)) desc.property)

let generate_AAE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_implies m (build_implies n (build_and o desc.property))

let generate_AAAE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  build_implies m (build_implies n (build_implies o (build_and p desc.property)))

let generate_EEA_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_and (build_and m n) (build_implies o desc.property)

let generate_AEA_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_implies m (build_and n (build_implies o desc.property))


let generate_EAAE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  build_and m (build_implies n (build_implies o (build_and p desc.property)))


let generate_EAAEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  let q =build_and desc.init_E desc.tr_E in
  build_and m (build_implies (build_and n o ) (build_and (build_and p q) desc.property))

let generate_AAEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  build_implies m (build_implies n (build_and o (build_and p desc.property)))

let generate_EEEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  build_and m (build_and n (build_and o (build_and p desc.property)))


let generate_AAAEEE_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  let p =build_and desc.init_D desc.tr_D in
  let q =build_and desc.init_E desc.tr_E in
  build_implies m (build_implies n (build_implies o (build_and p (build_and q desc.property))))




let generate_problem (desc:unrolled_problem_desc) : formula =
  match desc.quants with
  | AA -> generate_AA_problem desc
  | AE -> generate_AE_problem desc
  | EA -> generate_EA_problem desc
  | EE -> generate_EE_problem desc
  | EEE -> generate_EEE_problem desc
  | AEE -> generate_AEE_problem desc
  | EAA -> generate_EAA_problem desc
  | EEA -> generate_EEA_problem desc
  | AEA -> generate_AEA_problem desc
  | EAAE -> generate_EAAE_problem desc
  | EAAEE -> generate_EAAEE_problem desc
  | AAE -> generate_AAE_problem desc
  | AAAE -> generate_AAAE_problem desc
  | AEEE -> generate_AEEE_problem desc
  | AAEE -> generate_AAEE_problem desc
  | EEEE -> generate_EEEE_problem desc
  | AAAEEE -> generate_AAAEEE_problem desc

let unroll_property prop k (desc:problem_desc) : formula =
  let rec unroll p =
    match p with
      Pr.G(e)     -> unroll_expr_uptok_G_P e k
    | Pr.F(e)     -> unroll_expr_uptok_F_P e k
    | Pr.X(e)     -> unroll_expr_X_P e k
    | Pr.U(e1,e2)   -> (match desc.sem with
                        | OPT -> unroll_expr_uptok_OPT_U_P e1 e2 k (* optimistic *)
                        | PES -> unroll_expr_uptok_PES_U_P e1 e2 k (* pessimistic *)
                        | TER_OPT -> unroll_expr_uptok_OPT_U_P e1 e2 k  (* terminating-optimistic *)
                        | TER_PES -> unroll_expr_uptok_PES_U_P e1 e2 k) (* terminating-pessimistic *)
    | Pr.R(e1,e2)   -> (match desc.sem with
                        | OPT -> unroll_expr_uptok_OPT_R_P e1 e2 k (* optimistic *)
                        | PES -> unroll_expr_uptok_PES_R_P e1 e2 k (* pessimistic *)
                        | TER_OPT -> unroll_expr_uptok_OPT_R_P e1 e2 k  (* terminating-optimistic *)
                        | TER_PES -> unroll_expr_uptok_PES_R_P e1 e2 k) (* terminating-pessimistic *)
    | Pr.Neg(x)        -> Neg(unroll x)
    | Pr.Or(x,y)       -> Or(unroll x, unroll y)
    | Pr.And(x,y)      -> And(unroll x, unroll y)
    | Pr.Implies (x,y) -> Implies(unroll x, unroll y)
    | Pr.Iff (x,y)     -> Iff(unroll x, unroll y)
  in
  General(unroll prop)


let generate_unrolled_desc (desc:problem_desc) (k:int): unrolled_problem_desc =
  { init_A = unroll_name  desc.init_A 0 "A" ;
    tr_A   = unroll_uptok desc.tr_A   k "A" ;
    init_B = unroll_name  desc.init_B 0 "B" ;
    tr_B   = unroll_uptok desc.tr_B   k "B" ;
    init_C = unroll_name  desc.init_C 0 "C" ;
    tr_C   = unroll_uptok desc.tr_C   k "C" ;
    init_D = unroll_name  desc.init_D 0 "D" ;
    tr_D   = unroll_uptok desc.tr_D   k "D" ;
    init_E = unroll_name  desc.init_E 0 "E" ;
    tr_E   = unroll_uptok desc.tr_E   k "E" ;
    property = unroll_property desc.property k desc;
    quants = desc.quants
  }

let generate_unrolled_desc_anon (desc:problem_desc) (k:int): unrolled_problem_desc =
  { init_A = unroll_name  desc.init_A 0 "" ;
    tr_A   = unroll_uptok desc.tr_A   k "" ;
    init_B = unroll_name  desc.init_B 0 "" ;
    tr_B   = unroll_uptok desc.tr_B   k "" ;
    init_C = unroll_name  desc.init_C 0 "" ;
    tr_C   = unroll_uptok desc.tr_C   k "" ;
    init_D = unroll_name  desc.init_D 0 "" ;
    tr_D   = unroll_uptok desc.tr_D   k "" ;
    init_E = unroll_name  desc.init_E 0 "" ;
    tr_E   = unroll_uptok desc.tr_E   k "" ;
    property = unroll_property desc.property k desc;
    quants = desc.quants
  }


let generate_formula (desc:problem_desc) (k:int): formula =
  generate_problem (generate_unrolled_desc desc k)

let generate_formula_anon (desc:problem_desc) (k:int): formula =
  generate_problem (generate_unrolled_desc_anon desc k)

let two_varlists_to_set l1 l2 =
  let add_elem ss x = SetVar.add x ss in
  let add_list s l = List.fold_left add_elem s l in
  add_list (add_list SetVar.empty l1) l2


let generate_quantified_formula_aux desc k unroller : quantified_formula =
  let udesc  = unroller desc k in
  let f      = generate_problem (unroller desc k) in
  let vars_A = two_varlists_to_set (get_vars udesc.init_A) (get_vars udesc.tr_A) in
  let vars_C = two_varlists_to_set (get_vars udesc.init_C) (get_vars udesc.tr_C) in
  let vars_D = two_varlists_to_set (get_vars udesc.init_D) (get_vars udesc.tr_D) in
  let vars_E = two_varlists_to_set (get_vars udesc.init_E) (get_vars udesc.tr_E) in
  let all_vars_B = two_varlists_to_set (get_vars udesc.init_B) (get_vars udesc.tr_B) in
  let vars_B = SetVar.diff all_vars_B vars_A in
  match udesc.quants with
  | AA -> ([Forall(SetVar.elements (SetVar.union vars_A vars_B))], f)
  | EE -> ([Exists(SetVar.elements (SetVar.union vars_A vars_B))], f)
  | AE -> ([Forall(SetVar.elements vars_A) ;  Exists(SetVar.elements vars_B)], f)
  | EA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements vars_B)], f)
  | EEE -> ([Exists(SetVar.elements (SetVar.union (SetVar.union vars_A vars_B) vars_C))], f)
  | AEE -> ([Forall(SetVar.elements vars_A) ;  Exists(SetVar.elements (SetVar.union vars_B vars_C))], f)
  | EAA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements (SetVar.union vars_B vars_C))], f)
  | EEA -> ([Exists(SetVar.elements (SetVar.union vars_A vars_B)) ;  Forall(SetVar.elements vars_C)], f)
  | AEA -> ([Forall(SetVar.elements vars_A) ;  Exists(SetVar.elements vars_B); Forall(SetVar.elements vars_C)], f)
  | EAAE -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements vars_B) ;  Forall(SetVar.elements vars_C) ;  Exists(SetVar.elements vars_D)], f)
  | EAAEE -> ([Exists(SetVar.elements vars_A); Forall(SetVar.elements (SetVar.union vars_B vars_C)) ; Exists(SetVar.elements (SetVar.union vars_D vars_E))], f)
  | AAE -> ([Forall(SetVar.elements (SetVar.union vars_A vars_B)) ;  Exists(SetVar.elements vars_C)], f)
  | AAAE -> ([Forall(SetVar.elements (SetVar.union vars_A (SetVar.union vars_B vars_C))) ;  Exists(SetVar.elements vars_D)], f)
  | AEEE -> ([Forall(SetVar.elements vars_A) ;  Exists(SetVar.elements (SetVar.union vars_B (SetVar.union vars_C vars_D)))], f)
  | AAEE -> ([Forall(SetVar.elements (SetVar.union vars_A vars_B)) ;  Exists(SetVar.elements (SetVar.union vars_C vars_D))], f)
  | EEEE -> ([Exists(SetVar.elements (SetVar.union vars_A (SetVar.union vars_B (SetVar.union vars_C vars_D))))], f)
  | AAAEEE -> ([Forall(SetVar.elements (SetVar.union vars_A (SetVar.union vars_B vars_C))) ;  Exists(SetVar.elements (SetVar.union vars_D vars_E))], f)


let generate_quantified_formula desc k: quantified_formula =
  generate_quantified_formula_aux desc k generate_unrolled_desc

let generate_quantified_formula_anon desc k: quantified_formula =
  generate_quantified_formula_aux desc k generate_unrolled_desc_anon




(* Attepmt to implement -ES n_exists: A series of exists using tr_A and init_A *)

(* helper: build list of init, with extensions from 1 - es *)
let build_init_list (desc:problem_desc) (n_quantifier:int) : formula list =
  let rec unfold_from n =
    let f_n = (unroll_name desc.init_A 0 (string_of_int(n))) in
    if n==(n_quantifier+1) then [] else f_n :: unfold_from (n+1)
  in
  unfold_from 1

(* helper: build list of tr, extensions from 1 - es *)
let build_tr_list (desc:problem_desc) (k:int) (n_quantifier:int) : formula list =
  let rec unfold_from n =
    let f_n = (unroll_uptok desc.tr_A k (string_of_int(n))) in
    if n==(n_quantifier+1) then [] else f_n :: unfold_from (n+1)
  in
  unfold_from 1


(* build ES problem *)
let generate_ES_problem (desc:unrolled_ES_problem_desc) (n_exists:int) : formula =
  let rec generate_from n =
    let f_n = build_and (List.nth desc.init_list n) (List.nth desc.tr_list n) in
    if n==(n_exists-1) then f_n else build_and f_n (generate_from (n+1))
  in
  build_and (generate_from 0) desc.property

(* build ES problem *)
let build_ES (udesc: unrolled_ES_problem_desc) (n_exists:int) =
  let rec build_from n =
    let f_n = two_varlists_to_set (get_vars (List.nth udesc.init_list n)) (get_vars (List.nth udesc.tr_list n)) in
    if n==(n_exists-1) then f_n else (SetVar.union f_n (build_from (n+1)))
  in
  build_from 0

(* generate unroll ES *)
let generate_ES_unrolled_desc (desc:problem_desc) (k:int) (n_exists:int) : unrolled_ES_problem_desc =
  { init_list   = build_init_list desc n_exists;
    tr_list     = build_tr_list desc k n_exists;
    property    = unroll_property desc.property k desc;
  }

(* generate all existential quantified formula *)
let generate_quantified_ES_formula_aux desc k n_exists unroller : quantified_formula =
  let udesc         = unroller desc k n_exists in
  let f             = generate_ES_problem (unroller desc k n_exists) n_exists in
  let all_vars_list = build_ES udesc n_exists in
    ([Exists(SetVar.elements (all_vars_list))], f)


let generate_ES_formula (desc:problem_desc) (k:int) (n_exists:int): formula =
  generate_ES_problem (generate_ES_unrolled_desc desc k n_exists) n_exists

let generate_quantified_ES_formula desc k n_exists: quantified_formula =
  generate_quantified_ES_formula_aux desc k n_exists generate_ES_unrolled_desc



(* Attepmt to implement -AS n_forall: A series of foralls using tr_A and init_A *)

(* build AS problem *)
let generate_AS_problem (desc:unrolled_AS_problem_desc) (n_forall:int) : formula =
  let rec generate_from n =
    let f_n = build_and (List.nth desc.init_list n) (List.nth desc.tr_list n) in
    if n==(n_forall-1) then f_n else build_and f_n (generate_from (n+1))
  in
  build_implies (generate_from 0) desc.property

(* build AS problem *)
let build_AS (udesc: unrolled_AS_problem_desc) n_forall =
  let rec build_from n =
    let f_n = two_varlists_to_set (get_vars (List.nth udesc.init_list n)) (get_vars (List.nth udesc.tr_list n)) in
    if n==(n_forall-1) then f_n else (SetVar.union f_n (build_from (n+1)))
  in
  build_from 0

(* generate unroll AS *)
let generate_AS_unrolled_desc (desc:problem_desc) (k:int) (n_forall:int) : unrolled_AS_problem_desc =
  { init_list   = build_init_list desc n_forall;
    tr_list     = build_tr_list desc k n_forall;
    property    = unroll_property desc.property k desc;
  }

(* generate all universal quantified formula *)
let generate_quantified_AS_formula_aux desc k n_forall unroller : quantified_formula =
  let udesc         = unroller desc k n_forall in
  let f             = generate_AS_problem (unroller desc k n_forall) n_forall in
  let all_vars_list = build_AS udesc n_forall in
    ([Forall(SetVar.elements (all_vars_list))], f)

let generate_AS_formula (desc:problem_desc) (k:int) (n_forall:int): formula =
  generate_AS_problem (generate_AS_unrolled_desc desc k n_forall) n_forall

let generate_quantified_AS_formula desc k n_forall: quantified_formula =
  generate_quantified_AS_formula_aux desc k n_forall generate_AS_unrolled_desc
