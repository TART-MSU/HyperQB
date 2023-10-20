type 't input = Pos of 't | Neg of 't
    
type 't and_gate = 't * 't input * 't input

type 't latch = 't * 't input * bool

type 't symbol =
  Input of  't * string
| Output of 't * string
| Latch of  't * string
  
type aiger_circuit = {
  max : int;
  inputs : int list;
  latches : int latch list;
  outputs : int list;
  ands: int and_gate list;
  symbol_table : string list;
}

  
