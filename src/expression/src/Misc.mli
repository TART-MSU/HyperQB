class timer : object 
  method start : unit
  method stop : unit
  method elapsed_time : float
    (** @return the execution time of the wrapped code, 
        including the execution time for children processes. *)  
  
  method proc_elapsed_time : float
    (** @return the execution time of the wrapped code,
        excluding any other execution time. *)

  method timeIt : ('a -> 'b) -> 'a -> 'b
    (** [timeIt f x] computes [f x] and times it. 
        
        @return [f x]
    *)
end
