let _debug_ : bool ref = ref false
let _debug_level_ : int ref = ref 0

let infoLevel = 100

let msg (f:unit -> string) (level:int) : unit =
  if !_debug_ && !_debug_level_ >= level then
    print_endline (f ())

let infoMsg (f:unit -> string) : unit =
  msg f infoLevel

let print_file_name (label:string) (file_name:string) : unit =
  let out_str = Printf.sprintf "%s information in file %s" label file_name
  in
    print_endline out_str

let debug_enabled = ref false

let enable_debug () = debug_enabled := true
let disable_debug () = debug_enabled := false
let flush () = if !debug_enabled then  Stdlib.flush Stdlib.stderr
let is_debug_enabled () : bool =  !debug_enabled
      
