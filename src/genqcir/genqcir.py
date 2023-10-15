import sys
import math
import re

VAR_NAME_REGEX = r'# (\d+) : ([a-zA-Z_]+[a-zA-Z0-9]*)_([AB])\[([01])\]'
HELPER_NAME_REGEX = r"# (\d+) : ([a-zA-Z_]+[a-zA-Z0-9]*_[AB]'?)"
MAX_num=0
PREAMBLE="#QCIR-G14\n"
ENCODING_A=""
ENCODING_B=""
OUTPUT=""

NEW_ENCODING_A=False
NEW_ENCODING_B=False

def update_preamble(str, out=sys.stdout):
    global PREAMBLE
    PREAMBLE = PREAMBLE + str + '\n' 

def update_max(num):
    global MAX_num
    if(int(num) > int(MAX_num)):
        MAX_num=int(num)

def write_trans(ts_filename, var_base, trace, out=sys.stdout):
    global ENCODING_A
    global ENCODING_B
    global MAX_num
    ts_file = open(ts_filename, 'r')
    TS = ts_file.readlines()
    gate_num=""
    if(trace=='A'):
        ENCODING_A += ('\n# one step transition of trace '+ trace + '\n')
        # ENCODING_A += ('\n' + trace + '\n')
    else:
        ENCODING_B += ('\n# one step transition of trace '+ trace + '\n')
        # ENCODING_B += ('\n' + trace + '\n')

    # for ts in TS:
    for ts in renumber_model(open(ts_filename), var_base):
        if("output(" in ts or "#" in ts or "exists" in ts or 'forall' in ts):
            continue
        gate_num=(ts[:ts.find('=')].strip())
        update_max(gate_num)
        # out.write(ts)
        if(trace=='A'):
            ENCODING_A += ts+'\n'
        else:
            ENCODING_B += ts+'\n'

    if(trace=='A'):
        ENCODING_A += ('# one step transition of trace ' + trace + ' is: '+ gate_num + '\n\n')
    else:
        ENCODING_B += ('# one step transition of trace ' + trace + ' is: '+ gate_num + '\n\n')
    return int(gate_num)

def get_varlst(ts_filename, out=sys.stdout):
    ts_file = open(ts_filename, 'r')
    TS = ts_file.readlines()
    gate_num=""
    for ts in TS:
        if("exists" in ts or "forall" in ts):
            vars=ts[7:-2]
            vars = vars.replace(" ","")
            vars=list(vars.split(","))
            return vars
            break

def sign(a):
	return (a > 0) - (a < 0)

# Renumbers the model to adjust all variables by new_base.
# Model should be a file named infile stored in QCIR format.
def renumber_model(infile, new_base):
    offset = 0
    for line in infile:
        line = line.strip("\n")
        if "#" in line:
            # check if this comment is a variable name line
            match = re.match(VAR_NAME_REGEX, line)
            if match:
                new_var_id = int(match.group(1)) + new_base
                # new_var_id = new_base
                var_name = match.group(2)
                var_trace = match.group(3)
                helper_step = "'" if match.group(4) == "1" else ""
                yield f"# {new_var_id} : {var_name}_{var_trace}{helper_step}"
            else:
                # do not change comments that do not correspond to variables
                yield line 
            continue
        elif line.startswith(("exists","forall","output")):
            # instruction_type(gate1[, gate2, ...])
            
            line_prefix = line[:line.find("(")+1]
            # offset = int(line[line.find('(')+1: line.find(',')])

            vars = (line[line.find('(')+1: -1].split(','))
            list(map(int, vars))
            offset = int(min(vars))
            # if(new_base != 1):
            new_base -= offset
            print('offset: ' + str(offset))

        elif line and line[0].isdigit():
            # Line is assumed to be of following form:
            # gate_id = gate_type(gate1, gate2[, ...])
            gate_id, gate_actual = line.split("=")
            # gate_id = str(int(gate_id) + new_base)
            gate_id = str(int(gate_id) + new_base)
            gate_actual = gate_actual[:gate_actual.find("(")+1]
            line_prefix = gate_id + " =" + gate_actual
            update_max((gate_id))
        else:
            # Default to not modifying unknown lines
            # print("UNK: "+line)
            yield line
            continue
        
        # This is shared code for any line with gate numbers
        # get just the gate numbers only
        gates = line[line.find("(")+1:line.find(")")].split(",")
        # Convert to integers and offset by new_base (make sure to adjust for '-')
        gates = [sign(int(g))*(abs(int(g)) + new_base) for g in gates]
        # Convert back to strings and combine into new line
        new_line = line_prefix+",".join([str(g) for g in gates])+")"
        yield new_line
    
"""
This function generates the QCIR encoding for a transition diagram that allows
every state to transition to every other state.

num_states determines the number of valid states.
time_steps determines the number of time steps.
The optional output stream determines what file to write to. Default to standard output.
"""
def gen_qcir(dir, time_steps, FORMULA, out=sys.stdout):
    global ENCODING_A
    global ENCODING_B
    global OUTPUT
    global MAX_num
    global NEW_ENCODING_A
    global NEW_ENCODING_B

    # From a python list of any type(s), generate a comma-separated output string
    def to_out_list(in_list):
        return ','.join([str(x) for x in in_list])

    def gate_write(out_gate, trace, reuse_gates=False, gate_value_memo={}):
        global ENCODING_A
        global ENCODING_B

        nonlocal var_index
        if reuse_gates and out_gate in gate_value_memo:
            return gate_value_memo[out_gate]
        # Gate is new, create it and store index
        gate_id = var_index
        # out.write(f"{gate_id} = {out_gate}\n")
        if(trace=='A'):
            ENCODING_A += (f"{gate_id} = {out_gate}\n")
            # print(ENCODING_A)
        else:
            ENCODING_B += (f"{gate_id} = {out_gate}\n")
        # encoding += (f"{gate_id} = {out_gate}\n")
        var_index += 1
        gate_value_memo[out_gate] = gate_id
        update_max(gate_id)
        return gate_id

    def write_newencoding(var_index, time_step_gate, trace, all_vars, helpers_base, helpers_next, helper_vars, var_size):
        global ENCODING_A
        global ENCODING_B
        encoding = ""
        # Match helpers
        # out.write("# Match helper variables to time step variables\n")
        encoding += "# Match helper variables to time step variables\n"
        match_all_traces = []
        # for trace in range(0, traces):
        # out.write(f"# Matching for trace {trace}\n")
        encoding += (f"# Matching for trace {trace}\n")
        match_all_steps = [] # will store the combination checks for T(x,x+1)
        for time in range(0, time_steps):
            # out.write(f"# T({time},{time+1}) of trace {trace}\n")
            if(trace == 'A'):
                ENCODING_A += f"# T({time},{time+1}) of trace {trace}\n"
            else:
                ENCODING_B += f"# T({time},{time+1}) of trace {trace}\n"

            time_vars_base = vars_base + var_size * time   
            # Match current vars
            matches_one_step = [] # will store each "helper_digit == digit[time]" gate id
            for b in range(0, var_size):
                # time_current_var = time_vars_base + b + num_vars_one_trace
                # time_next_var = time_vars_base + num_of_vars + b + num_vars_one_trace
                # helper_current_var = helpers_base + b
                # helper_next_var = helpers_next + b
                # print(all_vars)
                time_current_var = all_vars[time + b]
                time_next_var = all_vars[time + var_size + b]
                
                helper_current_var = helpers_base + b
                helper_next_var = helpers_next + b    

                # Matches current var and current helper
                match_current_vars_1 = gate_write(f"or(-{time_current_var},{helper_current_var})", trace)
                match_current_vars_2 = gate_write(f"or({time_current_var},-{helper_current_var})", trace)
                match_current_vars = gate_write(f"and({match_current_vars_1},{match_current_vars_2})", trace)
                matches_one_step.append(match_current_vars)

                # Matches next var and next helper
                match_next_vars_1 = gate_write(f"or(-{time_next_var},{helper_next_var})", trace)
                match_next_vars_2 = gate_write(f"or({time_next_var},-{helper_next_var})", trace)
                match_next_vars = gate_write(f"and({match_next_vars_1},{match_next_vars_2})", trace)
                matches_one_step.append(match_next_vars)
            
            # Matches all variables for this time step
            # out.write(f"# Match all variables for T({time},{time+1}) trace {trace}\n")
            matches_step_gate = gate_write(f"and({to_out_list(matches_one_step)})", trace)
            match_all_steps.append(matches_step_gate)

        # At least one time step matching is true
        # out.write(f"# Disjunct all T(0,{time_steps}) for trace {trace}\n")
        if(trace == 'A'):
            ENCODING_A += (f"# Disjunct all T(0,{time_steps}) for trace {trace}\n")
        else:
            ENCODING_B += (f"# Disjunct all T(0,{time_steps}) for trace {trace}\n")

        match_some_step = gate_write(f"or({to_out_list(match_all_steps)})", trace)
        # If time step matches, this implies the transition step
        # out.write(f"# All time steps for trace {trace} implies transition\n")
        
        if(trace == 'A'):
            ENCODING_A += (f"# All time steps for trace {trace} implies transition\n")
        else:
            ENCODING_B += (f"# All time steps for trace {trace} implies transition\n")

        match_helpvars = gate_write(f"or(-{match_some_step},{time_step_gate})", trace)
        
        var_index = match_helpvars
        update_max(var_index)
        
        return match_helpvars

    def write_oldencoding(var_index, filename, trace):
        encoding = ""
        encoding += '\n\n'
        encoding += "# old encoding for trace " + trace + '\n'
        # var_index += 1
        # var_index = var_index+2
        with open(filename) as f:
            for newline in renumber_model(f, var_index):
                if ("#" in newline or "output" in newline):
                    continue
                elif ("forall" in newline or "exists" in newline):
                    update_preamble(newline)
                else:
                    # print(newline)
                    # out.write(newline+'\n')
                    encoding += newline+'\n'
                    gate_num = newline[:newline.find(" =")]
        update_max(gate_num)        
        return gate_num, encoding
        # return gate_num

    var_index = 1
    
    MODEL_A = ""
    MODEL_B = ""

    A_var_list = get_varlst(dir+"/A.qcir")
    # A_var_list = to_out_list(A_var_list)
    if(NEW_ENCODING_A):
        A_var_size = len(A_var_list)//2
        
        A_helpers_base = var_index
        A_helpers_next = A_helpers_base + A_var_size
        A_num_helpers = A_var_size * 2
        A_helpers_list = to_out_list(range(A_helpers_base, A_helpers_base + A_num_helpers))
        A_gate = write_trans(dir+"/A.qcir", int(A_helpers_base), 'A')

        var_index += A_num_helpers
        
        vars_base = MAX_num+1
        A_num_state_vars = A_var_size * (time_steps + 1)
        print("size: "+str(A_var_size))

        vars_base += 1
        A_state_vars = range(vars_base, vars_base + A_num_state_vars)
        
        A_var_offset = vars_base + A_num_state_vars
        update_preamble(f"exists({to_out_list(A_state_vars)})")
        update_preamble(f"forall({A_helpers_list})")
        
        var_index = max(A_state_vars) + 1
        MODEL_A = write_newencoding(var_index, A_gate, "A", A_state_vars, A_helpers_base, A_helpers_next, A_helpers_list, A_var_size)
    else:
        MODEL_A, ENCODING_A = write_oldencoding(var_index, dir+"/A.qcir", "A")


    # var_index = int(MODEL_A) + 1

    # MAX_num = int(MODEL_A) + 1
    print(MODEL_A+1)
    # var_index = int(MAX_num) + 1
    var_index = int(MODEL_A) + 5
    print(var_index)

    B_var_list = get_varlst(dir+"/B.qcir")
    # B_var_list = to_out_list(B_var_list)
    if(NEW_ENCODING_B):
        B_var_size = len(B_var_list)//2
        B_helpers_base = var_index
        B_helpers_next = B_helpers_base + B_var_size
        B_num_helpers = B_var_size * 2
        B_helpers_list = to_out_list(range(B_helpers_base, B_helpers_base + B_num_helpers))
        # print(B_helpers_base)
        B_gate = write_trans(dir+"/B.qcir", int(B_helpers_base), 'B')

        var_index += B_num_helpers
        
        vars_base = MAX_num+1
        B_num_state_vars = B_var_size * (time_steps + 1)
        print("size: "+str(B_var_size))

        vars_base += 1
        B_state_vars = range(vars_base, vars_base + B_num_state_vars)

        update_preamble(f"forall({to_out_list(B_state_vars)})")
        update_preamble(f"exists({B_helpers_list})")
        
        var_index = max(B_state_vars) + 1
        MODEL_B = write_newencoding(var_index, B_gate, "B", B_state_vars, B_helpers_base, B_helpers_next, B_helpers_list, B_var_size)
    else:
        MODEL_B, ENCODING_B = write_oldencoding(var_index, dir+"/B.qcir", "B")


    
    # MODEL_B, ENCODING_B = write_oldencoding(var_index, dir+"/B.qcir", "B")

    # prepare output
    output_base = MAX_num+1
    update_preamble(f"output({output_base})")

    # final build OUTPUT
    OUTPUT += PREAMBLE

    ENCODING_A+=("# A trans: "+ str(MODEL_A)+'\n')
    OUTPUT += ENCODING_A
    
    ENCODING_B+=("# B trans: "+ str(MODEL_B)+'\n')
    OUTPUT += ENCODING_B


    
    MAX_num = output_base+1
    with open(dir+"/P.qcir") as f:
        sub = ""
        for newline in (f):
            # print(newline)
            MAX_num += 1
            gate_num = MAX_num
            
            op = newline[newline.find("="):newline.find("(")+1]
            sub += str(gate_num) + op
            curr_vars = newline[newline.find("(")+1:-2]
            curr_vars = curr_vars.split(',')
            for v in curr_vars:
                if (int(v) > A_num_state_vars):
                    # print(v)
                    sub += str(gate_num-1) + ','
                    # MAX_num +=1
                else:
                    if ('-' in v):
                        sub += '-' + str(int(v)+A_var_offset) + ','    
                    else:
                        sub += str(int(v)+A_var_offset) + ','    
                    # v = v.replace('-', "")
                    # sub += A_var_list[int(v)-1] + ','

            sub = sub[:-1] + ")\n"
            # MAX_num += 1
            
    
    # print(sub)
    OUTPUT += "# formula\n"
    OUTPUT += str(MAX_num) + "=or(1,-1)" # temp

    QBF = ""

    # var_index = output_base+1
    var_index = MAX_num+1
    QBF += "\n\n# build EA phi \n"
    # QBF += (f"{var_index} = {FORMULA}\n")
    # var_index+=1
    QBF += (f"{var_index} = or({MODEL_B},{var_index-1})\n")
    var_index+=1
    QBF += (f"{var_index} = and({MODEL_A},{var_index-1})\n")
        
    QBF += (f"# final output\n")
    QBF += (f"{output_base} = and({var_index}) \n")

    OUTPUT += QBF

    outfile = open(dir+"/EAencoding.qcir", "w")
    outfile.write(OUTPUT)
    outfile.close()
    

if __name__ == "__main__":


    FORMULA = "or(1,-1)" # temp



    if (sys.argv[3] == 'T'):
        NEW_ENCODING_A=True
    if (sys.argv[4] == 'T'):
        NEW_ENCODING_B=True

    gen_qcir(sys.argv[1], int(sys.argv[2]), FORMULA)


