type symbol_table = {
  last_index : int ref ;
  tag_to_idx : (string,int) Hashtbl.t;
  idx_to_tag : (int,string) Hashtbl.t
}

val get_new_index : symbol_table -> int
val new_table : unit -> symbol_table
val tag : symbol_table -> string -> unit
