(******************************************************************************)
(* @file TranslateArgs.mli                                                    *)
(* Command Line Arguments for Translate                                       *)
(******************************************************************************)

exception MoreThanOneInputFile
exception No_file

type convert_t = DNF | CNF | NNF
val conversion :  convert_t option ref

type output_format_t = QCIR | QDIMACS | AIGER 
val format : output_format_t option ref

val is_input_file  : bool ref
val is_output_file  : bool ref

val input_file  : string ref
val output_file  : string ref
  
val simplify   : bool ref
val parse_args : 'a -> unit
val input_file : string ref
val is_input_file : bool ref
val onlyParse : bool ref

val print_vars : bool ref
  
val open_input : 'a -> in_channel
val close_input : 'a -> unit
val open_output : 'a -> out_channel
val close_output : 'a -> unit
val debugFlag : bool ref
val opts : (string * Arg.spec * string) list
val assignopt : 'a ref -> bool ref -> 'a -> unit
val setdebug : unit -> unit
val anon_fun : string -> unit
val usagemsg : string

