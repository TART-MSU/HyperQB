(******************************************************************************)
(* @file TranslateArgs.ml                                                     *)
(* Command Line Arguments for Translate                                       *)
(******************************************************************************)

(* open Debug *)
  
exception MoreThanOneInputFile
exception No_file

let input_file    = ref ""
let is_input_file = ref false
let input_file_fd : Unix.file_descr ref = ref Unix.stdin
let phiFile         = ref ""

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
    
let inputFormula (s:string) = 
  phiFile := s

let assigninputfile  (s:string) : unit = assignopt input_file is_input_file s

let convert_opt_list = ["dnf"; "cnf"; "nnf"]
type convert_t = DNF | CNF | NNF

let conversion = ref None
    
let set_convert (co:string) : unit  =
  match co with
  | "dnf" -> conversion := Some DNF
  | "cnf" -> conversion := Some CNF
  | "nnf" -> conversion := Some NNF
  | _ -> conversion := None


type output_format_t = QCIR | QDIMACS
let output_format_list = ["qcir"; "qdimacs"]

let format = ref None
  
let set_format (fmt:string) : unit =
  match fmt with
  | "qcir"    -> format := Some QCIR
  | "qdimacs" -> format := Some QDIMACS
  | _         -> format := None 

let intput_file  = ref ""
let output_file  = ref ""

let set_input_file (s:string) : unit = input_file:=s
let set_output_file (s:string) : unit = output_file:=s
    
let opts =
  [
    ("-i", Arg.String set_input_file, "input file");
    ("-o", Arg.String set_output_file, "output file");
    ("-F", Arg.Symbol (output_format_list, set_format), "output format");
    ("-c", Arg.Symbol (convert_opt_list,set_convert)," convert to CNF or DNF");
    ("-s", Arg.Unit dosimplify, "perform simplification after conversion");
    ("-p", Arg.Unit onlyparse, "only parse the input formula and print it");
    ("--debug", Arg.Unit setdebug,  "debug output information");
  ]

let anon_fun str = 
  if !is_input_file then raise(MoreThanOneInputFile)
  else assigninputfile str
    
let usagemsg = "Parses a Boolean formula (later will generate an input to QBF)."
let error msg = Arg.usage opts msg ; exit 0
let simple_error msg = Printf.eprintf "%s\n" msg ; exit 0
let postcheck () = ()
let parse_args _ = 
  Arg.parse opts anon_fun usagemsg;
  postcheck ()

let open_input _ =
  if !is_input_file then
    begin
      input_file_fd := Unix.openfile !input_file [Unix.O_RDONLY] 0 ;
      Unix.in_channel_of_descr !input_file_fd
    end
  else
    raise(No_file)
    (*stdin*)

let close_input _ =
  if !is_input_file then Unix.close !input_file_fd
