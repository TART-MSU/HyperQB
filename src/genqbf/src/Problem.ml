module M=Model
module E=Expr

type ProblemKind = AA | AE | EE | EA

type ProblemDesc =
  { I : Expr.expression ;
    R : Expr.expression ;
    P : Expr.expression ;
    F : UnrollArgs.unrolling_format_t
  }
    

type UnrolledProblemDesc =
  { I_M : Expr.expression ;
    R_M : Expr.expression ;
    I_N : Expr.expression ;
    R_N : Expr.expression ;
    P   : Expr.expression ;
    F   : UnrollArgs.unrolling_format_t
  }
    
(* TODO: check that I does not use primes *)

(* TODO: Move to model*)    
let unroll_uptok_M (R:E.formula) (k:int) (name:string) : E_formula =
  let unroll_from n =
    let R_n = M.unroll_M n name in
    if n==k-1 then R_n
    else           E.And(R_n,unroll_from (n+1))
  in
  unroll_from 0

let unroll_I_M (I:E.formula) (name:string) : E.formula =
  unroll_M I "M"


let generate_AA_problem (desc:UnrolledProblemDesc) : E.Expression =
  let M =E.And(desc.I_M,desc.R_M) in
  let N =E.And(desc.I_M,desc.R_M) in 
  E.Implies(E.And(M,N), desc.P)

let generate_AE_problem (desc:UnrolledProblemDesc) : E.Expression =
  let M =E.And(desc.I_M,desc.R_M) in
  let N =E.And(desc.I_M,desc.R_M) in 
  E.Implies(M,E.And(N, desc.P))

let generate_EE_problem (desc:UnrolledProblemDesc) : E.Expression =
  let M =E.And(desc.I_M,desc.R_M) in
  let N =E.And(desc.I_M,desc.R_M) in 
  E.And(M,E.And(N, desc.P))

let generate_EA_problem (desc:UnrolledProblemDesc) : E.Expression =
  let M =E.And(desc.I_M,desc.R_M) in
  let N =E.And(desc.I_M,desc.R_M) in 
  E.And(M,E.Implies(N, desc.P))

let generate_problem (desc:UnrolledProblemDesc) : E.Expression =
  match desc.F with
  | AA -> generate_AA_problem desc
  | AE -> generate_AE_problem desc
  | EA -> generate_EA_problem desc
  | EE -> generate_EE_problem desc
    
    
let generate_formula (desc:ProblemDesc, k:int): E.Expression =
  let I_M = unroll_I_M     desc.I   "M" in
  let R_M = unroll_uptok_M desc.R k "M" in
  let I_N = unroll_I_M     desc.I   "N" in
  let R_N = unroll_uptok_M desc.R k "N" in
  let P   = unroll_uptok_P desc.P k "M" "N" in
  
  
  
