open Printf

open Global

module Expr    = Expression
module EParser = ExprParser
module ELexer  = ExprLexer


(****************)
(* main         *)
(****************)


let convert (phi:Expr.formula) : Expr.formula =
  match !BoolArgs.conversion with
  | Some CNF -> CNF(Expr.cnf (Expr.formula_to_expression phi))
  | Some DNF -> DNF(Expr.dnf (Expr.formula_to_expression phi))
  | Some NNF -> Expr.nnf_formula phi
  | None     -> phi


let transform (phi:Expr.formula) : string =
  match !BoolArgs.format with
  | Some QCIR    -> Circuit.circuit_to_str (Circuit.formula_to_circuit (Expr.nnf_formula phi))
  | Some QDIMACS -> Expr.formula_to_str phi (*Not implemented yet*)
  | Some AIGER   -> Expr.formula_to_str phi (*Not implemented yet*)
  | None         -> Expr.formula_to_str phi
    
let _ =
  try
    let _ = BoolArgs.parse_args () in
    (* Benchmark timer *)
    (* let timer = new LeapLib.timer in *) (* NEED Unix *)
    (* timer#start; *)

    (* open input file and parse the formula *)
    let ch = BoolArgs.open_input () in
    let phi = Parser.parse ch (EParser.letclause ELexer.norm) in
    (* open output  file or stdout *)
    let outch = BoolArgs.open_output () in

    (* conversion *)
    let psi = convert phi in

    (* simplification *)
    let psi = if (!BoolArgs.simplify)  then (Expr.simplify_formula psi) else psi in

    (* print vars *)
    let _ = if (!BoolArgs.print_vars) then
	Printf.fprintf outch "%s\n"  (String.concat " " (Expr.get_vars psi))
    in
    (* transform formats *)
    let str:string = transform psi in
    (* Timer *)
    (* timer#stop; *)
    (* printf "Total Analysis time: %.3f\n" timer#elapsed_time *)
    Printf.fprintf outch "%s\n" str  ;
    BoolArgs.close_input ()  ;
    BoolArgs.close_output () 
  with
  | Global.ParserError msg -> 
    Interface.Err.msg "Parsing error" msg
  | ELexer.LexerError -> 
    Interface.Err.msg "Parsing error"
      (sprintf "Unexpected symbol \"%s\" at line %i" (Global.get_last()) (Global.get_linenum()))
  | e -> raise(e)

let _ = Debug.flush()
