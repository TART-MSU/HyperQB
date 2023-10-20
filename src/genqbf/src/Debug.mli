val _debug_ : bool ref
val _debug_level_ : int ref

val msg : (unit -> string) -> int -> unit
val print_file_name : string -> string -> unit

val infoMsg : (unit -> string) -> unit

val enable_debug : unit -> unit
val disable_debug : unit -> unit
val flush : unit -> unit
val is_debug_enabled : unit -> bool
