open Printf
open Global
module Args=GenArgs

open AIGER  

open Expression
(* module Expr    = Expression *)
module EParser = ExprParser
module ELexer  = ExprLexer
open Property

module PrParser = PropParser
module PrLexer  = PropLexer  
  
module QBF = QBF

exception ArgsError of string

let convert (f:formula) : formula =
  match !Args.conversion with
  | Some CNF -> cnf_formula f
  | Some DNF -> dnf_formula f
  | Some NNF -> nnf_formula f
  | None     -> f


let pr_time s =
  print_endline (sprintf "Time elapsed: %.3f secs (%s)" (Sys.time()) s)


let to_nnf f =
  if !Args.fastFlag then  nnf_formula_fast f
  else                    nnf_formula      f
let to_circuit f =
  if !Args.fastFlag then  Circuit.formula_to_circuit_fast f 
  else                    Circuit.formula_to_circuit      f 

  
let fprint_circuit ch qphi : unit =
  let (quants,phi) = qphi in
  match !Args.format with
  | Some QCIR    ->
     let get_body () =
       let _ = pr_time "calling nnf" in
       let nphi = to_nnf phi in
       let _ = pr_time "calling formula_to_circuit" in
       let b = to_circuit nphi in
       b
     in
     let body = get_body ()
     in
     let _ = pr_time "printing circuit" in
     let _ =
       if !Args.toNum then   Circuit.fprint_quantified_num_circuit ch (quants,body)
       else                  Circuit.fprint_quantified_circuit     ch (quants,body)
     in
     let _ = pr_time "done" in
     ()
  | Some QDIMACS -> fprintf ch "error: QDIMACS output not implemented yet)"
  | Some AIGER   -> fprintf ch "error: AIGER output not implemented yet)"
  | None         -> fprintf ch "error: must specify QCIR | QDIMACS | AIGER" 
  

  (* Was: let generate_formula desc k =
  let generator =
    if !Args.anonymous
    then QBF.generate_quantified_formula_anon
    else QBF.generate_quantified_formula
  in
  generator desc k  *)


  let generate_formula desc k n_exists n_forall =
  (* let generator = *)
    if      !Args.anonymous     then
      QBF.generate_quantified_formula_anon desc k
    else if !Args.is_exists_num then
      QBF.generate_quantified_ES_formula desc k n_exists
    else if !Args.is_forall_num then
      QBF.generate_quantified_AS_formula desc k n_forall
    else
      QBF.generate_quantified_formula desc k
  (* in *)
  (* generator desc k  *)


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

let parse_I_file () : formula =
  if !Args.is_initial_file_A then
    let ch = Stdlib.open_in !Args.initial_file_A in
    let f = Parser.parse ch (EParser.letclause ELexer.norm) in
    f
  else
    raise(ArgsError("must provide initial state formula file with -I file"))

let parse_R_file () : formula =
  if !Args.is_transition_file_A then
    let ch = Stdlib.open_in !Args.transition_file_A in
    let f = Parser.parse ch (EParser.letclause ELexer.norm) in
    f
  else
    raise(ArgsError("must provide transition relation formula file with -R file"))

let parse_J_file () : formula =
  if !Args.is_initial_file_B then
    let ch = Stdlib.open_in !Args.initial_file_B in
    let f = Parser.parse ch (EParser.letclause ELexer.norm) in
    f
  else
    raise(ArgsError("must provide initial state formula file for B with -J file"))

let parse_S_file () : formula =
  if !Args.is_transition_file_B then
    let ch = Stdlib.open_in !Args.transition_file_B in
    let f = Parser.parse ch (EParser.letclause ELexer.norm) in
    f
  else
    raise(ArgsError("must provide transition relation formula file for B with -S file"))

      
let parse_P_file () : property =
  if !Args.is_property_file then
    let ch = Stdlib.open_in !Args.property_file in
    let p = Parser.parse ch (PrParser.property PrLexer.norm) in
    p
  else
    raise(ArgsError("must provide property formula file with -P file"))
      




let parse_separate_files () : QBF.problem_desc =
  match !Args.unrolling_format with 
  | Some qalt -> 
    { init_A     = parse_I_file();
      tr_A       = parse_R_file();
      init_B     = parse_J_file();
      tr_B       = parse_S_file();
      init_C     = parse_J_file(); (* parse model B *)
      tr_C       = parse_S_file(); (* parse transition B *)
      property   = parse_P_file();
      quants     = qalt;
      sem        = (match !Args.unrolling_semantics with
                    | Some sem -> sem
                    | None -> raise(ArgsError("must provide semantics of unrolling")))
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

(* get number of ES *)
let parse_n_exists () =
  if !Args.is_exists_num then
    !Args.exists_num
  else
    raise (ArgsError("missing number of exists with -ES <num>"))

(* get number of AS *)
let parse_n_forall () =
  if !Args.is_forall_num then
    !Args.forall_num
  else
    raise (ArgsError("missing number of foralls with -FS <num>"))




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
    (* read number of exists if -ES is flagged, generate formula of series of exists  *)
    let n_exists    = if (!Args.is_exists_num) then parse_n_exists () else 0 in
    let n_forall    = if (!Args.is_forall_num) then parse_n_forall () else 0 in
    let (quants,phi) = generate_formula desc k n_exists n_forall in
    let outch = Args.open_output () in
    (* conversion (into CNF, etc, if required) for the time being on the
       whole formula and not on the unrolling for M and N separately *)
    let form = convert phi in
    
    (* simplification (if required) *)
    let form =if (!Args.simplify)  then (simplify_formula form) else form in
    (* generate_circuit *)
    
    (* OLD *)
    (* let str = generate_circuit (quants,form) in *)
    (* fprintf outch "%s\n" str  ; *)
    (* NEW *)
    let _ = fprint_circuit outch (quants,form) in
    Args.close_output () 
  with
  | Global.ParserError msg -> 
    Interface.Err.msg "Parsing error" msg
  | ELexer.LexerError -> 
    Interface.Err.msg "Parsing error"
      (sprintf "Unexpected symbol \"%s\" at line %i" (Global.get_last()) (Global.get_linenum()))
  | e -> raise(e)

let _ = Debug.flush()
