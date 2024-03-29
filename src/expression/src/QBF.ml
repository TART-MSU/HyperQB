open Printf
open Expression
open Model
module EParser = ExprParser
module ELexer  = ExprLexer
module Pr      = Property
module SetVar  = Set.Make(String)

type temporal = G | F | X | U | R

type quantifiers = AA | AE | EE | EA | EAA

type semantics = OPT | PES | TER_OPT | TER_PES

type newencoding = YY | NN | YN | NY

type problem_desc =
  { init_A     : formula ;
    tr_A       : formula ;
    init_B     : formula ;
    tr_B       : formula ;
    init_C     : formula ;
    tr_C       : formula ;
    property : Pr.property ;
    quants   : quantifiers ;
    sem      : semantics ;
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


type unrolled_YN_problem_desc = 
  { init_A     : formula ;
    helper_tr_A       : formula ;
    match_helper_A   : formula ;
    init_B     : formula ;
    tr_B       : formula ;
    property   : formula ;
    quants     : quantifiers ;
    encode     : newencoding ;
  }

type unrolled_NY_problem_desc = 
  { init_A     : formula ;
    tr_A       : formula ;
    init_B     : formula ;
    helper_tr_B       : formula ;
    match_helper_B   : formula ;
    property   : formula ;
    quants     : quantifiers ;
    encode     : newencoding ;
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

let generate_EAA_problem (desc:unrolled_problem_desc) : formula =
  let m =build_and desc.init_A desc.tr_A in
  let n =build_and desc.init_B desc.tr_B in
  let o =build_and desc.init_C desc.tr_C in
  build_and m (build_implies (build_and n o) desc.property) (* implication ??? *)

let generate_EA_problem (desc:unrolled_problem_desc) : formula =
  (* let m =build_and desc.init_A desc.tr_A in *)
  (* let n =build_and desc.init_B desc.tr_B in *)
  let m =build_and (dnf_formula desc.init_A) desc.tr_A in
  let n =build_and (dnf_formula desc.init_B) desc.tr_B in
  build_and m (build_implies n desc.property)
  (* build_and m desc.property *)
  

let generate_problem (desc:unrolled_problem_desc) : formula =
  match desc.quants with
  | AA -> generate_AA_problem desc
  | AE -> generate_AE_problem desc
  | EA -> generate_EA_problem desc
  | EE -> generate_EE_problem desc
  | EAA -> generate_EAA_problem desc

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
  { init_A = unroll_name  (dnf_formula desc.init_A) 0 "A" ;
    tr_A   = unroll_uptok desc.tr_A   k "A" ;
    init_B = unroll_name  (dnf_formula desc.init_B) 0 "B" ; 
    tr_B   = unroll_uptok desc.tr_B   k "B" ; 
    init_C = unroll_name  (dnf_formula desc.init_C) 0 "C" ; 
    tr_C   = unroll_uptok desc.tr_C   k "C" ; 
    property = unroll_property desc.property k desc;
    quants = desc.quants;
    encode = desc.encode
  }

let generate_unrolled_desc_anon (desc:problem_desc) (k:int): unrolled_problem_desc =
  { init_A = unroll_name  desc.init_A 0 "" ;
    tr_A   = unroll_uptok desc.tr_A   k "" ;
    init_B = unroll_name  desc.init_B 0 "" ; 
    tr_B   = unroll_uptok desc.tr_B   k "" ;
    init_C = unroll_name  desc.init_C 0 "" ; 
    tr_C   = unroll_uptok desc.tr_C   k "" ;  
    property = unroll_property desc.property k desc;
    quants = desc.quants;
    encode = desc.encode
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
  let f   = generate_problem udesc in
  (* let _ = print_endline (Printf.sprintf "Formula size: %d" (size f)) in *)
  let varinitA = get_vars udesc.init_A in
  (* let _ = print_endline (Printf.sprintf "varinitA length: %d" (List.length varinitA)) in *)
  let vartrA   = get_vars udesc.tr_A in
  (* let _ = print_endline (Printf.sprintf "vartrtA length: %d" (List.length vartrA)) in *)
  (* let vars_A = two_varlists_to_set (get_vars udesc.init_A) (get_vars udesc.tr_A) in *)
  let vars_A = two_varlists_to_set varinitA vartrA in 
  let vars_C = two_varlists_to_set (get_vars udesc.init_C) (get_vars udesc.tr_C) in
  let all_vars_B = two_varlists_to_set (get_vars udesc.init_B) (get_vars udesc.tr_B) in
  let vars_B = SetVar.diff all_vars_B vars_A in
  match udesc.quants with
  | AA -> ([Forall(SetVar.elements (SetVar.union vars_A vars_B))], f)
  | EE -> ([Exists(SetVar.elements (SetVar.union vars_A vars_B))], f)
  | AE -> ([Forall(SetVar.elements vars_A) ;  Exists(SetVar.elements vars_B)], f)
  | EA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements vars_B)], f)
  | EAA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements (SetVar.union vars_B vars_C))], f)


let generate_quantified_formula desc k: quantified_formula =
  generate_quantified_formula_aux desc k generate_unrolled_desc    

let generate_quantified_formula_anon desc k: quantified_formula =
  generate_quantified_formula_aux desc k generate_unrolled_desc_anon


(* helper: build list of init, extensions from 1 - es *)
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

(* Attepmt to implement -ES n_exists: A series of exists using tr_A and init_A *)

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



let get_var_set lst =
  List.fold_left (fun set elem -> SetVar.add elem set) SetVar.empty lst
  

(* problems of new encodings *)
(* build YN problem *)
let generate_YN_problem (desc:unrolled_YN_problem_desc) : formula =
  (* let init_m  =desc.init_A in *)
  let init_m  =(dnf_formula desc.init_A) in
  let match_m =desc.match_helper_A in
  let tr_m    =desc.helper_tr_A in
  (* let m =(build_and (build_implies match_m tr_m) init_m )in *)
  let m =(build_and init_m (build_and match_m tr_m))in
  let init_n =desc.init_B in
  let n =build_and init_n desc.tr_B in
  match desc.quants with
  | EE -> build_and m (build_and n desc.property)
  (* | EE -> build_and m desc.property *)
  | EA -> build_and (build_implies n desc.property) m
  (* | EA -> build_and m desc.property  *)
  | AE -> build_implies m (build_and n desc.property) 
  | AA -> build_implies (build_and m n) desc.property
  | EAA -> build_and m (build_implies n desc.property) (*TODO*)

let generate_NY_problem (desc:unrolled_NY_problem_desc) : formula =
  (* let m =build_and desc.init_A desc.tr_A in *)
  let m =build_and desc.init_A desc.tr_A in
  let init_n  =desc.init_B in
  let match_n =desc.match_helper_B in
  let tr_n    =desc.helper_tr_B in
  let n =(build_and init_n (build_implies match_n tr_n))in
  match desc.quants with
  | EE -> build_and m (build_and n desc.property)
  | EA -> build_and m (build_implies n desc.property)
  | AE -> build_implies m (build_and n desc.property) 
  | AA -> build_implies (build_and m n) desc.property  
  | EAA -> build_and m (build_implies n desc.property) (*TODO*)

let generate_YY_problem (desc:unrolled_YY_problem_desc) : formula =
  let init_m  =(dnf_formula desc.init_A) in
  let match_m =desc.match_helper_A in
  let tr_m    =desc.helper_tr_A in
  let m =(build_and init_m (build_implies match_m tr_m))in
  let init_n  =(dnf_formula desc.init_B) in
  let match_n =desc.match_helper_B in
  let tr_n    =desc.helper_tr_B in
  let n =(build_and init_n (build_implies match_n tr_n))in
  match desc.quants with
  | EE -> build_and m (build_and n desc.property)
  | EA -> build_and m (build_implies n desc.property)
  | AE -> build_implies m (build_and n desc.property) 
  | AA -> build_implies (build_and m n) desc.property  
  | EAA -> build_and m (build_implies n desc.property) (*TODO*)  

  



let generate_YN_encode_problem (desc:unrolled_YN_problem_desc) : formula =
  generate_YN_problem desc
  
let generate_NY_encode_problem (desc:unrolled_NY_problem_desc) : formula =
  generate_NY_problem desc

let generate_YY_encode_problem (desc:unrolled_YY_problem_desc) : formula =
  generate_YY_problem desc  

(* helper methods *)
let trim (var: string) (len: int): string = 
  (* let len = (String.length var) in *)
  if (len != 0) then
    String.sub var 0 (len)
  else
    var

(* let build_match_f (lst: variable list) =
  let lst_len = List.length lst in
  let helper_suf = "_helper'"   in
  let rec exp l i =
    let curr_var = (trim (List.nth lst i) (String.length (List.nth lst i))) in
    let curr_helper = curr_var ^ helper_suf in
    let matching = curr_var ^ " <-> " ^ curr_helper in
    if i == (lst_len-1) then 
      (l ^ "(" ^ matching ^ ")" ) 
    else 
      exp 
      (l ^ "(" ^ matching ^ ")" ^ "/\\" )
      (i+1) 
    in
  exp "" 0     *)

let str_to_formula (str: string) = 
  Parser.parse_str str (EParser.letclause ELexer.norm) 

let build_match_f (lst: variable list): formula =
  let lst_len = List.length lst in
  let helper_suf = "_helper'"   in
  let rec iter l i =
    let curr_var = (trim (List.nth lst i) (String.length (List.nth lst i))) in
    let curr_helper = curr_var ^ helper_suf in
    let matching = str_to_formula (curr_var ^ " <-> " ^ curr_helper ) in
    if i == (lst_len-1) then 
      matching  
    else 
      build_and matching (iter [] (i+1))
    in
  (iter [] 0)


(* generate unroll YN *)
let generate_unrolled_YN_desc (desc:problem_desc) (k:int): unrolled_YN_problem_desc =
  { init_A = unroll_name  desc.init_A 0 "A" ;
    helper_tr_A     = unroll_uptok desc.tr_A   1 "helper_A" ;
    (* match_helper_A  = unroll_match_uptok (build_match_expr (get_vars desc.init_A)) k "A" ; *)
    match_helper_A  = unroll_match_uptok (build_match_f (get_vars desc.init_A)) k "A" ;
    init_B = unroll_name  desc.init_B 0 "B" ; 
    tr_B   = unroll_uptok desc.tr_B   k "B" ;
    property = unroll_property desc.property k desc;
    quants = desc.quants;
    encode = desc.encode
  } 

  (* generate unroll NY *)
let generate_unrolled_NY_desc (desc:problem_desc) (k:int): unrolled_NY_problem_desc =
  { 
    init_A = unroll_name  desc.init_A 0 "A" ; 
    tr_A   = unroll_uptok desc.tr_A   k "A" ;
    init_B = unroll_name  desc.init_B 0 "B" ;
    helper_tr_B     = unroll_uptok desc.tr_B   1 "helper_B" ;
    match_helper_B  = unroll_match_uptok (build_match_f (get_vars desc.init_B)) k "B" ;
    property = unroll_property desc.property k desc;
    quants = desc.quants;
    encode = desc.encode
  } 

let generate_unrolled_YY_desc (desc:problem_desc) (k:int): unrolled_YY_problem_desc =
  { 
    init_A = unroll_name  desc.init_A 0 "A" ;
    helper_tr_A     = unroll_uptok desc.tr_A   1 "helper_A" ;
    match_helper_A  = unroll_match_uptok (build_match_f (get_vars desc.init_A)) k "A" ;
    init_B = unroll_name  desc.init_B 0 "B" ;
    helper_tr_B     = unroll_uptok desc.tr_B   1 "helper_B" ;
    match_helper_B  = unroll_match_uptok (build_match_f (get_vars desc.init_B)) k "B" ;
    property = unroll_property desc.property k desc;
    quants = desc.quants;
    encode = desc.encode
  }   



let generate_YN_formula (desc:problem_desc) (k:int): formula =
  generate_YN_problem (generate_unrolled_YN_desc desc k)

let generate_NY_formula (desc:problem_desc) (k:int): formula =
  generate_NY_problem (generate_unrolled_NY_desc desc k)

let generate_YY_formula (desc:problem_desc) (k:int): formula =
  generate_YY_problem (generate_unrolled_YY_desc desc k)

(* generate all universal quantified formula *)
let generate_quantified_YN_formula_aux desc k unroller : quantified_formula =
  let udesc  = unroller desc k in
  let f      = generate_YN_problem udesc in
  (* let _ = print_endline (Printf.sprintf "var: %s" (build_match_f (get_vars udesc.init_A))) in *)
  (* let _ = print_endline (Printf.sprintf "var: %s" (build_match_f (get_vars udesc.init_A))) in *)
  let varinitA    = get_vars udesc.init_A in
  let varhelperA  = get_vars udesc.helper_tr_A in
  let varsA       = get_vars udesc.match_helper_A in 
  let all_vars_A  = two_varlists_to_set varinitA varsA in 
  let helpers_A   = get_var_set varhelperA in 
  let vars_A      = SetVar.diff all_vars_A helpers_A in
  let all_vars_B = two_varlists_to_set (get_vars udesc.init_B) (get_vars udesc.tr_B) in
  let vars_B = SetVar.diff all_vars_B vars_A in
  match udesc.quants with
  | AA -> ([Forall(SetVar.elements vars_A) ; Exists(SetVar.elements helpers_A) ; Forall(SetVar.elements vars_B)], f)
  (* | AE -> ([Forall(SetVar.elements vars_A) ; Exists(SetVar.elements helpers_A) ; Exists(SetVar.elements vars_B)], f) *)
  | AE -> ([Forall(SetVar.elements vars_A) ; Exists(SetVar.elements (SetVar.union helpers_A vars_B)) ], f)
  | EE -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements helpers_A) ; Exists(SetVar.elements vars_B)], f)
  (* | EE -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements helpers_A)], f) *)
  | EA -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements helpers_A) ; Forall(SetVar.elements vars_B)], f)
  (* | EA -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements helpers_A)], f) *)
  (* | EA -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements (SetVar.union vars_B helpers_A ))], f) *)
  | EAA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements vars_B)], f)

let generate_quantified_NY_formula_aux desc k unroller : quantified_formula =
  let udesc  = unroller desc k in
  let f      = generate_NY_problem udesc in
  (* let _ = print_endline (Printf.sprintf "var: %s" (build_match_f (get_vars udesc.init_A))) in *)
  let varinitB    = get_vars udesc.init_B in
  let varhelperB  = get_vars udesc.helper_tr_B in
  let varsB       = get_vars udesc.match_helper_B in 
  let all_vars_B  = two_varlists_to_set varinitB varsB in 
  let helpers_B   = get_var_set varhelperB in 
  let vars_B      = SetVar.diff all_vars_B helpers_B in
  let all_vars_A = two_varlists_to_set (get_vars udesc.init_A) (get_vars udesc.tr_A) in
  let vars_A = SetVar.diff all_vars_A vars_B in
  match udesc.quants with
  | AA -> ([Forall(SetVar.elements vars_A) ; Forall(SetVar.elements vars_B) ; Exists(SetVar.elements helpers_B)], f)
  | AE -> ([Forall(SetVar.elements vars_A) ; Exists(SetVar.elements vars_B) ; Forall(SetVar.elements helpers_B)], f)
  | EE -> ([Exists(SetVar.elements vars_A) ; Exists(SetVar.elements vars_B) ; Forall(SetVar.elements helpers_B)], f)
  | EA -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements vars_B) ; Exists(SetVar.elements helpers_B)], f)
  | EAA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements vars_B)], f)

let generate_quantified_YY_formula_aux desc k unroller : quantified_formula =
  let udesc  = unroller desc k in
  let f      = generate_YY_problem udesc in
  let varinitA    = get_vars udesc.init_A in
  let varhelperA  = get_vars udesc.helper_tr_A in
  let varsA       = get_vars udesc.match_helper_A in 
  let all_vars_A  = two_varlists_to_set varinitA varsA in 
  let helpers_A   = get_var_set varhelperA in 
  let vars_A      = SetVar.diff all_vars_A helpers_A in
  (* let all_vars_A = two_varlists_to_set (get_vars udesc.init_A) (get_vars udesc.tr_A) in
  let vars_A = SetVar.diff all_vars_A vars_B in *)
  let varinitB    = get_vars udesc.init_B in
  let varhelperB  = get_vars udesc.helper_tr_B in
  let varsB       = get_vars udesc.match_helper_B in 
  let all_vars_B  = two_varlists_to_set varinitB varsB in 
  let helpers_B   = get_var_set varhelperB in 
  let vars_B      = SetVar.diff all_vars_B helpers_B in
  match udesc.quants with
  | AA -> ([Forall(SetVar.elements vars_A) ; Exists(SetVar.elements helpers_A) ; Forall(SetVar.elements vars_B) ; Exists(SetVar.elements helpers_B)], f)
  | AE -> ([Forall(SetVar.elements vars_A) ; Exists(SetVar.elements helpers_A) ; Exists(SetVar.elements vars_B) ; Forall(SetVar.elements helpers_B)], f)
  | EE -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements helpers_A) ; Exists(SetVar.elements vars_B) ; Forall(SetVar.elements helpers_B)], f)
  | EA -> ([Exists(SetVar.elements vars_A) ; Forall(SetVar.elements helpers_A) ; Forall(SetVar.elements vars_B) ; Exists(SetVar.elements helpers_B)], f)
  | EAA -> ([Exists(SetVar.elements vars_A) ;  Forall(SetVar.elements vars_B)], f)



let generate_quantified_YN_formula desc k: quantified_formula =
  generate_quantified_YN_formula_aux desc k generate_unrolled_YN_desc    

let generate_quantified_NY_formula desc k: quantified_formula =
  generate_quantified_NY_formula_aux desc k generate_unrolled_NY_desc 

let generate_quantified_YY_formula desc k: quantified_formula =
  generate_quantified_YY_formula_aux desc k generate_unrolled_YY_desc 
 