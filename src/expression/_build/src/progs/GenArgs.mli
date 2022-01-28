(******************************************************************************)
(* @file GenArgs.mli                                                          *)
(* Command Line Arguments for genqbf                                          *)
(******************************************************************************)

exception MoreThanOneInputFile
exception No_file

type convert_t = DNF | CNF | NNF
val conversion :  convert_t option ref

type output_format_t = QCIR | QDIMACS | AIGER 
val format : output_format_t option ref

val unrolling_format : QBF.quantifiers option ref
val unrolling_semantics : QBF.semantics option ref



val is_unroll_num : bool ref
val unroll_num    : int ref

val is_exists_num : bool ref
val exists_num    : int ref

val is_forall_num : bool ref
val forall_num    : int ref

val is_model_name : bool ref
val model_name    : string ref

val is_initial_file_A : bool ref
val initial_file_A    : string ref
val is_transition_file_A : bool ref
val transition_file_A    : string ref

val is_initial_file_B : bool ref
val initial_file_B    : string ref
val is_transition_file_B : bool ref
val transition_file_B    : string ref
  
val is_property_file : bool ref
val property_file    : string ref

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
val fastFlag : bool ref
  
val opts : (string * Arg.spec * string) list
val assignopt : 'a ref -> bool ref -> 'a -> unit
val setdebug : unit -> unit
val anon_fun : string -> unit
val usagemsg : string

val anonymous : bool ref
