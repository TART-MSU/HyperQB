open Expression

val unroll_expr_name : expression -> int -> string -> expression 
val unroll_name      : formula    -> int -> string -> formula 
val unroll           : formula    -> int -> formula 
val unroll_expr      : expression -> int -> expression

val unroll_expr_uptok   : expression -> int -> string -> expression
val unroll_expr_uptok_G_P : expression -> int -> expression 
val unroll_expr_uptok_F_P : expression -> int -> expression 
val unroll_expr_uptok_OPT_U_P : expression -> expression -> int -> expression
val unroll_expr_uptok_OPT_R_P : expression -> expression -> int -> expression
val unroll_expr_uptok_PES_U_P : expression -> expression -> int -> expression
val unroll_expr_uptok_PES_R_P : expression -> expression -> int -> expression


val unroll_uptok     : formula -> int -> string -> formula
val unroll_uptok_G_P : formula -> int -> formula
val unroll_uptok_F_P : formula -> int -> formula
val unroll_uptok_OPT_U_P : formula -> formula -> int -> formula
val unroll_uptok_OPT_R_P : formula -> formula -> int -> formula
val unroll_uptok_PES_U_P : formula -> formula -> int -> formula
val unroll_uptok_PES_R_P : formula -> formula -> int -> formula



(* TODO: not working *)
val unroll_expr_X_P : expression -> int -> expression 
val unroll_X_P : formula -> int -> formula

