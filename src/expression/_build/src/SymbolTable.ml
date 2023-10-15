type symbol_table = {
  last_index : int ref ;
  tag_to_idx : (string,int) Hashtbl.t;
  idx_to_tag : (int,string) Hashtbl.t
}


  
let get_new_index (st:symbol_table) : int =
  let ret = !(st.last_index) in
  st.last_index := !(st.last_index) + 1 ; ret
 
(* Tzu-Han: change last_index = ref from 0 to 1 *)  
let new_table(): symbol_table =
  { last_index = ref 1 ;
    tag_to_idx = Hashtbl.create 1024;
    idx_to_tag = Hashtbl.create 1024
  }

let insert (st:symbol_table) (t:string) : unit =
  let i = get_new_index st in
  Hashtbl.add st.idx_to_tag i t ;
  Hashtbl.add st.tag_to_idx t i
   
let tag (st:symbol_table) (t:string) : unit =
  if (not (Hashtbl.mem st.tag_to_idx t)) then insert st t
