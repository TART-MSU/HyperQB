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

val expression :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Expression.expression
val letclause :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Expression.formula
