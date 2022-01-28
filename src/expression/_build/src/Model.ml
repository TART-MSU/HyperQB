open Expression

let rename_variable (now:string) (next:string) (v:variable) : variable =
    let len  = String.length v in
    let last = v.[len-1] in
    let is_primed = (last == '\'') in
    if not is_primed then Printf.sprintf "%s%s" v now else
      Printf.sprintf "%s%s" (String.sub v 0 (len-1)) next
  
let rename_literal (now:string) (next:string) (l:literal) : literal =
  match l with
    Atom v    -> Atom   (rename_variable now next v)
  | NegAtom v -> NegAtom(rename_variable now next v)

let rec unroll_expr_suf (now:string) (next:string) (trel:expression) : expression =
  let rename = (unroll_expr_suf now next) in
  match trel with
  | True  -> True
  | False -> False
  | Literal(l)   -> Literal(rename_literal now next l)
  | Neg(x)       -> Neg(rename x)
  | Or(x,y)      -> Or(rename x, rename y)
  | And(x,y)     -> And(rename x, rename y)
  | Implies(x,y) -> Implies(rename x, rename y)
  | Iff(x,y)     -> Iff(rename x, rename y)
  | MOr(ls)      -> MOr(List.map rename ls)
  | MAnd(ls)     -> MAnd(List.map rename ls)
  
let unroll_df_suf (now:string) (next:string) (df:disjunctive_formula) : disjunctive_formula =
  match df with
  | Disj(ls) -> Disj(List.map (rename_literal now next) ls)
  | _        -> df

let unroll_cf_suf (now:string) (next:string) (cf:conjunctive_formula) : conjunctive_formula =
  match cf with
  | Conj(ls) -> Conj(List.map (rename_literal now next) ls)
  | _        -> cf

let rec unroll_suf (now:string) (next:string) (f:formula) : formula =
  let rename_expr    = unroll_expr_suf now next in
  let rename_formula = unroll_suf now next in
  let rename_var = rename_variable now next in
  match f with
  | Let(n,a,b) -> Let(rename_var n, rename_expr a, rename_formula b)
  | CNF(dfs) -> CNF(List.map (unroll_df_suf now next) dfs)
  | DNF(cfs) -> DNF(List.map (unroll_cf_suf now next) cfs)
  | General(e) -> General(rename_expr e)


let suffix (k:int) (name:string) : (string*string) =
  let now  = Printf.sprintf "%s[%d]" name k in
  let next = Printf.sprintf "%s[%d]" name (k+1) in
  (now,next)
  

let unroll (f:formula) (k:int): formula =
  let (now,next) = suffix k "" in
  unroll_suf now next f
  
let unroll_expr (trel:expression) (k:int): expression =
  let (now,next) = suffix k "" in
  unroll_expr_suf now next trel

let unroll_name (f:formula) (k:int) (name:string) : formula =
  let str = if (String.length name) =0 then "" else "_" ^ name in
  let (now,next) = suffix k str in
  unroll_suf now next f

let unroll_expr_name (trel:expression) (k:int) (name:string) : expression  =
  let str = if (String.length name) =0 then "" else "_" ^ name in
  let (now,next) = suffix k str  in
  unroll_expr_suf now next trel
    


    
(* assumes it uses v_A and V_B as the model unrolling generates *)
(* assume there is no v_A' or v_B' *)
let unroll_expr_prop (prop:expression) (k:int) : expression =
  let now = Printf.sprintf "[%d]" k in
  unroll_expr_suf now "" prop
  
let unroll_prop (prop:formula) (k:int) : formula =
  let now = Printf.sprintf "[%d]" k in
  unroll_suf now "" prop


(* keep as k-1 because unroll_name is adding suffix (n+1), it's upto and include k *)    
let unroll_expr_uptok (r:expression) (k:int) (name:string) : expression =
  let rec unroll_from n =
    let r_n = unroll_expr_name r n name in
    if n==k-1 then r_n
    else  And(r_n,unroll_from (n+1))
  in
  unroll_from 0

(* keep as k-1 because unroll_name is adding suffix (n+1), it's upto and include k *)
let unroll_uptok (r:formula) (k:int) (name:string) : formula =
  let rec unroll_from n =
    let r_n = unroll_name r n name in
    if n==k-1 then r_n
    else  build_and r_n (unroll_from (n+1))
  in
  unroll_from 0


(* chagne to n==k, so include the last state *)
let unroll_expr_uptok_G_P (prop:expression) (k:int) : expression =
  let rec unroll_from n =
    let prop_n = unroll_expr prop n in
    if n==k-1 then prop_n else And(prop_n,unroll_from (n+1))
  in
  unroll_from 0

(* chagne to n==k, so include the last state *)
let unroll_uptok_G_P (f:formula) (k:int) : formula =
  let rec unroll_from n =
    let f_n = unroll f n in
    if n==k-1 then f_n else build_and f_n (unroll_from (n+1))
  in
  unroll_from 0

    
(* chagne to n==k, so include the last state *)
let unroll_expr_uptok_F_P (prop:expression) (k:int) : expression =
  let rec unroll_from n =
    let prop_n = unroll_expr prop n in
    if n==k-1 then prop_n else Or(prop_n,unroll_from (n+1))
  in
  unroll_from 0

(* chagne to n==k, so include the last state *)
let unroll_uptok_F_P (f:formula) (k:int) : formula =
  let rec unroll_from n =
    let f_n = unroll f n in
    if n==k-1 then f_n else build_or f_n (unroll_from (n+1))
  in
  unroll_from 0



(* UNTIL *)
(* OPTIMISTIC*)
(* prop1 UNTIL prop2, forever pro1 = UNSAT, "Weak until" *)
let unroll_expr_uptok_OPT_U_P (prop1:expression) (prop2:expression) (k:int) : expression = 
  let rec unroll_from n =
    let prop1_n = unroll_expr prop1 n in
    let prop2_n = unroll_expr prop2 (n+1) in
    if n==k-1 then Or(And(prop1_n, prop2_n), And(prop1_n,unroll_expr prop1 k)) 
              else Or(And(prop1_n, prop2_n), unroll_from (n+1))
  in
  unroll_from 0


let unroll_uptok_OPT_U_P (f1:formula) (f2:formula) (k:int) : formula =
  let rec unroll_from n =
    let f1_n = unroll f1 n in
    let f2_n = unroll f2 (n+1) in
    if n==k-1 then (build_or (build_and f1_n f2_n) (build_and f1_n (unroll f1 k))) 
              else (build_or (build_and f1_n f2_n) (unroll_from (n+1)))
  in
  unroll_from 0


(* UNTIL *)
(* PESSIMISTIC *)
(* pro1 UNTIL prop2, forever pro1 = UNSAT  *)
let unroll_expr_uptok_PES_U_P (prop1:expression) (prop2:expression) (k:int) : expression = 
  let rec unroll_from n =
    let prop1_n = unroll_expr prop1 n in
    let prop2_n = unroll_expr prop2 (n+1) in
    if n==k-1 then And(prop1_n, prop2_n) else Or(And(prop1_n, prop2_n), unroll_from (n+1))
  in
  unroll_from 0


let unroll_uptok_PES_U_P (f1:formula) (f2:formula) (k:int) : formula =
  let rec unroll_from n =
    let f1_n = unroll f1 n in
    let f2_n = unroll f2 (n+1) in
    if n==k-1 then (build_and f1_n f2_n) else (build_or (build_and f1_n f2_n) (unroll_from (n+1)))
  in
  unroll_from 0





(* RELEASE *)
(* OPTIMISTIC *)
(* prop1 RELEASE prop2, forever prop2 = SAT *)
let unroll_expr_uptok_OPT_R_P (prop1:expression) (prop2:expression) (k:int) : expression = 
  let rec unroll_from n =
    let prop1_n = unroll_expr prop1 n in
    let prop2_n = unroll_expr prop2 n in
    if n==k then Or(unroll_expr prop1 k, unroll_expr prop2 k) 
            else Or(prop1_n, And(prop2_n ,unroll_from (n+1)))
  in
  unroll_from 0



let unroll_uptok_OPT_R_P (f1:formula) (f2:formula) (k:int) : formula =
  let rec unroll_from n =
    let f1_n = unroll f1 n in
    let f2_n = unroll f2 n in
    if n==k then (build_or (unroll f1 k) (unroll f2 k)) 
            else build_or f1_n (build_and f2_n (unroll_from (n+1)))
  in
  unroll_from 0



(* RELEASE *)
(* PESSIMISTIC *)
(* prop1 RELEASE prop2, forever prop2 = UNSAT, "Strong release" *)
let unroll_expr_uptok_PES_R_P (prop1:expression) (prop2:expression) (k:int) : expression = 
  let rec unroll_from n =
    let prop1_n = unroll_expr prop1 n in
    let prop2_n = unroll_expr prop2 n in
    if n==k then prop1_n else Or(prop1_n, And(prop2_n ,unroll_from (n+1)))
  in
  unroll_from 0



let unroll_uptok_PES_R_P (f1:formula) (f2:formula) (k:int) : formula =
  let rec unroll_from n =
    let f1_n = unroll f1 n in
    let f2_n = unroll f2 n in
    if n==k then f1_n else build_or f1_n  (build_and f2_n (unroll_from (n+1)))
  in
  unroll_from 0





(* TODO: NEXT prop is not working... *)
let unroll_expr_X_P (prop:expression) (k:int) : expression =
  let rec unroll_from n =
    let prop_n = unroll_expr prop n in
    if n==k-1 then prop_n else And(prop_n,unroll_from (n+1))
  in
  unroll_from 0


let unroll_X_P (f:formula) (k:int) : formula =
  let rec unroll_from n =
    let f_n = unroll f n in
    if n==k-1 then f_n else build_or f_n (unroll_from (n+1))
  in
  unroll_from 0






    
