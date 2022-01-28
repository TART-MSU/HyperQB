
(* Functions related with time *)
class timer = object (self)
  val mutable started = Unix.times ()
  val mutable ended = Unix.times ()
  val mutable running = false
  
  method start =
    running <- true;
    started <- Unix.times()
  
  method stop =
    ended <- Unix.times();
    running <- false              
  
  method elapsed_time =
    let ended' = if running then Unix.times() else ended in
    (ended'.Unix.tms_utime +. ended'.Unix.tms_cutime) 
      -. (started.Unix.tms_utime +. started.Unix.tms_cutime)
      
  method proc_elapsed_time =
    let ended' = if running then Unix.times() else ended in
    ended'.Unix.tms_utime -. started.Unix.tms_utime
  
  method timeIt : 'a 'b. ('a -> 'b) -> 'a -> 'b = fun f x ->
    self#start;
    let f_x = f x in
    self#stop;
    f_x
end
