import sys
import math

"""
This function generates the QCIR encoding for a transition diagram that allows
every state to transition to every other state.

This encoding is the one with no helper variables.

num_states determines the number of valid states.
time_steps determines the number of time steps.
The optional output stream determines what file to write to. Default to standard output.
"""
def gen_qcir(time_steps, num_states=8, traces=1, out=sys.stdout):
    # Convert a number into a logical representation of its binary digits
    # ex. 
    # For digits x,y,z, with digit_base = 13
    # dec 6 -> bin 110 -> [13, 14, -15]
    def state_to_bin(state_no, digit_base):
        return [(1 if state_no & (1<<(bin_digits-b-1)) else -1)*(digit_base + b) for b in range(0, bin_digits)]
    
    # From a python list of any type(s), generate a comma-separated output string
    def to_out_list(in_list):
        return ','.join([str(x) for x in in_list])

    # out_gate should be an expression such as "and(1,2,3)" or "or(-4,5)".
    # If the value has been used before, don't output a new gate and instead reuse the old gate id.
    # Returns the gate_id used, so that further gates can be constructed referencing out_gate.
    def gate_write(out_gate, reuse_gates=False, gate_value_memo={}):
        nonlocal var_index
        if reuse_gates and out_gate in gate_value_memo:
            return gate_value_memo[out_gate]
        # Gate is new, create it and store index
        gate_id = var_index
        out.write(f"{gate_id} = {out_gate}\n")
        var_index += 1
        gate_value_memo[out_gate] = gate_id
        return gate_id

    var_index = 1
    # Get the minimum number of binary digits required to identify a state
    bin_digits = math.ceil(math.log(num_states, 2))
    # Get the number of boolean variables required
    num_vars_one_trace = bin_digits * (time_steps + 1)
    num_vars = num_vars_one_trace * traces
   
    # Format
    out.write("#QCIR-G14\n")
    # Generate exists variables
    # These will be the binary digits for each time step
    # ex. k=3 should have
    # x_0 y_0 z_0 ->  1  2  3
    # x_1 y_1 z_1 ->  4  5  6
    # x_2 y_2 z_2 ->  7  8  9
    # x_3 y_3 z_3 -> 10 11 12
    vars_base = var_index
    vars_list = to_out_list(range(vars_base, vars_base + num_vars))
    out.write(f"exists({vars_list})\n")
    var_index += num_vars
   
    # Generate output variable
    # This stores the final result.
    output_base = var_index
    out.write(f"output({output_base})\n")
    var_index += 1

    # Write out transition info for each time step
    out.write("# Generate transition steps\n")
    trace_gates = [] # Conjunction gates for one entire trace
    for trace in range(0, traces):
        out.write(f"# Transitions for trace {trace}\n")
        
        time_step_gates = [] # Gates for each time step
        for time in range(0, time_steps):
            out.write(f"# Transitions for T({time},{time+1}) trace {trace}\n")
            vars_current_base = vars_base + time * bin_digits + num_vars_one_trace * trace
            vars_next_base = vars_current_base + bin_digits
            
            src_dest_impls = [] # will store the gate ids corresponding to implications for src to dest
            for src_state in range(0, num_states):
                out.write(f" # Transition from: {src_state}\n")
                # Generate source state gate
                src_var = state_to_bin(src_state, vars_current_base)
                src_var_gate = gate_write(f"and({to_out_list(src_var)})")
                
                # Generate all destinations
                var_dests = [] # will store the gate ids corresponding to destination states
                for dest_state in range(0, num_states):
                    dest_var = state_to_bin(dest_state, vars_next_base)
                    dest_var_gate = gate_write(f"and({to_out_list(dest_var)})")
                    var_dests.append(dest_var_gate)
                
                # Generate implication (src -> dest) = or(-src, dests)
                or_all_dest_gate = gate_write(f"or({to_out_list(var_dests)})")
                src_impl_dest_gate = gate_write(f"or(-{src_var_gate},{or_all_dest_gate})")
                src_dest_impls.append(src_impl_dest_gate)
                out.write("\n")
            
            # All sources and destinations matched: need to "and" these together
            out.write(f"# End transitions for T({time},{time+1}) trace {trace}\n")
            time_step_gate = gate_write(f"and({to_out_list(src_dest_impls)})")
            time_step_gates.append(time_step_gate)
        
        # Conjunction of all time steps
        out.write(f"# Conjunct T(0,{time_steps})\n")
        all_steps = gate_write(f"and({to_out_list(time_step_gates)})")
        trace_gates.append(all_steps)
    
    # Start from state zero and above should be true
    out.write(f"# Conjunct initial condition and all traces\n")
    out.write(f"{output_base} = and({to_out_list(state_to_bin(0,vars_base))},{to_out_list(trace_gates)})\n")

if __name__ == "__main__":
    gen_qcir(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
