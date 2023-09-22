import sys
import math

"""
This function generates the QCIR encoding for a transition diagram that allows
every state to transition to every other state.

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
    num_helper_vars = bin_digits * 2
    
    # Format
    out.write("#QCIR-G14\n")
    # Generate exists variables
    # These will be the binary digits for each time step
    # ex. k=3 should have
    # x_0 y_0 z_0 ->  1  2  3
    # x_1 y_1 z_1 ->  4  5  6
    # x_2 y_2 z_2 ->  7  8  9
    # x_3 y_3 z_3 -> 10 11 12
    # For multiple traces, index a second "bank" of variables essentially:
    # ex. k=3 traces=2 should have
    # xB_0 yB_0 zB_0 -> 13 14 15
    # xB_1 yB_1 zB_1 -> 16 17 18
    # xB_2 yB_2 zB_2 -> 19 20 21
    # xB_3 yB_3 zB_3 -> 22 23 24
    vars_base = var_index
    vars_list = to_out_list(range(vars_base, vars_base + num_vars))
    out.write(f"exists({vars_list})\n")
    var_index += num_vars

    # Generate forall variables
    # These are the helpers that connect the current time value to the next
    # ex. for digits xyz, you have current xyz and future x'y'z'.
    # x  y  z  = 13 14 15
    # x' y' z' = 16 17 18
    helpers_base = var_index
    helpers_next = helpers_base + bin_digits
    helpers_list = to_out_list(range(helpers_base, helpers_base + num_helper_vars))
    out.write(f"forall({helpers_list})\n")
    var_index += num_helper_vars
    
    # Generate output variable
    # This stores the final result.
    output_base = var_index
    out.write(f"output({output_base})\n")
    var_index += 1

    # Generate variables for transitions
    # Use helper variables to write transitions out
    helpers_current = helpers_base
    # ex. 13, 14, 15
    helpers_next = helpers_base + bin_digits
    # ex. 16, 17, 18
    
    # Write out transition info using helper gates
    # TODO: Move trace_id loop to helper variable generation
    out.write("# Generate transition steps\n")
    src_dest_impls = [] # will store the gate ids corresponding to implications for src to dest
    for src_state in range(0, num_states):
        # Generate source state gate
        helper_src = state_to_bin(src_state, helpers_current)
        helper_src_base = gate_write(f"and({to_out_list(helper_src)})", True)
        
        # Generate all destinations
        helper_dests = [] # will store the gate ids corresponding to destination states
        for dest_state in range(0, num_states):
            helper_dest = state_to_bin(dest_state, helpers_next)
            dest_gate_id = gate_write(f"and({to_out_list(helper_dest)})")
            helper_dests.append(dest_gate_id)
        
        # Generate implication (src -> dest) = or(-src, dests)
        or_all_dest_gate = gate_write(f"or({to_out_list(helper_dests)})")
        src_impl_dest_gate = gate_write(f"or(-{helper_src_base},{or_all_dest_gate})")
        src_dest_impls.append(src_impl_dest_gate)
    
    # All sources and destinations matched: need to "and" these together
    out.write("# Transitions for single time step \n")
    time_step_gate = gate_write(f"and({to_out_list(src_dest_impls)})")
    
    # Match helpers
    out.write("# Match helper variables to time step variables\n")
    match_all_traces = []
    for trace in range(0, traces):
        out.write(f"# Matching for trace {trace}\n")
        match_all_steps = [] # will store the combination checks for T(x,x+1)
        for time in range(0, time_steps):
            out.write(f"# T({time},{time+1}) trace {trace}\n")
            time_vars_base = vars_base + bin_digits * time
            
            # Match current vars
            matches_one_step = [] # will store each "helper_digit == digit[time]" gate id
            for b in range(0, bin_digits):
                time_current_var = time_vars_base + b + num_vars_one_trace * trace
                time_next_var = time_vars_base + bin_digits + b + num_vars_one_trace * trace
                helper_current_var = helpers_base + b
                helper_next_var = helpers_next + b

                # Matches current var and current helper
                match_current_vars_1 = gate_write(f"or(-{time_current_var}, {helper_current_var})")
                match_current_vars_2 = gate_write(f"or({time_current_var}, -{helper_current_var})")
                match_current_vars = gate_write(f"and({match_current_vars_1}, {match_current_vars_2})")
                matches_one_step.append(match_current_vars)

                # Matches next var and next helper
                match_next_vars_1 = gate_write(f"or(-{time_next_var}, {helper_next_var})")
                match_next_vars_2 = gate_write(f"or({time_next_var}, -{helper_next_var})")
                match_next_vars = gate_write(f"and({match_next_vars_1}, {match_next_vars_2})")
                matches_one_step.append(match_next_vars)
            
            # Matches all variables for this time step
            out.write(f"# Match all variables for T({time},{time+1}) trace {trace}\n")
            matches_step_gate = gate_write(f"and({to_out_list(matches_one_step)})")
            match_all_steps.append(matches_step_gate)

        # At least one time step matching is true
        out.write(f"# Combined time steps T(0,{time_steps}) for trace {trace}\n")
        match_some_step = gate_write(f"or({to_out_list(match_all_steps)})")
        # If time step matches, this implies the transition step
        out.write(f"# All time steps for trace {trace} implies transition\n")
        match_step_impl_time_step = gate_write(f"or(-{match_some_step},{time_step_gate})")
        match_all_traces.append(match_step_impl_time_step)
        
    # Start from state zero and above combined traces should be true
    out.write(f"# Conjunct initial condition and all traces\n")
    initial_condition = []
    for var_trace in range(0, traces):
        initial_condition.extend(state_to_bin(0, var_trace * num_vars_one_trace + vars_base))
    out.write(f"{output_base} = and({to_out_list(initial_condition)},{to_out_list(match_all_traces)})\n")

if __name__ == "__main__":

    gen_qcir(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
