(******************************************************************************)
(* @file TranslateArgs.mli                                                    *)
(* Command Line Arguments for Translate                                       *)
(******************************************************************************)

exception MoreThanOneInputFile
exception No_file

type convert_t = DNF | CNF | NNF
val conversion :  convert_t option ref

type output_format_t = QCIR | QDIMACS 
val format : output_format_t option ref

val intput_file  : string ref
val output_file  : string ref
  
val simplify   : bool ref
val parse_args : 'a -> unit
val input_file : string ref
val is_input_file : bool ref
val input_file_fd : Unix.file_descr ref
val onlyParse : bool ref

val open_input : 'a -> in_channel
val close_input : 'a -> unit
val debugFlag : bool ref
val inputFormula : string -> unit
val opts : (string * Arg.spec * string) list
val phiFile : string ref
val assignopt : 'a ref -> bool ref -> 'a -> unit
val setdebug : unit -> unit
val anon_fun : string -> unit
val usagemsg : string

