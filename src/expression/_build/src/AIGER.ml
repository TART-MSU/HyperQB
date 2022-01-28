open Expression
open SymbolTable

type 't input = Pos of 't | Neg of 't

type 't and_gate = 't * 't input * 't input

type 't latch = 't * 't input * bool

type 't symbol =
  Input of  't * string
| Output of 't * string
| Latch of  't * string
  
type  aiger_circuit = {
  max : int;
  inputs : int list;
  latches : int latch list;
  outputs : int list;
  ands: int and_gate list;
  symbol_table : string list;
}

type modifiable_aiger_circuit = {
  max : int ref;
  inputs : int list ref;
  latches : int latch list ref;
  outputs : int list ref;
  ands: int and_gate list ref;
  symbol_table : string list ref;
}


(* TODO: to this as a class with mutable elements *)
let new_modifiable_aiger_circuit () =
  { max     = ref 0;
    inputs  = ref [];
    latches = ref [];
    outputs = ref [];
    ands    = ref [];
    symbol_table = ref []
  }



    
let var_table_from_expression_aux st e : unit =
  let rec tag_expr (e:expression) : unit =
      match e with
	Literal(Atom(v))    -> tag st v
      | Literal(NegAtom(v)) -> tag st v
      | Neg(x)       -> tag_expr x
      | Or(x,y)      -> tag_expr x ; tag_expr y
      | And(x,y)     -> tag_expr x ; tag_expr y
      | MOr(ls)      -> List.iter tag_expr ls
      | MAnd(ls)     -> List.iter tag_expr ls
      | Implies(x,y) -> tag_expr x ; tag_expr y
      | Iff(x,y)     -> tag_expr x ; tag_expr y
      | _ -> ()
  in
  tag_expr e 
let var_table_from_expression (e:expression) : symbol_table =
  let table = new_table() in
  var_table_from_expression_aux table e ; table
  

let rec add_to_table_from_formula (st:symbol_table) (f:formula) : unit = 
  let var_from_literal l =
    match l with
      Atom(v)    -> tag st v
    | NegAtom(v) -> tag st v
  in
  let var_table_from_conj cf =
    match cf with Conj(ls) -> List.iter var_from_literal ls | _ -> () in
  let var_table_from_disj df =
    match df with Disj(ls) -> List.iter var_from_literal ls | _ -> () in
  let do_formula () =
    match f with
      Let(n,x,y) -> tag st n ; var_table_from_expression_aux st x ;
	add_to_table_from_formula st y
      | CNF (dfs)  -> List.iter var_table_from_disj dfs
      | DNF (cfs)  -> List.iter var_table_from_conj cfs
      | General(e) -> var_table_from_expression_aux st e
  in
  do_formula () 
    
let var_table_from_formula (f:formula) : symbol_table =
  let st = new_table () in
  add_to_table_from_formula st f ; st
  


let var_to_id (st:symbol_table) (v:variable)  = Hashtbl.find st.tag_to_idx v
 
let expression_to_aiger (e:expression) : modifiable_aiger_circuit =
  let st   = var_table_from_expression e  in
  let crct = new_modifiable_aiger_circuit () in
  let _ = (crct.max := !(st.last_index)) in
  let new_id () = (let n = !(crct.max) in crct.max := !(crct.max) ; n) in
  let new_gate (x:int input) (y:int input) : (int input) =
    let n = new_id() in
    crct.ands := (n,x,y) :: !(crct.ands) ;
    Pos(n)
  in
  let op n = match n with Pos(v) -> Neg(v) | Neg(v) -> Pos(v) in
  let rec to_aiger (e:expression) : int input =
    match e with 
      True  -> Pos(0)
    | False -> Neg(0)
    | Literal(Atom(v))    -> let v = var_to_id st v in Pos(v)
    | Literal(NegAtom(v)) -> let v = var_to_id st v in Neg(v)
    | Neg(x)       -> op (to_aiger x)
    | And(x,y)     -> new_gate (to_aiger x) ( to_aiger y) 
    | Or(x,y)      -> op(new_gate (op (to_aiger x)) (op (to_aiger y)))
    | MAnd(ls)     ->
      let rec ands (ls:expression list) =
	match ls with
	  []  -> Pos(0)
	| [x] -> to_aiger x
	| x::tl -> let z = ands tl in new_gate (to_aiger x) z
      in
      ands ls
    |  MOr(ls)  ->
      let rec ors (ls:expression list) =
	match ls with
	  []      -> Pos(0)
	| [x]     -> op (to_aiger x)
	| x::tl -> let z = ors tl in new_gate(op (to_aiger x)) z
      in
      op (ors ls)
    | Implies(x,y) ->
      op (new_gate (to_aiger x) (op (to_aiger y)))
    | Iff(x,y) ->
      let cx  = to_aiger x in
      let cy  = to_aiger y in
      new_gate (op (new_gate cx (op cy))) (op (new_gate (op cx) cy))
  in
  let _ = to_aiger e in
  crct
