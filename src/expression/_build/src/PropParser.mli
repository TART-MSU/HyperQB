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

val formula :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Expression.expression
val property :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Property.property
