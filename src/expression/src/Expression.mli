exception ErrorInNNF

type variable = string
type ident = string

type literal =
| Atom of variable
| NegAtom of variable
  
type expression =
  True
| False
| Literal of literal
| Neg of expression
| Or of expression * expression
| MOr of expression list    
| And of expression * expression
| MAnd of expression list
| Implies of expression * expression
| Iff of expression * expression
  
type disjunctive_formula =
  FalseDisj
| TrueDisj
| Disj of literal list

type conjunctive_formula =
  FalseConj
| TrueConj
| Conj of literal list

type formula =
| Let of ident * expression * formula
| CNF of disjunctive_formula list
| DNF of conjunctive_formula list
| General of expression

type quantifier =
| Forall of variable list
| Exists of variable list

type quantified_expression = (quantifier list) * expression
  
type quantified_formula = (quantifier list) * formula


(* TRANFORMERS *)

val build_and     : formula -> formula -> formula
val build_or      : formula -> formula -> formula
val build_implies : formula -> formula -> formula
  
val nnf: expression -> expression
val nnf_fast: expression -> expression
val cnf: expression -> disjunctive_formula list
val dnf: expression -> conjunctive_formula list

val nnf_formula: formula -> formula
val nnf_formula_fast: formula -> formula
val cnf_formula: formula -> formula
val dnf_formula: formula -> formula
  

  
val simplify_formula : formula -> formula

val formula_to_expression : formula -> expression  
   
(* PRINTING FUNCTIONS *)
val literal_to_str : literal -> string
val conjunctive_formula_to_str : conjunctive_formula -> string
val disjunctive_formula_to_str : disjunctive_formula -> string
val expression_to_str : expression -> string
val formula_to_str : formula -> string
  

(* OUTPUT FUNCTIONS *)
val fprint_literal : out_channel -> literal -> unit
val fprint_conjunctive_formula : out_channel -> conjunctive_formula -> unit
val fprint_disjunctive_formula : out_channel -> disjunctive_formula -> unit
val fprint_expression : out_channel -> expression -> unit
val fprint_formula : out_channel -> formula -> unit
  

val get_vars : formula -> variable list
val get_vars_expr : expression -> variable list
val get_vars_disjunctive_formula : disjunctive_formula -> variable list
val get_vars_conjunctive_formula : conjunctive_formula -> variable list
  
val size : formula -> int
val size_expr : expression -> int                       
