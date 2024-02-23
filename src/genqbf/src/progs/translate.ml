open Printf

open Global

module Expr    = Expression
module EParser = ExprParser
module ELexer  = ExprLexer


(****************)
(* main         *)
(****************)


let convert (phi:Expr.expression) : Expr.formula =
  match !TranslateArgs.conversion with
  | Some CNF -> CNF(Expr.cnf phi)
  | Some DNF -> DNF(Expr.dnf phi)
  | Some NNF -> General (Expr.nnf phi)
  | None     -> General(phi)
   
let _ =
  try
    let _ = TranslateArgs.parse_args () in
    (* Benchmark timer *)
    (* let timer = new LeapLib.timer in *) (* NEED Unix *)
    (* timer#start; *)
    
    let ch = TranslateArgs.open_input () in
    let phi = Parser.parse ch (EParser.expression ELexer.norm) in
    let _ = TranslateArgs.close_input () in
    let psi = convert phi in
    let psi = if (!TranslateArgs.simplify)  then (Expr.simplify_formula psi) else psi
    in
    let _ = if (!TranslateArgs.onlyParse) then
	Printf.printf "\n%s\n" (Expr.formula_to_str psi)
    (* pretty print and exit *)
    in
    (* Timer *)
    (* timer#stop; *)
    (* printf "Total Analysis time: %.3f\n" timer#elapsed_time *)
    ()
  with
  | Global.ParserError msg -> 
    Interface.Err.msg "Parsing error" msg
  | ELexer.LexerError -> 
    Interface.Err.msg "Parsing error"
      (sprintf "Unexpected symbol \"%s\" at line %i" (Global.get_last()) (Global.get_linenum()))
  | e -> raise(e)

let _ = Debug.flush()
