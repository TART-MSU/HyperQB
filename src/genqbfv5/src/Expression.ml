open Printf

exception ErrorInNNF
exception ErrorMergingLets
  
type variable = string
type ident = string

type literal =
| Atom of variable
| NegAtom of variable

type expression =
  True
| False
| Literal of literal
| Neg of expression
| Or of expression * expression
| MOr of expression list    
| And of expression * expression
| MAnd of expression list
| Implies of expression * expression
| Iff of expression * expression


type let_clause = (ident * expression)
    
type disjunctive_formula =
| FalseDisj
| TrueDisj
| Disj of literal list

type conjunctive_formula =
| FalseConj
| TrueConj
| Conj of literal list

type formula =
| Let of ident * expression * formula
| CNF of disjunctive_formula list
| DNF of conjunctive_formula list
| General of expression

type quantifier =
| Forall of variable list
| Exists of variable list

type quantified_expression = (quantifier list) * expression

type quantified_formula = (quantifier list) * formula

type logic_op_t = AndOp | OrOp | ImpliesOp | IffOp | NegOp | NoneOp | LetOp

let trueSym = "true"
let falseSym = "false"
let andSym = "/\\"
let orSym = "\\/"
let negSym = "~"
let implSym = "->"
let iffSym = "<->"
let letSym = "let"




(* FORMULA TO EXPR *)
let rec identity (a:expression) (b:expression) : bool=
  match (a,b) with
    True,True     -> true
  | False, False  -> true
  | Literal(Atom(v)),Literal(Atom(w))      -> v=w
  | Literal(NegAtom(v)),Literal(NegAtom(w)) -> v=w
  | Neg(a),Neg(b) -> identity a b
  | Or(a1,b1),Or(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | And(a1,b1),And(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | Implies(a1,b1),Implies(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | Iff(a1,b1),Iff(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | MOr(xs),MOr(ys)->
    if List.length xs != List.length ys then false
    else List.for_all2 identity xs ys
  | MAnd(xs), MAnd(ys) ->
    if List.length xs != List.length ys then false
    else List.for_all2 identity xs ys
  | _ -> false
  
let disjunctive_formula_toexpr (d:disjunctive_formula) : expression =
  match d with
    TrueDisj  -> True
  | FalseDisj -> False
  | Disj  ls  ->
    match ls with
      []   -> False
    | [a]  -> Literal(a)
    | a::[b] -> Or(Literal(a),Literal(b))
    | ls     ->  MOr(List.map (fun l -> Literal(l)) ls)

let conjunctive_formula_toexpr (c:conjunctive_formula) : expression =
  match c with
    TrueConj  -> True
  | FalseConj -> False
  | Conj  ls  ->
    match ls with
      []   -> True
    | [a]  -> Literal(a)
    | a::[b] -> And(Literal(a),Literal(b))
    | ls     -> MAnd(List.map (fun l -> Literal(l)) ls)
    
let cnf_to_expression (ds:disjunctive_formula list) : expression =
  let dtoexpr = disjunctive_formula_toexpr in
  match ds with
    []  -> False
  | [a] -> dtoexpr a
  | a::[b] -> Or(dtoexpr a,dtoexpr b)
  | ls     -> MOr(List.map dtoexpr ls)

let dnf_to_expression (cs:conjunctive_formula list) : expression =
  let ctoexpr = conjunctive_formula_toexpr in
  match cs with
    []  -> True
  | [a] -> ctoexpr a
  | a::[b] -> And(ctoexpr a,ctoexpr b)
  | ls     -> MAnd(List.map ctoexpr ls)

  
(* Merge formulas, with Let, etc into a single formula *)

    
let rec get_exp (f:formula): expression =
  match f with
    Let(n,a,b) -> get_exp b
  | CNF(ds)    -> cnf_to_expression ds
  | DNF(cs)    -> dnf_to_expression cs
  | General(e) -> e

let rec get_lets (f:formula) : let_clause list =
  match f with
    Let(n,a,b) -> (n,a)::(get_lets b)
  | _ -> []

let compare (fs: let_clause list) (gs:let_clause list) : bool =
  let compare_one_on_one x y =
    match x,y with  (nx,ax),(ny,ay) -> not (String.equal nx ny) || (identity ax ay) in
  let compare_one x = List.for_all (fun y -> compare_one_on_one x y) gs in
  List.for_all compare_one fs

let id (a:let_clause) = match a with (n,_) -> n
let body (a:let_clause) = match a with (_,b) -> b
    
let merge (fs:let_clause list) (gs:let_clause list) : let_clause list =
  let not_in_gs x = not (List.exists (fun y->id x = id y) gs) in
  let newfs = List.filter not_in_gs fs in
    newfs @ gs

let builder (f:formula) (g:formula) (op:expression->expression->expression) : formula =
  let fs = get_lets f in
  let gs = get_lets g in
  let fexp = get_exp f in
  let gexp = get_exp g in
  let body = op fexp gexp in
  let rec build lets : formula =
    match lets with
      []    -> General(body)
    | (n,a)::tl -> Let(n,a,build tl) in
  if not (compare fs gs) then raise ErrorMergingLets 
  else build (merge fs gs)
      
let build_and (f:formula) (g:formula) : formula =
  let op x y = And(x,y) in
  builder f g op

let build_or (f:formula) (g:formula) : formula =
  let op x y = Or(x,y) in
  builder f g op

let build_implies (f:formula) (g:formula) : formula =
  let op x y = Implies(x,y) in
  builder f g op
    
    
(* SIMPLIFIERS *)
let is_same_literal (l1:literal) (l2:literal) : bool =
  match (l1,l2) with
    (Atom v1,Atom v2)       -> String.equal v1 v2
  | (NegAtom v1,NegAtom v2) -> String.equal v1 v2
  | _ -> false
    
let is_oposite_literal  (l1:literal) (l2:literal) : bool =
  match (l1,l2) with
    (Atom v1,   NegAtom v2) -> String.equal v1 v2
  | (NegAtom v1,Atom v2)    -> String.equal v1 v2
  | _ -> false

let rec eliminate_repetitions (ls:literal list) : literal list =
  match ls with
  | []    -> []
  | l::[] -> [l]
  | l::tl ->
    if (List.exists (is_same_literal l) tl) then (eliminate_repetitions tl)
    else l::(eliminate_repetitions tl)

let simplify_conjunctive_formula (f:conjunctive_formula) : conjunctive_formula =
  let rec contradiction ls =
    match ls with
    | []    -> false
    | l::[] -> false
    | l::tl ->
      if  (List.exists (is_oposite_literal l) tl) then true 
      else contradiction tl
  in
  match f with
  | TrueConj   -> TrueConj
  | FalseConj  -> FalseConj
  | Conj []    -> FalseConj
  | Conj (a::[]) -> f
  | Conj (a::tl) -> if contradiction (a::tl) then FalseConj else Conj (eliminate_repetitions (a::tl))

let simplify_dnf (cs:conjunctive_formula list) : conjunctive_formula list =
  let ls = List.map (simplify_conjunctive_formula) cs in
  if List.exists (fun l -> match l with TrueConj -> true | _ -> false) ls then [TrueConj] else
    match (List.filter (fun l -> match l with FalseConj -> false | _-> true) ls) with
      [] -> [FalseConj]
    | ret -> ret
  

let simplify_disjunctive_formula (f:disjunctive_formula) : disjunctive_formula =
  let rec tautology ls =
    match ls with
    | [] -> false
    | l::[] -> false
    | l::tl -> if (List.exists (is_oposite_literal l) tl) then true else (tautology tl)
  in
  match f with
  | TrueDisj  -> TrueDisj
  | FalseDisj -> FalseDisj
  | Disj []   -> TrueDisj
  | Disj (a::[]) -> f
  | Disj (a::tl) -> if tautology (a::tl) then TrueDisj else Disj(eliminate_repetitions (a::tl))

let simplify_cnf (ds:disjunctive_formula list): disjunctive_formula list =
  let ls = List.map (simplify_disjunctive_formula) ds in
  if List.exists (fun l -> match l with FalseDisj -> true  | _ -> false) ls then [FalseDisj] else
    match (List.filter (fun l -> match l with TrueDisj  -> false | _ -> true) ls) with
      [] -> [TrueDisj]
    | ret -> ret

let rec simplify_formula (f:formula) : formula =
  match f with
  | Let(x,a,b) -> Let(x,a,simplify_formula b)
  | CNF e -> CNF (simplify_cnf e)
  | DNF e -> DNF (simplify_dnf e)
  | General e -> General e

(* PRETTY PRINTERS *)
let var_to_str (v:variable) : string = v

let ident_to_str (x:ident) : string = x
    
let literal_to_str (l:literal) : string =
  match l with
  | Atom v    -> var_to_str v
  | NegAtom v -> negSym ^ (var_to_str v)


let flatten (ls:'a list) (sym:string) (f:'a -> string) : string =
  let body = String.concat sym (List.map f ls) in
  if (List.length ls <2) then body else "(" ^ body ^ ")"
    
let concat_literals (ls: literal list) (sym:string) : string =
  flatten ls sym (literal_to_str)
    
let conjunctive_formula_to_str (c:conjunctive_formula) : string =
  match c with
  | TrueConj  -> trueSym
  | FalseConj -> falseSym
  | Conj ls -> concat_literals ls andSym

let disjunctive_formula_to_str (d:disjunctive_formula) : string =
  match d with
  | TrueDisj -> trueSym
  | FalseDisj -> falseSym
  | Disj ls -> concat_literals ls orSym


(* TOSTR *)    
    
let dnf_to_str (ds:conjunctive_formula list) : string =
  flatten ds orSym conjunctive_formula_to_str

let cnf_to_str (cs:disjunctive_formula list) : string =
  flatten cs andSym disjunctive_formula_to_str
   
let rec expression_to_str_aux (op:logic_op_t) (phi:expression) : string =
  let tostr = expression_to_str_aux in
  match phi with
  | Literal l -> literal_to_str l
  | True -> trueSym
  | False -> falseSym
  | And(a,b)   ->  if op = AndOp then
		    sprintf  "%s %s %s" (tostr AndOp a) andSym (tostr AndOp b)
                  else
		    sprintf "(%s %s %s)" (tostr AndOp a) andSym (tostr AndOp b)
  | Or(a,b)      -> if op = OrOp then
		    sprintf  "%s %s %s" (tostr OrOp a) andSym (tostr OrOp b)
                  else
		    sprintf "(%s %s %s)" (tostr OrOp a) andSym (tostr OrOp b)    
  | MOr(ls)      -> let body = String.concat orSym (List.map (expression_to_str_aux OrOp) ls) in
		    if op = OrOp then body else "(" ^ body ^ ")"
  | MAnd(ls)      -> let body = String.concat orSym (List.map (expression_to_str_aux AndOp) ls) in
		    if op = AndOp then body else "(" ^ body ^ ")"
  | Neg a        -> let s =  sprintf "%s %s" negSym (tostr NegOp a) in
                    if op = NegOp then s else "(" ^ s ^ ")"
  | Implies(a,b) -> let s = sprintf "%s -> %s" (tostr ImpliesOp a) (tostr ImpliesOp b) in
                    if op = ImpliesOp then s else "(" ^ s ^ ")"
  | Iff(a,b)     -> let s = sprintf "%s <-> %s" (tostr IffOp a) (tostr IffOp b) in
                    if op = IffOp then s else "(" ^ s ^ ")"
let expression_to_str (phi:expression) : string =
  expression_to_str_aux NoneOp phi

let rec formula_to_str(phi : formula): string =
  match phi with
  | Let(x,a,b) -> sprintf "let %s=%s in\n%s" (ident_to_str x) (expression_to_str a) (formula_to_str b)
  | CNF(e)     -> cnf_to_str e
  | DNF(e)     -> dnf_to_str e
  | General(e) -> expression_to_str e


(* OUTPUT FUNCTIONS *)
    
let fprint_var ch v = output_string ch (var_to_str v)
    
let fprint_literal ch l =
  match l with
  | Atom v    -> fprint_var ch v
  | NegAtom v -> output_string ch (negSym ^ var_to_str v)
    
let fprint_list (ch:out_channel) (ls:'a list) (sym:string) (f: out_channel -> 'a -> unit) : unit =
  let op = " " ^ sym ^ " " in
  let pr x =  output_string ch op ; (f ch x) in
  match ls with
    [] -> ()
  | a::xs -> f ch a ; List.iter pr xs
    
let fprint_disjunctive_formula ch d =
  match d with
  | TrueDisj  -> output_string ch trueSym
  | FalseDisj -> output_string ch falseSym
  | Disj ls   -> fprint_list ch ls orSym fprint_literal

let fprint_conjunctive_formula ch c =
  match c with
  | TrueConj  -> output_string ch trueSym
  | FalseConj -> output_string ch falseSym
  | Conj ls   -> fprint_list ch ls andSym fprint_literal
    
    
let fprint_cnf ch cs = fprint_list ch cs andSym fprint_disjunctive_formula
let fprint_dnf ch cs = fprint_list ch cs  orSym fprint_conjunctive_formula

let fprint_expression ch  e =
  let rec pr_sym sym = output_string ch sym
  and open_paren op sameop = (if op!=sameop then pr_sym "(")
  and close_paren op sameop = (if op!=sameop then pr_sym ")") 
  and or_open   op = open_paren op OrOp
  and or_close  op = close_paren op OrOp
  and and_open  op = open_paren op AndOp
  and and_close op = close_paren op AndOp
  and neg_open  op = open_paren op NegOp
  and neg_close op = close_paren op NegOp
  and pr_list op sym ls =
    match ls with
      [] -> ()
    | e::es -> pr_expr e op ; List.iter (fun e -> pr_sym sym ; pr_expr e op) ls
  and  pr_expr e op =
    match e with
      Literal l -> fprint_literal ch l
    | True      -> output_string ch trueSym
    | False     -> output_string ch falseSym
    | And(a,b)  ->
      and_open op ;
      pr_expr a AndOp ; pr_sym andSym ; pr_expr  b AndOp ;
      and_close op
    | Or(a,b) ->
      or_open op ;
      pr_expr a OrOp ; pr_sym orSym ; pr_expr b OrOp ;
      or_close op
    | MOr(ls)  -> or_open op  ; pr_list OrOp  orSym  ls ; or_close op
    | MAnd(ls) -> and_open op ; pr_list AndOp andSym ls ; and_close op
    | Neg a -> neg_open op ; pr_expr a NegOp ; neg_close op
    | Implies(a,b) ->
      pr_sym "(" ;
      pr_expr a ImpliesOp ; pr_sym (" " ^ implSym ^ " ") ; pr_expr a ImpliesOp ;
      pr_sym ")"
    | Iff(a,b) ->
      pr_sym "(" ;
      pr_expr a IffOp ; pr_sym (" " ^ iffSym ^ " ") ; pr_expr a IffOp ;
      pr_sym ")"
  in
  pr_expr e NoneOp
      
let rec fprint_formula ch phi =
  match phi with
  | Let(x,a,b) -> fprintf ch "let %s=" (ident_to_str x) ; fprint_expression ch a ; fprintf ch " in\n" ; fprint_formula ch b
  | CNF(e)     -> fprint_cnf ch e
  | DNF(e)     -> fprint_dnf ch e
  | General(e) -> fprint_expression ch e




    
(* CONVERTERS *)
let rec normalize (e:expression) : expression =
  let rec is_and (e:expression) : bool=
    match e with
    | And(_,_) -> true
    | MAnd(_) -> true
    | _ -> false
  and is_or (e:expression) : bool =
    match e with
      Or(_,_) -> true
    | MOr(_)  -> true
    | _       -> false
  and and_list (e:expression) : expression list =
    match e with
      MAnd xs -> xs
    | And(a,b) -> [a ; b]
    | _ -> [e]
  and or_list (e:expression) : expression list =
    match e with
      MOr xs -> xs
    | Or(a,b) -> [a ; b]
    | _ -> [e]
  in 
  match e with
  | False          -> False
  | True           -> True
  | Literal(a)     -> e
  | Iff(e1,e2)     -> Iff(normalize e1,normalize e2)
  | Implies(e1,e2) -> Implies(normalize e1, normalize e2)

  | MAnd(ls) -> let newls = List.flatten (List.map and_list (List.map normalize ls)) in
		MAnd(newls)
  | MOr (ls) -> let newls = List.flatten (List.map or_list (List.map normalize ls)) in
		MOr(newls)
  | And(x,y) -> let (nx,ny) = (normalize x, normalize y) in
		if (is_and nx || is_and ny) then MAnd((and_list nx) @ (and_list ny)) else And(nx,ny)
  | Or(x,y)  -> let (nx,ny) = (normalize x, normalize y) in
		if (is_or nx  || is_or ny)  then MOr((or_list nx) @ (or_list ny)) else Or(nx,ny)
  | Neg(x)   -> Neg(normalize x)		    
      
let rec nnf (phi:expression) : expression =
  let nnf_aux (phi:expression) : expression =
    match phi with
    | False -> False
    | True -> True
    | Iff (e1,e2)    -> And (nnf (Implies(e1,e2)),nnf (Implies(e2,e1)))
    | Implies(e1,e2) -> Or (nnf (Neg e1),nnf e2)
    | And (e1,e2)    -> And (nnf e1, nnf e2)
    | MAnd(ls)       -> MAnd(List.map nnf ls)
    | MOr(ls)        -> MOr (List.map nnf ls)
    | Or (e1,e2)     -> Or (nnf e1, nnf e2)
    | Neg (Neg e)    -> nnf e
    | Neg (And (e1,e2)) -> Or(nnf (Neg e1),nnf (Neg e2))
    | Neg (Or (e1,e2))  -> And(nnf (Neg e1),nnf (Neg e2))
    | Neg (MOr (ls))    -> MAnd(List.map nnf ls)
    | Neg (MAnd(ls))    -> MOr (List.map nnf ls)
    | Neg (Implies (e1,e2))  -> And(nnf e1, nnf (Neg e2))
    | Neg (Iff (e1, e2)) ->  Or (And (nnf e1, nnf (Neg e2)), And (nnf (Neg e1), nnf e2))
    | Neg Literal(Atom a)    -> Literal(NegAtom a)
    | Neg Literal(NegAtom a) -> Literal(Atom a)    
    | Neg True -> False
    | Neg False -> True
    | Literal(_) -> phi
  in
  normalize (nnf_aux phi)
  
let rec nnf_formula (f:formula) : formula =
  match f with
  | Let(x,a,b) -> Let(x,nnf a,nnf_formula b)
  | General(e) -> General(nnf e)
  | x -> x


let cnf (phi:expression) : disjunctive_formula list =
  let rec cnf_nnf (nnfphi:expression) : disjunctive_formula list =
    let and_to_cnf (c1:disjunctive_formula list) (c2:disjunctive_formula list) : disjunctive_formula list =
      match (c1,c2) with
      | ([FalseDisj],_) -> [FalseDisj]
      | (_,[FalseDisj]) -> [FalseDisj]
      | ([TrueDisj],x)  -> x
      | (x,[TrueDisj])  -> x
      | (lx,ly) -> lx @ ly
    and or_to_cnf (c1:disjunctive_formula list) (c2:disjunctive_formula list) : disjunctive_formula list =
	match (c1, c2) with
	  ([TrueDisj],_) -> [TrueDisj]
          | (_,[TrueDisj]) -> [TrueDisj]
          | ([FalseDisj],x)  -> x
          | (x,[FalseDisj])  -> x
          | (e1_cnf, e2_cnf) ->
	    let get_disjuncts d =
	      match d with
	      | Disj l -> l
	      | _ -> raise(ErrorInNNF)
	    in
	    let add_to_all_in_e2 final_list x1 =
	      let lx1 = get_disjuncts x1 in
              let add_x1 l2 x2 = l2 @ [Disj(lx1 @ (get_disjuncts x2))] in
              let lst = List.fold_left add_x1 [] e2_cnf in
              final_list @ lst
            in
            List.fold_left add_to_all_in_e2 [] e1_cnf
    in
    match nnfphi with
    | Or(e1,e2)  -> or_to_cnf  (cnf_nnf e1) (cnf_nnf e2)
    | And(e1,e2) -> and_to_cnf (cnf_nnf e1) (cnf_nnf e2)
    | MOr(ls)    -> List.fold_left (fun c e-> or_to_cnf  c (cnf_nnf e))  [FalseDisj] ls
    | MAnd(ls)   -> List.fold_left (fun c e-> and_to_cnf c (cnf_nnf e))  [TrueDisj] ls
    | Literal(l) -> [ Disj [ l ]]
    | True       -> [TrueDisj]
    | False      -> [FalseDisj]
    | _          -> raise(ErrorInNNF)
  in
(* simplify_cnf (cnf_nnf (nnf phi)) *)
  cnf_nnf (nnf phi)

    
let dnf (phi:expression): conjunctive_formula list =
  let rec dnf_nnf (nnfphi:expression) : conjunctive_formula list =
    let and_to_dnf (d1:conjunctive_formula list) (d2:conjunctive_formula list) : conjunctive_formula list =
      match d1,d2 with
	([FalseConj],_) -> [FalseConj]
      | (_,[FalseConj]) -> [FalseConj]
      | ([TrueConj],x)  -> x
      | (x,[TrueConj])  -> x
      | (e1_dnf, e2_dnf) ->
	let get_conjuncts c =
	  match c with
	  | Conj l -> l
	  | _ -> raise(ErrorInNNF)
	in
	let add_to_all_in_e2 final_list x1 =
	  let lx1 = get_conjuncts x1 in
          let add_x1 l2 x2 = l2 @ [Conj(lx1 @ (get_conjuncts x2))] in
          let lst = List.fold_left add_x1 [] e2_dnf in
          final_list @ lst
        in
        List.fold_left add_to_all_in_e2 [] e1_dnf
    and or_to_dnf(d1:conjunctive_formula list) (d2:conjunctive_formula list) : conjunctive_formula list =
      match(d1, d2) with
      | ([TrueConj],_) -> [TrueConj]
      | (_,[TrueConj]) -> [TrueConj]
      | ([FalseConj],x)  -> x
      | (x,[FalseConj])  -> x
      | (lx,ly) -> lx @ ly
    in
      match nnfphi with
    | Or(e1,e2)  -> or_to_dnf  (dnf_nnf e1) (dnf_nnf e2)
    | And(e1,e2) -> and_to_dnf (dnf_nnf e1) (dnf_nnf e2)
    | MOr(ls)    -> List.fold_left (fun d e -> or_to_dnf  d (dnf_nnf e)) [FalseConj] ls
    | MAnd(ls)   -> List.fold_left (fun d e -> and_to_dnf d (dnf_nnf e)) [TrueConj] ls
    | Literal(l) -> [ Conj [ l ]]
    | True       -> [TrueConj]
    | False      -> [FalseConj]
    | _          -> raise(ErrorInNNF)
  in
(* simplify_dnf (dnf_nnf (nnf phi)) *)
  dnf_nnf (nnf phi)


let rec cnf_formula f =
  match f with
    CNF(_) -> f
  | Let(n,a,b) -> Let(n,a,cnf_formula b)
  | General(e) -> CNF(cnf e)
  | DNF(cs) -> CNF(cnf (dnf_to_expression cs))

let rec dnf_formula f =
  match f with
    DNF(_) -> f
  | Let(n,a,b) -> Let(n,a,dnf_formula b)
  | General(e) -> DNF(dnf e)
  | CNF(ds) -> DNF(dnf (cnf_to_expression ds))
    
    

let rec identity (a:expression) (b:expression) : bool=
  match (a,b) with
    True,True     -> true
  | False, False  -> true
  | Literal(Atom(v)),Literal(Atom(w))      -> v=w
  | Literal(NegAtom(v)),Literal(NegAtom(w)) -> v=w
  | Neg(a),Neg(b) -> identity a b
  | Or(a1,b1),Or(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | And(a1,b1),And(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | Implies(a1,b1),Implies(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | Iff(a1,b1),Iff(a2,b2)-> (identity a1 a2) && (identity b1 b2) 
  | MOr(xs),MOr(ys)->
    if List.length xs != List.length ys then false
    else List.for_all2 identity xs ys
  | MAnd(xs), MAnd(ys) ->
    if List.length xs != List.length ys then false
    else List.for_all2 identity xs ys
  | _ -> false
    

(* AUX *)
let rec remove_dup (ls:variable list) : variable list =
    match ls with
    | [x] -> [x]
    | []  -> []
    | hd::tl -> hd::(remove_dup (List.filter (fun x -> not (String.equal hd x)) tl))


(* FIX: We need to remove the let ids vars from get_vars *)

let rec get_let_ids (f:formula): variable list =
  match f with
    Let(x,a,b) -> x::(get_let_ids b)
  | _ -> []



let remove_from ls xs =
  let remove_one xs a = List.filter (fun x ->not (String.equal a x)) xs in
  List.fold_left remove_one xs ls 
    
let rec get_vars (f:formula) : variable list =
  let lets = get_let_ids f in
  let vars = remove_dup (get_vars_aux f) in
  remove_from lets vars
and get_vars_expr (e:expression) : variable list =
  remove_dup (get_vars_expr_aux e)
and get_vars_aux (f:formula) : variable list =
  match f with
  | Let(x,a,b) -> remove_dup (get_vars_expr a @ get_vars_aux b)
  | CNF(ls) -> List.flatten (List.map get_vars_disjunctive_formula ls)
  | DNF(ls) -> List.flatten (List.map get_vars_conjunctive_formula ls)
  | General(e) -> get_vars_expr e
and get_vars_disjunctive_formula (d:disjunctive_formula) : variable list =
  match d with
    Disj(ls) -> List.map (fun l -> match l with Atom(v) -> v | NegAtom(v) -> v) ls
  | _ -> []
and get_vars_conjunctive_formula (c:conjunctive_formula) : variable list =
  match c with 
  Conj(ls) -> List.map (fun l -> match l with Atom(v) -> v | NegAtom(v) -> v) ls
  | _ -> []
and get_vars_expr_aux (e:expression) : variable list=
  match e with
  | Literal(Atom(v)) -> [v]
  | Literal(NegAtom(v)) -> [v]
  | Neg(x)   -> get_vars_expr_aux x
  | Or(x,y)  -> (get_vars_expr_aux x ) @ (get_vars_expr_aux y) 
  | MOr(ls)  -> List.flatten (List.map get_vars_expr_aux ls)
  | And(x,y) -> (get_vars_expr_aux x) @ (get_vars_expr_aux y)
  | MAnd(ls) -> List.flatten (List.map get_vars_expr_aux ls)
  | Implies(x,y) -> (get_vars_expr_aux x) @ (get_vars_expr_aux y)
  | Iff(x,y)     -> (get_vars_expr_aux x) @ (get_vars_expr_aux y)
  | _ -> []
  
    
(* this can be more efficient *)  
let formula_to_expression (phi:formula) : expression =
  let rec disjunct_to_expression d =
    match d with
    | FalseDisj -> False
    | TrueDisj  -> True
    | Disj(ls)  -> MOr(List.map (fun l -> Literal(l)) ls)
  and  conjunct_to_expression c =
    match c with
    | FalseConj -> False
    | TrueConj  -> True
    | Conj(ls)  -> MAnd(List.map (fun l -> Literal(l)) ls)
  and apply_let_literal w a l =
    match l with
    | Atom(v)    -> if String.equal v w then a      else Literal(l)
    | NegAtom(v) -> if String.equal v w then Neg(a) else Literal(l)
  and apply_let_expr w a b =
    let rec apply e =
      match e with
	Literal l -> apply_let_literal w a l
      | Neg(x)   -> Neg(apply x) 
      | Or(x,y)  -> Or(apply x, apply y)
      | And(x,y) -> And(apply x, apply y)
      | MOr(ls)  -> MOr(List.map apply ls)
      | MAnd(ls) -> MAnd(List.map apply ls)
      | Implies(x,y) -> Implies(apply x, apply y)
      | Iff (x,y) -> Iff (apply x, apply y)
      | True  -> True
      | False -> False
    in
    apply b
  and apply_let_conjunct w a c =
    match c with
    | FalseConj -> False
    | TrueConj  -> True
    | Conj(ls)  -> MAnd(List.map (fun l->apply_let_literal w a l) ls)
  and apply_let_disjunct w a d =
    match d with
    | FalseDisj -> False
    | TrueDisj  -> True
    | Disj(ls)  -> MOr(List.map (fun l->apply_let_literal w a l) ls)
  and apply_let w a b =
    match b with
    | Let(y,c,d) -> apply_let_expr y (apply_let_expr w a c) (apply_let w a d)
    | CNF(ds)    -> MAnd(List.map (apply_let_disjunct w a) ds)
    | DNF(cs)    -> MOr (List.map (apply_let_conjunct w a) cs)
    | General(e) -> apply_let_expr w a e
  in
  match phi with
  | Let(x,a,b) -> apply_let x a b
  | CNF(ds)    -> MAnd(List.map disjunct_to_expression ds)
  | DNF(cs)    -> MOr(List.map conjunct_to_expression cs)
  | General(e) -> e
