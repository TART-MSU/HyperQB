open Expression
open SymbolTable
open Printf
  
  type 't circuit =
    True
  | False
  | Literal of 't * literal
  | Let of  't * ('t circuit) * ('t circuit)
  | Or   of 't * ('t circuit) * ('t circuit)
  | And  of 't * ('t circuit) * ('t circuit)
  | MAnd of 't * ('t circuit list)
  | MOr  of 't * ('t circuit list)
  | Xor  of 't * ('t circuit) * ('t circuit)
  | Ite  of 't * ('t circuit) * ('t circuit) * ('t circuit)
    
(* First version: 
   - gives a name to every node of the expression tree 
   - uses only binary operators
*)

type str_circuit = string circuit    

type quantified_str_circuit = (quantifier list)*str_circuit

(***********************)
(* Converts to circuit *)
(***********************)
let rec formula_to_circuit_fast (f:formula) : str_circuit =
  let last_id = ref 1 in
  let get_new_symbol (unit): int =
    let ret = !last_id in
    last_id := !last_id + 1 ; ret in
  let get_new_name (unit): string =
    "g" ^ (Stdlib.string_of_int (get_new_symbol())) in
  let convert_literal (l:literal) : str_circuit =
    match l with
      Atom(v)    -> Literal(v,l)
    | NegAtom(v) -> Literal("-"^v,l) in
  let rec expression_to_circuit (name:string) (e:expression) : str_circuit =
    let convert = expression_to_circuit "" in
    let f_or n cx cy  = Or(n,cx,cy) in
    let f_and n cx cy = And(n,cx,cy) in
    let f_ite n cx cy = Ite(n,cx,cy,cy) in
    let rec floop n (phi:expression) cont =
      let get_name () = if n="" then get_new_name() else n in
      let nfloop f c = floop "" f c in
      let cont2 e1 e2 f = nfloop e1 (fun res1 -> nfloop e2 (fun res2 -> cont (f res1 res2))) in
      match phi with
        True   -> cont True
      | False  -> cont False
      | Literal(l)   -> cont (convert_literal l)
      | Neg(_)       -> print_endline (expression_to_str e) ; raise(ErrorInNNF)
      | Or(x,y)      -> cont2 x y (f_or (get_name()))
      | And(x,y)     -> cont2 x y (f_and (get_name()))
      | MOr(ls)      -> cont (MOr (get_name(), List.map convert ls))
      | MAnd(ls)     -> cont (MAnd(get_name(), List.map convert ls))
      | Implies(x,y) -> print_endline (expression_to_str e) ; raise(ErrorInNNF)
      | Iff(x,y)     -> cont2 x y (f_ite (get_name()))
    in
    floop name e (fun x->x)
  in
  let  convert_disjunctive_formula (f:disjunctive_formula) : str_circuit =
    match f with
    | FalseDisj -> False
    | TrueDisj  -> True
    | Disj(ls)  -> let ors = List.map convert_literal ls in MOr(get_new_name(),ors) in
  let  cnf_to_circuit (e:disjunctive_formula list) : str_circuit =
    match e with
      [] -> True
    | [l] -> convert_disjunctive_formula l
    | ls  -> let ands = List.map convert_disjunctive_formula ls in MAnd(get_new_name(),ands) in
  let  convert_conjunctive_formula (f:conjunctive_formula) : str_circuit =
    match f with
    | FalseConj -> False
    | TrueConj  -> True
    | Conj(ls)  -> let ands = List.map convert_literal ls in MAnd(get_new_name(),ands)
  in
  let  dnf_to_circuit (e:conjunctive_formula list) : str_circuit =
    match e with
      []  -> True
    | [l] -> convert_conjunctive_formula l
    | ls  -> let ors = List.map convert_conjunctive_formula ls in MOr(get_new_name(),ors)
  in
  let let_to_circuit n x y =
    let ca = expression_to_circuit n x in
    let cb = formula_to_circuit_fast y in
    Let(n,ca,cb)
  in
  match f with
  | Let (n,x,y) -> let_to_circuit n x y
  | CNF e     -> cnf_to_circuit e
  | DNF e     -> dnf_to_circuit e
  | General e -> expression_to_circuit "" e


let rec formula_to_circuit (f:formula) : str_circuit =
  let last_id = ref 1 in
  let get_new_symbol (unit): int =
    let ret = !last_id in
    last_id := !last_id + 1 ; ret in
  let get_new_name (unit): string =
    "g" ^ (Stdlib.string_of_int (get_new_symbol())) in
  let convert_literal (l:literal) : str_circuit =
    match l with
      Atom(v)    -> Literal(v,l)
    | NegAtom(v) -> Literal("-"^v,l) in
  let rec expression_to_circuit (name:string) (e:expression) : str_circuit =
    let convert = expression_to_circuit "" in
    let get_name () = if name="" then get_new_name() else name in
    match e with
      True   -> True
    | False  -> False
    | Literal(l)   -> convert_literal l
    | Neg(_)       -> print_endline (expression_to_str e) ; raise(ErrorInNNF)
    | Or(x,y)      -> let (cx,cy)=(convert x, convert y) in  Or(get_name(),cx,cy)
    | And(x,y)     -> let (cx,cy)=(convert x, convert y) in And(get_name(),cx,cy)
    | MOr(ls)      -> MOr (get_name(), List.map convert ls)
    | MAnd(ls)     -> MAnd(get_name(), List.map convert ls)
    | Implies(x,y) -> print_endline (expression_to_str e) ; raise(ErrorInNNF)
    | Iff(x,y)     -> let (cx,cy)=(convert x, convert y) in Ite(get_name(),cx,cy,cy)
  in
  let  convert_disjunctive_formula (f:disjunctive_formula) : str_circuit =
    match f with
    | FalseDisj -> False
    | TrueDisj  -> True
    | Disj(ls)  -> let ors = List.map convert_literal ls in MOr(get_new_name(),ors) in
  let  cnf_to_circuit (e:disjunctive_formula list) : str_circuit =
    match e with
      [] -> True
    | [l] -> convert_disjunctive_formula l
    | ls  -> let ands = List.map convert_disjunctive_formula ls in MAnd(get_new_name(),ands) in
  let  convert_conjunctive_formula (f:conjunctive_formula) : str_circuit =
    match f with
    | FalseConj -> False
    | TrueConj  -> True
    | Conj(ls)  -> let ands = List.map convert_literal ls in MAnd(get_new_name(),ands)
  in
  let  dnf_to_circuit (e:conjunctive_formula list) : str_circuit =
    match e with
      []  -> True
    | [l] -> convert_conjunctive_formula l
    | ls  -> let ors = List.map convert_conjunctive_formula ls in MOr(get_new_name(),ors)
  in
  let let_to_circuit n x y =
    let ca = expression_to_circuit n x in
    let cb = formula_to_circuit y in
    Let(n,ca,cb)
  in
  match f with
  | Let (n,x,y) -> let_to_circuit n x y
  | CNF e     -> cnf_to_circuit e
  | DNF e     -> dnf_to_circuit e
  | General e -> expression_to_circuit "" e
               


               
let expression_to_circuit e = formula_to_circuit (General(e))
let expression_to_circuit_fast e = formula_to_circuit_fast (General(e))
                            
let cnf_to_circuit e       = formula_to_circuit (CNF(e))
let dnf_to_circuit e       = formula_to_circuit (DNF(e))

(****************************************)
(* Renumbers vars and gates to 1,2,3... *)
(****************************************)
    
let rec tag_vars (e:str_circuit): symbol_table =
  let table = new_table() in
  let rec tag_expr (e:str_circuit) : unit =
      match e with
	Literal(_,Atom(v))    -> tag table v
      | Literal(_,NegAtom(v)) -> tag table v
      | Let(a,x,y)    -> tag table a ; tag_expr x ; tag_expr y
      | Or(_,x,y)     -> tag_expr x ; tag_expr y
      | And(_,x,y)    -> tag_expr x ; tag_expr y
      | MOr(_,ls)     -> List.iter tag_expr ls
      | MAnd(_,ls)    -> List.iter tag_expr ls
      | Xor(_,x,y)    -> tag_expr x ; tag_expr y
      | Ite(_,c,x,y)  -> tag_expr c ; tag_expr x ; tag_expr y
      | _ -> ()
  in
  tag_expr e ; table

let tag_circuit (circ:str_circuit) : symbol_table =
  let table = tag_vars circ in
  let addtag t = tag table t in
  let rec tag_c (circ:str_circuit) : unit =
    match circ with
    | Literal(_) -> ()
    | Let(_,x,y)    -> tag_c x ; tag_c y
    | Or (t,x,y)    -> addtag t ; tag_c x ; tag_c y
    | And(t,x,y)    -> addtag t ; tag_c x ; tag_c y
    | MOr(t,ls)     -> addtag t ; List.iter tag_c ls
    | MAnd(t,ls)    -> addtag t ; List.iter tag_c ls
    | Xor(t,x,y)    -> addtag t ; tag_c x ; tag_c y
    | Ite(t,c,x,y)  -> addtag t ; tag_c c ; tag_c x ; tag_c y
    | _ -> ()
  in
  tag_c circ ; table

let circuit_to_num_circuit (circ:str_circuit) (st:symbol_table) =
  let get_tag t = string_of_int(Hashtbl.find st.tag_to_idx t) in
  let rec trans c =
    match c with
    | Literal(t,Atom(v))    -> let newt=get_tag v in Literal(newt,Atom(newt))
    | Literal(t,NegAtom(v)) -> let newt=get_tag v in Literal("-"^newt,NegAtom(newt))
    | Let(n,x,y)   -> Let(get_tag n, trans x, trans y)
    | Or(t,x,y)    -> Or (get_tag t, trans x, trans y)
    | And(t,x,y)   -> And(get_tag t, trans x, trans y)
    | MAnd(t,ls)   -> MAnd(get_tag t, List.map trans ls)
    | MOr(t,ls)    -> MOr(get_tag t, List.map trans ls) 
    | Xor(t,x,y)   -> Xor(get_tag t, trans x, trans y)
    | Ite(t,c,x,y) -> Ite(get_tag t, trans c,trans x, trans y)
    | True  -> True
    | False -> False
  in
  trans circ
  
(* TO_STR *)
let rec get_name (c:str_circuit) : string =
  match c with
    True         -> "gtrue"
  | False        -> "gfalse"
  | Literal(t,_) -> t
  | Let(t,_,d)   -> get_name d
  | Or(t,_,_)    -> t
  | MOr(t,_)   -> t
  | And(t,_,_)   -> t
  | MAnd(t,_)   -> t
  | Xor(t,_,_)   -> t
  | Ite(t,_,_,_) -> t
    
let circuit_to_str (c: str_circuit) : string =
  let true_used = ref false in
  let false_used = ref false in
  let rec to_str c =
    match c with
      True  -> if !true_used then "" else begin true_used:=true  ; "gtrue =and()\n" end
    | False -> if !false_used then "" else begin false_used:=true ; "gfalse=or()\n" end
    | Literal(s,_) -> ""
    | Let(s,cx,cy) -> sprintf "%s\n%s" (to_str cx) (to_str cy)
    | Or(s,cx,cy)  ->
      let (sx,sy)=(to_str cx,to_str cy) in
      sx ^ sy ^ sprintf "%s = or(%s,%s)\n" s (get_name cx) (get_name cy)
    | And(s,cx,cy)  ->
      let (sx,sy)=(to_str cx,to_str cy) in
      sx ^ sy ^ sprintf "%s = and(%s,%s)\n" s (get_name cx) (get_name cy)
    | MOr(s,ls) ->
      let pre = List.fold_left (fun pref c -> pref ^ (to_str c)) "" ls in
      let args = String.concat "," (List.map get_name ls) in
      pre ^ s ^ " = or(" ^ args ^ ")\n"
    | MAnd(s,ls) ->
      let pre = List.fold_left (fun pref c -> pref ^ (to_str c)) "" ls in
      let args = String.concat "," (List.map get_name ls) in
      pre ^ s ^ " = and(" ^ args ^ ")\n"
    | Xor(s,cx,cy)  ->
      let (sx,sy)=(to_str cx,to_str cy) in
      sx ^ sy ^ sprintf "%s = xor(%s,%s)\n" s (get_name cx) (get_name cy)
    | Ite(s,cx,cy,cz)  ->
      let (sx,sy,sz)=(to_str cx,to_str cy,to_str cz) in
      sx ^ sy ^ sz ^ sprintf "%s = ite(%s,%s,%s)\n" s (get_name cx) (get_name cy)(get_name cz)
  in
  (sprintf "output(%s)\n" (get_name c)) ^ (to_str c)
     
    
(* NOTE: these functions convert circuits, etc to strings, to be dumped later *)
(* it is more efficient to have similar functions that use channels and write into 
   them... channels can be strings or I/O, which would be way more efficient *)
  
let quantified_circuit_to_str (qc:quantified_str_circuit) : string =
  let (qs,c) = qc in
  let quantifier_to_str q =
    match q with
    | Exists vs -> sprintf "exists(%s)\n" (String.concat ", " vs)
    | Forall vs -> sprintf "forall(%s)\n" (String.concat ", " vs)
  in
  let quantifier_prefix =  String.concat "" (List.map quantifier_to_str qs) in
  "#QCIR-G14\n" ^ quantifier_prefix ^ (circuit_to_str c)


  
let quantified_circuit_to_str (qc:quantified_str_circuit): string =
  let (qs,c) = qc in
  let quantifier_to_str q =
    match q with
    | Exists vs -> sprintf "exists(%s)\n" (String.concat ", " vs)
    | Forall vs -> sprintf "forall(%s)\n" (String.concat ", " vs)
  in
  let quantifier_prefix =  String.concat "" (List.map quantifier_to_str qs) in
  "#QCIR-G14\n" ^ quantifier_prefix ^ circuit_to_str c
 

let quantified_circuit_to_num_circuit (qc:quantified_str_circuit) (st:symbol_table)  : quantified_str_circuit =
  let (qs,circ)=qc in
  let trans_var v = string_of_int(Hashtbl.find st.tag_to_idx v) in
  let trans_qs q =
    match q with
      Exists vs -> Exists(List.map trans_var vs)
    | Forall vs -> Forall(List.map trans_var vs)
  in
  let nqs = List.map trans_qs qs in
  let  ncirc = circuit_to_num_circuit circ st in
  (nqs,ncirc)
  
let quantified_num_circuit_to_str (qc:quantified_str_circuit) : string =
  let (qs,circ)=qc in
  let table = tag_circuit circ in
  let (nqs,ncirc) = quantified_circuit_to_num_circuit qc table in
  let str = quantified_circuit_to_str (nqs,ncirc) in
  let print_varsymbol v =
    sprintf "# %s : %s\n" v (Hashtbl.find table.idx_to_tag (int_of_string v)) in
  let print_qs q =
    match q with
      Exists vs -> String.concat "" (List.map print_varsymbol vs)
    | Forall vs -> String.concat "" (List.map print_varsymbol vs)
  in
  str ^ (String.concat "" (List.map print_qs nqs))


(* OUTPUT *)
let rec fprint_circuit ch circ =
  let true_used  = ref false in
  let false_used = ref false in
  let pr s = output_string ch s in
  let rec pr_circ circ =
    match circ with
      True  -> if !true_used then () else
                 begin true_used:=true  ; pr "gtrue =and()\n" end
    | False -> if !false_used then () else
                 begin false_used:=true ; pr "gfalse=or()\n" end
      | Literal(s,_) -> ()
      | Let(n,cx,cy) -> pr_circ cx ; pr_circ cy
      | Or(s,cx,cy)  ->
	let me = sprintf "%s = or(%s,%s)\n" s (get_name cx) (get_name cy)in
	pr_circ cx ; pr_circ cy ; pr me 
      | And(s,cx,cy)  ->
	let me = sprintf "%s = and(%s,%s)\n" s (get_name cx) (get_name cy)in
	pr_circ cx ; pr_circ cy ; pr me 
      | MOr(s,ls) ->
	let args = String.concat "," (List.map get_name ls) in
	let me = s ^ " = or(" ^ args ^ ")\n" in
	List.iter (fun c -> pr_circ c) ls ; pr me
      | MAnd(s,ls) ->
	let args = String.concat "," (List.map get_name ls) in
	let me = s ^ " = and(" ^ args ^ ")\n" in
	  List.iter (fun c -> pr_circ c) ls ; pr me
      | Xor(s,cx,cy)  ->
	let me = sprintf "%s = xor(%s,%s)\n" s (get_name cx) (get_name cy)in
	pr_circ cx ; pr_circ cy ; pr me 
      | Ite(s,cx,cy,cz)  ->
	let me = sprintf "%s = ite(%s,%s,%s)\n" s (get_name cx) (get_name cy)(get_name cz) in
	pr_circ cx ; pr_circ cy ; pr_circ cz ; pr me
  in
  output_string ch (sprintf "output(%s)\n" (get_name circ)) ; pr_circ circ

 
  
let fprint_quantified_circuit ch qc =
  let (qs,circ) = qc in
  let fprint_quantifiers q =
    match q with
    | Exists vs -> output_string ch (sprintf "exists(%s)\n" (String.concat ", " vs))
    | Forall vs -> output_string ch (sprintf "forall(%s)\n" (String.concat ", " vs))
  in
  let _ = output_string ch "#QCIR-G14\n" in
  let _ = List.iter fprint_quantifiers qs in
  fprint_circuit ch circ
  
let fprint_quantified_num_circuit ch qc =
  let (qs,circ)=qc in
  let table = tag_circuit circ in
  let (nqs,ncirc) = quantified_circuit_to_num_circuit qc table in
  let fprint_varsymbol v =
    output_string ch (sprintf "# %s : %s\n" v (Hashtbl.find table.idx_to_tag (int_of_string v))) in
  let print_qs q =
    match q with
      Exists vs -> List.iter fprint_varsymbol vs
    | Forall vs -> List.iter fprint_varsymbol vs
  in
  fprint_quantified_circuit ch (nqs,ncirc) ; List.iter print_qs nqs
  
