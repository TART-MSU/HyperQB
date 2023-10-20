{
open PropParser

exception LexerError

let keyword_table = Hashtbl.create 128

let _ = List.iter (fun (kwd, tok) -> Hashtbl.add keyword_table kwd tok)
          [ ("true" , LOGICAL_TRUE);
            ("false", LOGICAL_FALSE)]
}

let whitespc = [' ' '\t']
let letter = ['A'-'Z' 'a'-'z']
let letterordollar = ['A'-'Z' 'a'-'z' '$']
let digit = ['0'-'9']
let alphanum = ['A'-'Z' 'a'-'z' '0'-'9' '_' '/' ''' '@' ]
let almostany =['A'-'Z' 'a'-'z' '0'-'9' '_' ' ' '.' '/' '-' '(' ')' '\'' ',']
let mostletters =['A'-'Z' 'a'-'z' '0'-'9' '_' '-' '[' ']' '\'']

rule norm = parse
  | '['             { Global.last "["             ; OPEN_BRACKET }
  | ']'             { Global.last "]"             ; CLOSE_BRACKET }
  | '('             { Global.last "("             ; OPEN_PAREN }
  | ')'             { Global.last ")"             ; CLOSE_PAREN }
  | "/\\"           { Global.last "/\\"           ; LOGICAL_AND }
  | "\\/"           { Global.last "\\/"           ; LOGICAL_OR  }
  | "->"            { Global.last "->"            ; LOGICAL_THEN }
  | "<->"           { Global.last "<->"           ; LOGICAL_IFF }
  | '~'             { Global.last "~"             ; LOGICAL_NOT }
  | 'G'             { Global.last "G"             ; LTL_ALWAYS }
  | 'F'             { Global.last "F"             ; LTL_EVENTUALLY }
  | 'X'             { Global.last "X"             ; LTL_NEXT }
  | 'U'             { Global.last "U"             ; LTL_UNTIL }
  | 'R'             { Global.last "R"             ; LTL_RELEASE }
  | (letter mostletters*) as id
          { Global.last id;
            try
              Hashtbl.find keyword_table id
            with Not_found -> 
              IDENT (id, Global.get_linenum())
          }
  | whitespc          { Global.last "whitespc"; norm lexbuf }
  | '\n'              { Global.last "\\n"; Global.incr_linenum (); norm lexbuf }
  | eof               { Global.last "EOF"; EOF }
  | _ as x            { Global.last (String.make 1 x);
                          print_endline ("Bad token: " ^ (String.make 1 x)); raise LexerError
                      }
