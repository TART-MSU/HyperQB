open Printf

type 'a parser_t = Lexing.lexbuf -> 'a


exception File_not_found of string


let def_comm = "//"

let comm : string ref = ref def_comm

let comm_regexp : Str.regexp = Str.regexp (!comm ^ "[^\n]*")


let reset_comment_sym () : unit =
  comm := def_comm


let set_comment_sym (sym:string) : unit =
  comm := sym


let get_comment_sym () : string =
  !comm


let remove_comments_from_str (str:string) : string =
  Str.global_replace comm_regexp "" str


let remove_comments (ch:Stdlib.in_channel) : string =
  let len = Stdlib.in_channel_length ch in
  let buf = Bytes.create len in
  let _   = Stdlib.really_input ch buf 0 len
  in
     remove_comments_from_str (Bytes.to_string buf)


let parse (ch:Stdlib.in_channel) (the_parser:'a parser_t) : 'a =
  begin
    Global.reset_linenum();
    the_parser (Lexing.from_string (remove_comments ch))
  end

let parse_str (ch:string) (the_parser:'a parser_t) : 'a =
  begin
    Global.reset_linenum();
    the_parser (Lexing.from_string (ch))
  end

 
let open_and_parse (file_name:string) (the_parser:'a parser_t) : 'a =
  let input = try
                open_in file_name
              with _ -> begin
                Interface.Err.msg "File not found"
                  (sprintf "File \"%s\" could not be found" file_name);
                raise(File_not_found file_name)
              end in
  try
    parse input the_parser
  with e -> begin
    Interface.Err.msg "Parser Error"
      ("While parsing file " ^ file_name);
    raise(e)
  end
  
