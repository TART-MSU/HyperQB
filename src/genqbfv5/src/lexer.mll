{
open Global
open Parser

exception Error

}

let whitespc = [' ' '\t']
let letter = ['A'-'Z' 'a'-'z']
let number = ['0'-'9']
let alphanumbar = ['A'-'Z' 'a'-'z' '0'-'9' '_' '/' ''']
let almostany =['A'-'Z' 'a'-'z' '0'-'9' '_' ' ' '.' '/' '\'' ',']

  
rule norm = parse
  | '"' (almostany* as str) '"' { Global.last str ; QUOTED str }
  | '('             { Global.last "("; OPEN_PAREN }
  | ')'             { Global.last ")"; CLOSE_PAREN }
  | '='             { Global.last "="; EQUALS }
  | 'let'           { Global.last "let"; LET }
  | 'in'            { Global.last "in" ; IN}
  | alphanumbar* as id { Global.last id; IDENT id }  
  | whitespc          { Global.last "whitespc"; norm lexbuf }
  | '\n'              { Global.last "\\n"; Global.incr_linenum (); norm lexbuf }
  | eof               { Global.last "EOF"; EOF }
  | _ as x            { Global.last (String.make 1 x);
		      print_endline ("Bad token: " ^ (String.make 1 x));
		      raise Error
		    }
