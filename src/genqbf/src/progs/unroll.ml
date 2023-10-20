open Printf
open Global
module Args=UnrollArgs

open AIGER  

open Expression
(* module Expr    = Expression *)
module EParser = ExprParser
module ELexer  = ExprLexer

module QBF = QBF

exception ArgsError of string

let convert (phi:expression) : formula =
  match !Args.conversion with
  | Some CNF -> CNF(cnf phi)
  | Some DNF -> DNF(dnf phi)
  | Some NNF -> General (nnf phi)
  | None     -> General(phi)


let generate_circuit (qphi:quantified_formula) : string =
  let (quants,phi) = qphi in
  match !Args.format with
  | Some QCIR    ->
    let body = Circuit.formula_to_circuit (nnf_formula phi) in
    if !Args.toNum then
      Circuit.quantified_num_circuit_to_str (quants,body)
    else
      Circuit.quantified_circuit_to_str (quants,body)
  | Some QDIMACS -> "error: QDIMACS output not implemented yet)" (*Not implemented yet*)
  | Some AIGER   -> "error: AIGER output not implemented yet)" (*Not implemented yet*)
  | None         -> "error: must specify QCIR | QDIMACS | AIGER" 

let parse_command_line_args () : unit =
  Args.parse_args ()

let parse_I_file () : expression =
  if !Args.is_initial_file then
    let ch = Stdlib.open_in !Args.initial_file in
    Parser.parse ch (EParser.formula ELexer.norm)
  else
    raise(ArgsError("must provide initial state formula file with -I file"))

let parse_R_file () : expression =
  if !Args.is_transition_file then
    let ch = Stdlib.open_in !Args.transition_file in
    Parser.parse ch (EParser.formula ELexer.norm)
  else
    raise(ArgsError("must provide transition relation formula file with -R file"))

let parse_P_file () : expression =
  if !Args.is_property_file then
    let ch = Stdlib.open_in !Args.property_file in
    Parser.parse ch (EParser.formula ELexer.norm)
  else
    raise(ArgsError("must provide property formula file with -P file"))
      
let parse_separate_files () : QBF.problem_desc =
  let op = match !Args.temporal_op with
      None -> raise(ArgsError("need temporal operator G or F"))
    | Some t -> t in
  match !Args.unrolling_format with
  | Some qalt ->
    { init     = parse_I_file();
      tr       = parse_R_file();
      property = parse_P_file();
      t        = op;
      quants   = qalt;
    }
  | None -> raise(ArgsError("must provide filenames"))


let parse_input_file () : QBF.problem_desc =
  raise(ArgsError("single file not implemented yet"))
    
      
let parse_problem_desc () : QBF.problem_desc =
  if !Args.is_input_file then
    parse_input_file ()
  else
    parse_separate_files ()


let parse_k () =
  if !Args.is_unroll_num then
    !Args.unroll_num
  else
    raise (ArgsError("missing unrolling depth with -k <num>"))

(****************)
(* main         *)
(****************)
let _ =
  (* parse args *)
  try
    let _    = parse_command_line_args () in
    (* read problem descrition and generate Boolean formula for k unrollings *) 
    let desc = parse_problem_desc () in
    let k    = parse_k () in
    let (quants,phi) = QBF.generate_quantified_expression desc k in
    let outch = Args.open_output () in
    (* conversion (into CNF, etc, if required) for the time being on the
       whole formula and not on the unrolling for M and N separately *)
    let form = convert phi in
    
    (* simplification (if required) *)
    let form =if (!Args.simplify)  then (simplify_formula form) else form in
  (* generate_circuit *)
    let str = generate_circuit (quants,form) in
    fprintf outch "%s\n" str  ;
    Args.close_output () 
  with
  | Global.ParserError msg -> 
    Interface.Err.msg "Parsing error" msg
  | ELexer.LexerError -> 
    Interface.Err.msg "Parsing error"
      (sprintf "Unexpected symbol \"%s\" at line %i" (Global.get_last()) (Global.get_linenum()))
  | e -> raise(e)

let _ = Debug.flush()
