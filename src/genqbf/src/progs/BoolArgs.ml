(******************************************************************************)
(* @file TranslateArgs.ml                                                     *)
(* Command Line Arguments for Translate                                       *)
(******************************************************************************)

(* open Debug *)

exception MoreThanOneInputFile
exception No_file

let is_input_file = ref false
let is_output_file = ref false

let input_file  = ref ""
let output_file  = ref ""

let output_channel = ref Stdlib.stdout
let input_channel = ref Stdlib.stdin

(* Program arguments *)
let debugFlag       = ref false

let assignopt (valref : 'a ref) (valbool : bool ref) (aval : 'a) : unit =
  valref := aval ; valbool := true

let setdebug () =
  (*  Debug.enable_debug(); *)
  debugFlag := true

let onlyParse = ref false
    
let onlyparse() =
  onlyParse := true

let simplify = ref false

let dosimplify() =
  simplify := true
    
let convert_opt_list = ["dnf"; "cnf"; "nnf"]
type convert_t = DNF | CNF | NNF

let conversion = ref None
    
let set_convert (co:string) : unit  =
  match co with
  | "dnf" -> conversion := Some DNF
  | "cnf" -> conversion := Some CNF
  | "nnf" -> conversion := Some NNF
  | _ -> conversion := None


type output_format_t = QCIR | QDIMACS | AIGER
let output_format_list = ["qcir"; "qdimacs"; "aiger"]

let format = ref None
  
let set_format (fmt:string) : unit =
  match fmt with
  | "qcir"    -> format := Some QCIR
  | "qdimacs" -> format := Some QDIMACS
  | "aiger"   -> format := Some AIGER
  | _         -> format := None 


let set_input_file (s:string) : unit = input_file:=s ; is_input_file:=true
let set_output_file (s:string) : unit = output_file:=s ; is_output_file:=true

let print_vars = ref false
  
let opts =
  [
    ("-i", Arg.String set_input_file, "input file");
    ("-o", Arg.String set_output_file, "output file");
    ("-F", Arg.Symbol (output_format_list, set_format), "output format");
    ("-c", Arg.Symbol (convert_opt_list,set_convert)," convert to CNF or DNF");
    ("-s", Arg.Unit dosimplify, "perform simplification after conversion");
    ("-p", Arg.Unit onlyparse, "only parse the input formula and print it");
    ("-v", Arg.Set print_vars, "print variable");
    ("--debug", Arg.Unit setdebug,  "debug output information");
  ]

let anon_fun str = 
  if !is_input_file then raise(MoreThanOneInputFile)
  else set_input_file str
    
let usagemsg =
  "Parses a Boolean formula and generates a representation of the input formula
and generates output of the formula in some formats: Boolean (default) QCIR, QDIMACS, AIGER.
It can optionally transform the formula to cnf,dnf,nnf and perform simple simplifications."



let error msg = Arg.usage opts msg ; exit 0
let simple_error msg = Printf.eprintf "%s\n" msg ; exit 0
let postcheck () = ()
let parse_args _ = 
  Arg.parse opts anon_fun usagemsg;
  postcheck ()

let open_input _ =
  if !is_input_file then
    if(String.equal !input_file "-") then
	input_channel := Stdlib.stdin
    else
      begin
	input_channel := Stdlib.open_in !input_file
    (* input_file_fd := Unix.openfile !input_file [Unix.O_RDONLY] 0 ; *)
    (* input_channel := Unix.in_channel_of_descr !input_file_fd *)
      end
  else
    raise(No_file) 
  ;
  !input_channel
    
      
    (*stdin*)

let close_input _ =
(*  if !is_input_file then Unix.close !input_file_fd*)
  Stdlib.close_in !input_channel
  


let open_output _ =
  if !is_output_file then
    begin
      output_channel := Stdlib.open_out !output_file
  (*   output_file_fd := Unix.openfile !output_file [Unix.O_WRONLY] 0 ; *)
  (* Unix.out_channel_of_descr !output_file_fd *)
    end
  ;
  !output_channel

let close_output _ =
  Stdlib.close_out !output_channel
(* if !is_output_file then Unix.close !output_file_fd *)

    




