(******************************************************************************)
(* @file UnrollArgs.mli                                                       *)
(* Command Line Arguments for unroll                                          *)
(******************************************************************************)

exception MoreThanOneInputFile
exception No_file

type convert_t = DNF | CNF | NNF
val conversion :  convert_t option ref

type output_format_t = QCIR | QDIMACS | AIGER 
val format : output_format_t option ref

val unrolling_format : QBF.quantifiers option ref 
  
val is_unroll_num : bool ref
val unroll_num    : int ref

val is_model_name : bool ref
val model_name    : string ref

val is_initial_file : bool ref
val initial_file    : string ref

val is_transition_file : bool ref
val transition_file    : string ref

val is_property_file : bool ref
val property_file    : string ref

val temporal_op : QBF.temporal option ref
  
val is_input_file  : bool ref
val input_file  : string ref

val is_output_file  : bool ref
val output_file  : string ref
  
val simplify   : bool ref


val parse_args : 'a -> unit
val onlyParse : bool ref

val toNum : bool ref
  
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

