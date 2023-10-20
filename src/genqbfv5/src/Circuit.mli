 type 't circuit =
   True
 | False
 | Literal of 't * Expression.literal
 | Let of  't * ('t circuit) * ('t circuit)
 | Or   of 't * ('t circuit) * ('t circuit)
 | And  of 't * ('t circuit) * ('t circuit)
 | MAnd of 't * ('t circuit list)
 | MOr  of 't * ('t circuit list)
 | Xor  of 't * ('t circuit) * ('t circuit)
 | Ite  of 't * ('t circuit) * ('t circuit) * ('t circuit)
  
type str_circuit = string circuit    

type quantified_str_circuit = (Expression.quantifier list)*str_circuit
  
val formula_to_circuit : Expression.formula -> str_circuit 
val expression_to_circuit : Expression.expression -> str_circuit 
val cnf_to_circuit : Expression.disjunctive_formula list -> str_circuit 
val dnf_to_circuit : Expression.conjunctive_formula list -> str_circuit 

val circuit_to_str : str_circuit -> string 
val quantified_circuit_to_str : quantified_str_circuit -> string
val quantified_num_circuit_to_str : quantified_str_circuit ->string 

val fprint_circuit : out_channel -> str_circuit -> unit 
val fprint_quantified_circuit : out_channel -> quantified_str_circuit -> unit
val fprint_quantified_num_circuit : out_channel -> quantified_str_circuit ->unit 
  
