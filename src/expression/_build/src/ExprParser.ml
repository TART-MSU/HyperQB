type token =
  | IDENT of (string*int)
  | BEGIN
  | END
  | DOT
  | COMMA
  | LET
  | IN
  | OPEN_BRACKET
  | CLOSE_BRACKET
  | OPEN_PAREN
  | CLOSE_PAREN
  | OPEN_ANGLE
  | CLOSE_ANGLE
  | GOOD
  | BAD
  | VERTICAL_BAR
  | COLON
  | DOUBLECOLON
  | SEMICOLON
  | NOT_EQUALS
  | EQUALS
  | LOGICAL_AND
  | LOGICAL_OR
  | LOGICAL_NOT
  | LOGICAL_THEN
  | LOGICAL_IFF
  | LOGICAL_TRUE
  | LOGICAL_FALSE
  | EOF

open Parsing;;
let _ = parse_error;;
# 2 "src/ExprParser.mly"
open Printf

open Global

let curr_tag : string ref = ref ""

(* Parsing error message funtion *)
let parser_error (msg:string) =
  let msg = sprintf "Error at line %i:\n%s" (Global.get_linenum ()) msg in
    raise(Global.ParserError msg)

let get_name id = fst id
let get_line id = snd id


# 51 "src/ExprParser.ml"
let yytransl_const = [|
  258 (* BEGIN *);
  259 (* END *);
  260 (* DOT *);
  261 (* COMMA *);
  262 (* LET *);
  263 (* IN *);
  264 (* OPEN_BRACKET *);
  265 (* CLOSE_BRACKET *);
  266 (* OPEN_PAREN *);
  267 (* CLOSE_PAREN *);
  268 (* OPEN_ANGLE *);
  269 (* CLOSE_ANGLE *);
  270 (* GOOD *);
  271 (* BAD *);
  272 (* VERTICAL_BAR *);
  273 (* COLON *);
  274 (* DOUBLECOLON *);
  275 (* SEMICOLON *);
  276 (* NOT_EQUALS *);
  277 (* EQUALS *);
  278 (* LOGICAL_AND *);
  279 (* LOGICAL_OR *);
  280 (* LOGICAL_NOT *);
  281 (* LOGICAL_THEN *);
  282 (* LOGICAL_IFF *);
  283 (* LOGICAL_TRUE *);
  284 (* LOGICAL_FALSE *);
    0 (* EOF *);
    0|]

let yytransl_block = [|
  257 (* IDENT *);
    0|]

let yylhs = "\255\255\
\002\000\002\000\001\000\001\000\001\000\001\000\001\000\001\000\
\001\000\001\000\001\000\000\000\000\000"

let yylen = "\002\000\
\006\000\001\000\003\000\001\000\001\000\002\000\003\000\003\000\
\003\000\003\000\001\000\002\000\002\000"

let yydefred = "\000\000\
\000\000\000\000\000\000\011\000\000\000\000\000\004\000\005\000\
\000\000\000\000\000\000\013\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\003\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\001\000"

let yydgoto = "\003\000\
\011\000\012\000"

let yysindex = "\031\000\
\007\255\001\255\000\000\000\000\007\255\007\255\000\000\000\000\
\254\254\004\255\254\254\000\000\019\255\244\254\007\255\007\255\
\007\255\007\255\028\255\000\000\254\254\243\254\022\255\254\254\
\007\255\249\254\001\255\000\000"

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\040\000\000\000\043\000\000\000\000\000\001\000\000\000\000\000\
\000\000\000\000\000\000\000\000\004\000\003\000\006\000\009\000\
\000\000\000\000\000\000\000\000"

let yygindex = "\000\000\
\021\000\023\000"

let yytablesize = 285
let yytable = "\027\000\
\006\000\004\000\008\000\007\000\019\000\009\000\010\000\004\000\
\010\000\016\000\005\000\017\000\018\000\018\000\015\000\016\000\
\005\000\017\000\018\000\015\000\016\000\009\000\017\000\018\000\
\006\000\013\000\014\000\007\000\008\000\020\000\006\000\001\000\
\002\000\007\000\008\000\021\000\022\000\023\000\024\000\012\000\
\015\000\016\000\002\000\017\000\018\000\026\000\017\000\018\000\
\025\000\028\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\006\000\
\000\000\008\000\007\000\006\000\009\000\008\000\007\000\010\000\
\009\000\000\000\000\000\010\000\000\000\000\000\006\000\006\000\
\008\000\006\000\000\000\009\000\009\000"

let yycheck = "\007\001\
\000\000\001\001\000\000\000\000\001\001\000\000\006\001\001\001\
\000\000\023\001\010\001\025\001\026\001\026\001\022\001\023\001\
\010\001\025\001\026\001\022\001\023\001\001\000\025\001\026\001\
\024\001\005\000\006\000\027\001\028\001\011\001\024\001\001\000\
\002\000\027\001\028\001\015\000\016\000\017\000\018\000\000\000\
\022\001\023\001\000\000\025\001\026\001\025\000\025\001\026\001\
\021\001\027\000\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\007\001\
\255\255\007\001\007\001\011\001\007\001\011\001\011\001\007\001\
\011\001\255\255\255\255\011\001\255\255\255\255\022\001\023\001\
\022\001\025\001\255\255\022\001\023\001"

let yynames_const = "\
  BEGIN\000\
  END\000\
  DOT\000\
  COMMA\000\
  LET\000\
  IN\000\
  OPEN_BRACKET\000\
  CLOSE_BRACKET\000\
  OPEN_PAREN\000\
  CLOSE_PAREN\000\
  OPEN_ANGLE\000\
  CLOSE_ANGLE\000\
  GOOD\000\
  BAD\000\
  VERTICAL_BAR\000\
  COLON\000\
  DOUBLECOLON\000\
  SEMICOLON\000\
  NOT_EQUALS\000\
  EQUALS\000\
  LOGICAL_AND\000\
  LOGICAL_OR\000\
  LOGICAL_NOT\000\
  LOGICAL_THEN\000\
  LOGICAL_IFF\000\
  LOGICAL_TRUE\000\
  LOGICAL_FALSE\000\
  EOF\000\
  "

let yynames_block = "\
  IDENT\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 4 : string*int) in
    let _4 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _6 = (Parsing.peek_val __caml_parser_env 0 : Expression.formula) in
    Obj.repr(
# 62 "src/ExprParser.mly"
    ( Expression.Let(get_name _2,_4,_6) )
# 240 "src/ExprParser.ml"
               : Expression.formula))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 63 "src/ExprParser.mly"
              ( General(_1) )
# 247 "src/ExprParser.ml"
               : Expression.formula))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 1 : Expression.expression) in
    Obj.repr(
# 67 "src/ExprParser.mly"
      ( _2 )
# 254 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    Obj.repr(
# 69 "src/ExprParser.mly"
      ( Expression.True )
# 260 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    Obj.repr(
# 71 "src/ExprParser.mly"
      ( Expression.False )
# 266 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 73 "src/ExprParser.mly"
      (
	let phi = _2 in
	match phi with
	| Expression.Literal(Expression.Atom(v)) -> Expression.Literal(Expression.NegAtom(v))
	| _ -> Expression.Neg(phi)
      )
# 278 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 80 "src/ExprParser.mly"
      ( Expression.And (_1, _3) )
# 286 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 82 "src/ExprParser.mly"
      ( Expression.Or (_1, _3) )
# 294 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 84 "src/ExprParser.mly"
      ( Expression.Implies (_1, _3) )
# 302 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 86 "src/ExprParser.mly"
      ( Expression.Iff (_1, _3) )
# 310 "src/ExprParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string*int) in
    Obj.repr(
# 88 "src/ExprParser.mly"
      (
	Expression.Literal(Expression.Atom(get_name(_1)))
      )
# 319 "src/ExprParser.ml"
               : Expression.expression))
(* Entry expression *)
; (fun __caml_parser_env -> raise (Parsing.YYexit (Parsing.peek_val __caml_parser_env 0)))
(* Entry letclause *)
; (fun __caml_parser_env -> raise (Parsing.YYexit (Parsing.peek_val __caml_parser_env 0)))
|]
let yytables =
  { Parsing.actions=yyact;
    Parsing.transl_const=yytransl_const;
    Parsing.transl_block=yytransl_block;
    Parsing.lhs=yylhs;
    Parsing.len=yylen;
    Parsing.defred=yydefred;
    Parsing.dgoto=yydgoto;
    Parsing.sindex=yysindex;
    Parsing.rindex=yyrindex;
    Parsing.gindex=yygindex;
    Parsing.tablesize=yytablesize;
    Parsing.table=yytable;
    Parsing.check=yycheck;
    Parsing.error_function=parse_error;
    Parsing.names_const=yynames_const;
    Parsing.names_block=yynames_block }
let expression (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 1 lexfun lexbuf : Expression.expression)
let letclause (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 2 lexfun lexbuf : Expression.formula)
