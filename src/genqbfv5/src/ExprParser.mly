%{
open Printf

open Global

let curr_tag : string ref = ref ""

(* Parsing error message funtion *)
let parser_error (msg:string) =
  let msg = sprintf "Error at line %i:\n%s" (Global.get_linenum ()) msg in
    raise(Global.ParserError msg)

let get_name id = fst id
let get_line id = snd id


%}
%token <string*int> IDENT  // second param is line number

%token BEGIN END

%token DOT COMMA
%token LET IN
%token OPEN_BRACKET CLOSE_BRACKET
%token OPEN_PAREN CLOSE_PAREN
%token OPEN_ANGLE CLOSE_ANGLE
%token GOOD BAD
%token VERTICAL_BAR
%token COLON DOUBLECOLON SEMICOLON NOT_EQUALS EQUALS
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT LOGICAL_THEN LOGICAL_IFF
%token LOGICAL_TRUE LOGICAL_FALSE

%token EOF

%nonassoc IDENT

%right LOGICAL_AND
%right LOGICAL_OR
%right LOGICAL_THEN
%nonassoc LOGICAL_NOT

%nonassoc OPEN_BRACKET CLOSE_BRACKET
%nonassoc OPEN_PAREN CLOSE_PAREN


%left DOT

/* %start expression */
%start expression letclause


%type <Expression.expression> expression
%type <Expression.formula> letclause

%%


/* EXPRESSIONS */

letclause :
 | LET IDENT EQUALS expression IN letclause
    { Expression.Let(get_name $2,$4,$6) }
 | expression { General($1) }
  
expression :
  | OPEN_PAREN expression CLOSE_PAREN
      { $2 }
  | LOGICAL_TRUE
      { Expression.True }
  | LOGICAL_FALSE
      { Expression.False }
  | LOGICAL_NOT expression
      {
	let phi = $2 in
	match phi with
	| Expression.Literal(Expression.Atom(v)) -> Expression.Literal(Expression.NegAtom(v))
	| _ -> Expression.Neg(phi)
      }
  | expression LOGICAL_AND expression
      { Expression.And ($1, $3) }
  | expression LOGICAL_OR expression
      { Expression.Or ($1, $3) }
  | expression LOGICAL_THEN expression
      { Expression.Implies ($1, $3) }
  | expression LOGICAL_IFF expression
      { Expression.Iff ($1, $3) }
  | IDENT
      {
	Expression.Literal(Expression.Atom(get_name($1)))
      }
