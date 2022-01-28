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
%token OPEN_BRACKET CLOSE_BRACKET
%token OPEN_PAREN CLOSE_PAREN
%token VERTICAL_BAR
%token COLON DOUBLECOLON SEMICOLON NOT_EQUALS
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT LOGICAL_THEN LOGICAL_IFF
%token LOGICAL_TRUE LOGICAL_FALSE
%token LTL_ALWAYS LTL_EVENTUALLY LTL_NEXT LTL_UNTIL LTL_RELEASE


%token EOF

%nonassoc IDENT

%right LOGICAL_AND
%right LOGICAL_OR
%right LOGICAL_THEN
%nonassoc LOGICAL_NOT
%nonassoc LTL_ALWAYS
%nonassoc LTL_EVENTUALLY
%nonassoc LTL_NEXT
%nonassoc LTL_UNTIL
%nonassoc LTL_RELEASE
%nonassoc OPEN_BRACKET CLOSE_BRACKET
%nonassoc OPEN_PAREN CLOSE_PAREN


%left DOT

%start formula property


%type <Property.property> property
%type <Expression.expression> formula

%%

property:
 | OPEN_PAREN property CLOSE_PAREN { $2 }
 | LTL_ALWAYS formula     { let phi = $2 in Property.G(phi) }
 | LTL_EVENTUALLY formula { let phi = $2 in Property.F(phi) }
 | LTL_NEXT formula { let phi = $2 in Property.X(phi) }
 | formula LTL_UNTIL formula {  let phi1 = $1 in 
                                let phi2 = $3 in  Property.U(phi1, phi2) }
 | formula LTL_RELEASE formula {  let phi1 = $1 in 
                                  let phi2 = $3 in  Property.R(phi1, phi2) }
 | LOGICAL_NOT property   { let p   = $2 in Property.Neg(p) }
 | property LOGICAL_AND property  { Property.And ($1, $3) }
 | property LOGICAL_OR  property  { Property.Or  ($1, $3) }
 | property LOGICAL_THEN property { Property.Implies ($1, $3) }
 | property LOGICAL_IFF property  { Property.Iff ($1, $3) }
     

     
formula :
  | OPEN_PAREN formula CLOSE_PAREN { $2 }
  | LOGICAL_TRUE   { Expression.True }
  | LOGICAL_FALSE  { Expression.False }
  | LOGICAL_NOT formula { let phi = $2 in
	match phi with
	| Expression.Literal(Expression.Atom(v)) -> Expression.Literal(Expression.NegAtom(v))
	| _ -> Expression.Neg(phi)
      }
  | formula LOGICAL_AND formula   { Expression.And ($1, $3) }
  | formula LOGICAL_OR formula    { Expression.Or ($1, $3) }
  | formula LOGICAL_THEN formula  { Expression.Implies ($1, $3) }
  | formula LOGICAL_IFF formula   { Expression.Iff ($1, $3) }
  | IDENT    {Expression.Literal(Expression.Atom(get_name($1)))  }
