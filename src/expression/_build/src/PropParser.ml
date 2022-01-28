type token =
  | IDENT of (string*int)
  | BEGIN
  | END
  | DOT
  | COMMA
  | OPEN_BRACKET
  | CLOSE_BRACKET
  | OPEN_PAREN
  | CLOSE_PAREN
  | VERTICAL_BAR
  | COLON
  | DOUBLECOLON
  | SEMICOLON
  | NOT_EQUALS
  | LOGICAL_AND
  | LOGICAL_OR
  | LOGICAL_NOT
  | LOGICAL_THEN
  | LOGICAL_IFF
  | LOGICAL_TRUE
  | LOGICAL_FALSE
  | LTL_ALWAYS
  | LTL_EVENTUALLY
  | LTL_NEXT
  | LTL_UNTIL
  | LTL_RELEASE
  | EOF

open Parsing;;
let _ = parse_error;;
# 2 "src/PropParser.mly"
open Printf

open Global

let curr_tag : string ref = ref ""

(* Parsing error message funtion *)
let parser_error (msg:string) =
  let msg = sprintf "Error at line %i:\n%s" (Global.get_linenum ()) msg in
    raise(Global.ParserError msg)

let get_name id = fst id
let get_line id = snd id


# 49 "src/PropParser.ml"
let yytransl_const = [|
  258 (* BEGIN *);
  259 (* END *);
  260 (* DOT *);
  261 (* COMMA *);
  262 (* OPEN_BRACKET *);
  263 (* CLOSE_BRACKET *);
  264 (* OPEN_PAREN *);
  265 (* CLOSE_PAREN *);
  266 (* VERTICAL_BAR *);
  267 (* COLON *);
  268 (* DOUBLECOLON *);
  269 (* SEMICOLON *);
  270 (* NOT_EQUALS *);
  271 (* LOGICAL_AND *);
  272 (* LOGICAL_OR *);
  273 (* LOGICAL_NOT *);
  274 (* LOGICAL_THEN *);
  275 (* LOGICAL_IFF *);
  276 (* LOGICAL_TRUE *);
  277 (* LOGICAL_FALSE *);
  278 (* LTL_ALWAYS *);
  279 (* LTL_EVENTUALLY *);
  280 (* LTL_NEXT *);
  281 (* LTL_UNTIL *);
  282 (* LTL_RELEASE *);
    0 (* EOF *);
    0|]

let yytransl_block = [|
  257 (* IDENT *);
    0|]

let yylhs = "\255\255\
\002\000\002\000\002\000\002\000\002\000\002\000\002\000\002\000\
\002\000\002\000\002\000\001\000\001\000\001\000\001\000\001\000\
\001\000\001\000\001\000\001\000\000\000\000\000"

let yylen = "\002\000\
\003\000\002\000\002\000\002\000\003\000\003\000\002\000\003\000\
\003\000\003\000\003\000\003\000\001\000\001\000\002\000\003\000\
\003\000\003\000\003\000\001\000\002\000\002\000"

let yydefred = "\000\000\
\000\000\000\000\000\000\020\000\000\000\000\000\013\000\014\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\012\000\000\000\000\000\000\000\000\000\
\001\000\000\000\000\000\000\000\000\000\000\000\000\000"

let yydgoto = "\003\000\
\015\000\016\000"

let yysindex = "\017\000\
\063\255\053\255\000\000\000\000\063\255\063\255\000\000\000\000\
\093\255\053\255\053\255\063\255\063\255\063\255\075\255\098\255\
\247\254\245\254\063\255\063\255\063\255\063\255\070\255\088\255\
\027\255\248\254\245\254\245\254\245\254\063\255\063\255\053\255\
\053\255\053\255\053\255\000\000\093\255\010\255\030\255\093\255\
\000\000\245\254\245\254\098\255\018\255\039\255\098\255"

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\014\000\000\000\000\000\000\000\000\000\000\000\000\000\020\000\
\000\000\001\000\000\000\000\000\000\000\000\000\000\000\000\000\
\047\255\027\000\035\000\039\000\043\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\013\000\003\000\005\000\015\000\
\000\000\047\000\051\000\002\000\059\000\055\000\004\000"

let yygindex = "\000\000\
\011\000\034\000"

let yytablesize = 330
let yytable = "\036\000\
\015\000\008\000\017\000\011\000\018\000\019\000\020\000\022\000\
\021\000\022\000\035\000\009\000\016\000\021\000\019\000\017\000\
\018\000\001\000\002\000\022\000\023\000\025\000\027\000\028\000\
\029\000\020\000\007\000\021\000\022\000\037\000\038\000\039\000\
\040\000\033\000\002\000\034\000\035\000\000\000\003\000\000\000\
\042\000\043\000\004\000\024\000\026\000\022\000\005\000\021\000\
\022\000\000\000\006\000\030\000\031\000\004\000\010\000\015\000\
\034\000\035\000\009\000\000\000\010\000\015\000\015\000\004\000\
\015\000\044\000\045\000\046\000\047\000\011\000\005\000\000\000\
\007\000\008\000\012\000\013\000\014\000\000\000\036\000\006\000\
\000\000\000\000\007\000\008\000\019\000\020\000\000\000\021\000\
\022\000\019\000\020\000\000\000\021\000\022\000\030\000\031\000\
\041\000\000\000\000\000\030\000\031\000\000\000\032\000\033\000\
\000\000\034\000\035\000\019\000\020\000\000\000\021\000\022\000\
\032\000\033\000\000\000\034\000\035\000\000\000\000\000\000\000\
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
\000\000\015\000\008\000\017\000\011\000\018\000\000\000\015\000\
\015\000\017\000\015\000\018\000\018\000\016\000\000\000\019\000\
\000\000\015\000\015\000\017\000\017\000\018\000\018\000\000\000\
\000\000\000\000\000\000\007\000\000\000\016\000\016\000\019\000\
\019\000\007\000\007\000\002\000\007\000\000\000\000\000\003\000\
\000\000\002\000\002\000\004\000\002\000\003\000\003\000\005\000\
\003\000\004\000\004\000\006\000\004\000\005\000\005\000\010\000\
\005\000\006\000\006\000\009\000\006\000\010\000\010\000\000\000\
\000\000\009\000"

let yycheck = "\009\001\
\000\000\000\000\000\000\000\000\000\000\015\001\016\001\019\001\
\018\001\019\001\019\001\001\000\000\000\000\000\000\000\005\000\
\006\000\001\000\002\000\000\000\010\000\011\000\012\000\013\000\
\014\000\016\001\000\000\018\001\019\001\019\000\020\000\021\000\
\022\000\016\001\000\000\018\001\019\001\255\255\000\000\255\255\
\030\000\031\000\000\000\010\000\011\000\019\001\000\000\018\001\
\019\001\255\255\000\000\025\001\026\001\001\001\000\000\009\001\
\018\001\019\001\000\000\255\255\008\001\015\001\016\001\001\001\
\018\001\032\000\033\000\034\000\035\000\017\001\008\001\255\255\
\020\001\021\001\022\001\023\001\024\001\255\255\009\001\017\001\
\255\255\255\255\020\001\021\001\015\001\016\001\255\255\018\001\
\019\001\015\001\016\001\255\255\018\001\019\001\025\001\026\001\
\009\001\255\255\255\255\025\001\026\001\255\255\015\001\016\001\
\255\255\018\001\019\001\015\001\016\001\255\255\018\001\019\001\
\015\001\016\001\255\255\018\001\019\001\255\255\255\255\255\255\
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
\255\255\009\001\009\001\009\001\009\001\009\001\255\255\015\001\
\016\001\015\001\018\001\015\001\016\001\009\001\255\255\009\001\
\255\255\025\001\026\001\025\001\026\001\025\001\026\001\255\255\
\255\255\255\255\255\255\009\001\255\255\025\001\026\001\025\001\
\026\001\015\001\016\001\009\001\018\001\255\255\255\255\009\001\
\255\255\015\001\016\001\009\001\018\001\015\001\016\001\009\001\
\018\001\015\001\016\001\009\001\018\001\015\001\016\001\009\001\
\018\001\015\001\016\001\009\001\018\001\015\001\016\001\255\255\
\255\255\015\001"

let yynames_const = "\
  BEGIN\000\
  END\000\
  DOT\000\
  COMMA\000\
  OPEN_BRACKET\000\
  CLOSE_BRACKET\000\
  OPEN_PAREN\000\
  CLOSE_PAREN\000\
  VERTICAL_BAR\000\
  COLON\000\
  DOUBLECOLON\000\
  SEMICOLON\000\
  NOT_EQUALS\000\
  LOGICAL_AND\000\
  LOGICAL_OR\000\
  LOGICAL_NOT\000\
  LOGICAL_THEN\000\
  LOGICAL_IFF\000\
  LOGICAL_TRUE\000\
  LOGICAL_FALSE\000\
  LTL_ALWAYS\000\
  LTL_EVENTUALLY\000\
  LTL_NEXT\000\
  LTL_UNTIL\000\
  LTL_RELEASE\000\
  EOF\000\
  "

let yynames_block = "\
  IDENT\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 1 : Property.property) in
    Obj.repr(
# 60 "src/PropParser.mly"
                                   ( _2 )
# 252 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 61 "src/PropParser.mly"
                          ( let phi = _2 in Property.G(phi) )
# 259 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 62 "src/PropParser.mly"
                          ( let phi = _2 in Property.F(phi) )
# 266 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 63 "src/PropParser.mly"
                    ( let phi = _2 in Property.X(phi) )
# 273 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 64 "src/PropParser.mly"
                             (  let phi1 = _1 in 
                                let phi2 = _3 in  Property.U(phi1, phi2) )
# 282 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 66 "src/PropParser.mly"
                               (  let phi1 = _1 in 
                                  let phi2 = _3 in  Property.R(phi1, phi2) )
# 291 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : Property.property) in
    Obj.repr(
# 68 "src/PropParser.mly"
                          ( let p   = _2 in Property.Neg(p) )
# 298 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Property.property) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Property.property) in
    Obj.repr(
# 69 "src/PropParser.mly"
                                  ( Property.And (_1, _3) )
# 306 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Property.property) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Property.property) in
    Obj.repr(
# 70 "src/PropParser.mly"
                                  ( Property.Or  (_1, _3) )
# 314 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Property.property) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Property.property) in
    Obj.repr(
# 71 "src/PropParser.mly"
                                  ( Property.Implies (_1, _3) )
# 322 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Property.property) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Property.property) in
    Obj.repr(
# 72 "src/PropParser.mly"
                                  ( Property.Iff (_1, _3) )
# 330 "src/PropParser.ml"
               : Property.property))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 1 : Expression.expression) in
    Obj.repr(
# 77 "src/PropParser.mly"
                                   ( _2 )
# 337 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    Obj.repr(
# 78 "src/PropParser.mly"
                   ( Expression.True )
# 343 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    Obj.repr(
# 79 "src/PropParser.mly"
                   ( Expression.False )
# 349 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 80 "src/PropParser.mly"
                        ( let phi = _2 in
	match phi with
	| Expression.Literal(Expression.Atom(v)) -> Expression.Literal(Expression.NegAtom(v))
	| _ -> Expression.Neg(phi)
      )
# 360 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 85 "src/PropParser.mly"
                                  ( Expression.And (_1, _3) )
# 368 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 86 "src/PropParser.mly"
                                  ( Expression.Or (_1, _3) )
# 376 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 87 "src/PropParser.mly"
                                  ( Expression.Implies (_1, _3) )
# 384 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : Expression.expression) in
    let _3 = (Parsing.peek_val __caml_parser_env 0 : Expression.expression) in
    Obj.repr(
# 88 "src/PropParser.mly"
                                  ( Expression.Iff (_1, _3) )
# 392 "src/PropParser.ml"
               : Expression.expression))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string*int) in
    Obj.repr(
# 89 "src/PropParser.mly"
             (Expression.Literal(Expression.Atom(get_name(_1)))  )
# 399 "src/PropParser.ml"
               : Expression.expression))
(* Entry formula *)
; (fun __caml_parser_env -> raise (Parsing.YYexit (Parsing.peek_val __caml_parser_env 0)))
(* Entry property *)
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
let formula (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 1 lexfun lexbuf : Expression.expression)
let property (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 2 lexfun lexbuf : Property.property)
