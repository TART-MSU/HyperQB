import re
import sys
import time

###############################################################
#        This script build the follwing form of trajectories
#
#        forall pi. forall pi'. E tau.
#              ((in_{pi, tau} = in_{pi', tau}))
#                       ==> G((out_{pi, tau} = out_{pi', tau}))
###############################################################

### logical operators
IFF = " IFF "
AND = " AND "
OR = " OR "
NOT = " -"

# extensions
M1 = "_A"
M2 = "_B"

### parameters setup
path1_termination_flag = "halt"
path2_termination_flag = "halt"


# obs_vars_path1 = ["public_out_1", "public_out_0"]
# obs_vars_path2 = ["public_out_1", "public_out_0"]
obs_vars_path1 = ["secret_out_0", "public_out_0"]
obs_vars_path2 = ["secret_out_0", "public_out_0"]
num_observables = len(obs_vars_path1)

# low_vars_path1 = ["LOW"]
# low_vars_path2 = ["LOW"]
# num_low_vars = len(low_vars_path1)


# allow_00=True
allow_00=False


tau_path1_diameter =int(sys.argv[1])
tau_path2_diameter =int(sys.argv[2])
len_longest_trajectory = int(sys.argv[3])
# len_longest_trajectory = tau_path1_diameter + tau_path2_diameter
# len_longest_trajectory = int(sys.argv[3])

print("diameter_tau_path1: ", tau_path1_diameter)
print("diameter_tau_path2: ", tau_path2_diameter)
print("len_longest_trajectory: ", len_longest_trajectory)
start = time.time()


### ALL HELPER METHODS ###

### little helper
def timestamp(t):
    return "["+str(t)+"]"

### build QCIR gates:
def build_AND2(a, b):
    global var_index
    index = var_index
    s = str(index)+" = and("+str(a)+ ","+ str(b)+")"
    var_index+=1
    NEW_QCIR.append(s)
    # return s
    return index

def build_AND3(a, b, c):
    global var_index
    index = var_index
    s = str(index)+" = and("+str(a)+","+str(b)+","+str(c)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return index

def build_AND4(a, b, c, d):
    global var_index
    index = var_index
    s = str(index)+" = and("+str(a)+","+str(b)+","+str(c)+","+str(d)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return  index

def build_AND5(a, b, c, d, e):
    global var_index
    index = var_index
    s = str(index)+" = and("+str(a)+","+str(b)+","+str(c)+","+str(d)+","+str(e)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return  index

def build_AND_multi(list):
    global var_index
    index=var_index
    s = str(index)+ " = and("
    for l in list:
        s = s + str(l) + ","
    s = s[:-1]
    s = s + ")"
    var_index+=1
    NEW_QCIR.append(s)
    # print(s)
    # print(index)
    return index

def build_OR_multi(list):
    global var_index
    index=var_index
    s = str(index)+ " = or("
    for l in list:
        s = s + str(l) + ","
    s = s[:-1]
    s = s + ")"
    var_index+=1
    NEW_QCIR.append(s)
    # print(s)
    # print(index)
    return index

def build_OR2(a,b):
    global var_index
    index =  var_index
    s = str(index)+" = or("+str(a)+","+str(b)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return index

def build_OR3(a,b,c):
    global var_index
    index = var_index
    s = str(index)+" = or("+str(a)+","+str(b)+","+str(c)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return index

def build_OR4(a,b,c,d):
    global var_index
    index = var_index
    s = str(index)+" = or("+str(a)+","+str(b)+","+str(c)+","+str(d)+")"
    var_index+=1
    NEW_QCIR.append(s)
    return index
    # print(s)

def build_IFF(a,b):
    global var_index
    not_a = "-"+str(a)
    not_b = "-"+str(b)
    index1=var_index
    LR=build_OR2(not_a, b)
    index2=var_index
    RL=build_OR2(not_b, a)
    # print(LR)
    # print(RL)
    index = var_index
    s = str(index) + " = and(" + str(index1) +"," + str(index2) +")"
    # print(s)
    NEW_QCIR.append(s)
    var_index+=1
    return index

def build_IMPLIES(a, b):
    global var_index
    index = var_index
    not_a = "-"+str(a)
    s = str(index) + " = or(" + str(not_a) + "," + str(b) + ")"
    # print(s)
    NEW_QCIR.append(s)
    var_index+=1
    return index

def build_exclusive_OR(lst):
    picks=[]
    for v_picked in lst:
        pick_v=[]
        for v in lst:
            if (v==v_picked):
                # pick_v.append(str(v))
                pick_v.append(str(var_dict[v]))
            else:
                # pick_v.append(NOT+str(v))
                pick_v.append(NOT+str(var_dict[v]))
        exclusive_pick_v = build_AND_multi(pick_v)
        picks.append(exclusive_pick_v)

    index = build_OR_multi(picks)
    return index

### get time
def get_timestamp(s):
    s = s.split("[")
    s = s[1].replace("]", "")
    s = int(s)
    return s

### FILE READ and WRITE
open_QCIR = open("HQ.qcir", "r")
QCIR=open_QCIR.read()
vars = re.findall('#\s.*', QCIR)

### NEW GATES ADDED to NEW QCIR
NEW_QCIR = []


## a little helper
def get_phi(t1, t2, time):
    # return "phi"+str(t1)+str(t2)+"["+str(time)+"]"
    return "pos_"+str(t1)+"_"+str(t2)+"_["+str(time)+"]"






### BUILD TRAJECTORY and ITS TRANSITIONS
# this function recursively find all possible reachable trajectory options with four possible
# moves for two concurrent threads: 00, 01, 10, 11
# -----------------------------------------
# for example, tau_phi00[0] is the initial state
# tau_phi10[1] is a reachable trajectory option, with program 1 moves and program 2 doesn't
# however, tau_phi22[1] is an unreachable trajectory option, since it's impossible for both programs
# to move two steps within only one time unit.
# ------------------------------------------
# The built transitions are presented in the following formats:
# (curr_layer, curr_t1, curr_t2, t1&t2(movements), next_layer, next_t1, next_t2)

def build_traj_advances(layer, t1, t2, diameter_path1, diameter_path2, valid_traj_options, traj_relations):
    ### collect valid trajectory options (i.e., valid trajectory)
    # valid_traj_options_tau.append([layer, t1, t2])
    if (not(str(layer) in valid_traj_options)):
        valid_traj_options[str(layer)] = []
    if ([t1, t2] not in valid_traj_options[str(layer)]):
        valid_traj_options[str(layer)].append([t1, t2])

    next_layer=layer+1 # next layer

    # Last layer
    if (layer==len_longest_trajectory):
        # traj_relations.append([layer, t1, t2])
        traj_relations.add((layer, t1, t2))
        return
    else:
        ## calculate next positions for both threads with four possible trajevtories
        t1_not_move = t1
        t2_not_move = t2

        if (t1 < diameter_path1):
            t1_move = t1 + 1
        else: t1_move = t1
        if (t2 < diameter_path2):
            t2_move = t2 + 1
        else: t2_move = t2

        ### let's ban this to make the exploration more efficient
        # 00 situation
        if(allow_00):
            traj_relations.add((layer, t1, t2, "00", next_layer, t1_not_move, t2_not_move))

        ## 01 situation
        traj_relations.add((layer, t1, t2, "01", next_layer, t1_not_move, t2_move))

        ## 10 situation
        traj_relations.add((layer, t1, t2, "10", next_layer, t1_move, t2_not_move))

        ## 11 situation
        traj_relations.add((layer, t1, t2, "11", next_layer, t1_move, t2_move))

        ## recursivly try all four directions for the next time step
        if(allow_00):
            build_traj_advances(next_layer, t1_not_move, t2_not_move, diameter_path1, diameter_path2, valid_traj_options, traj_relations) ## banned 00 for efficiency purpose
        build_traj_advances(next_layer, t1_not_move, t2_move, diameter_path1, diameter_path2, valid_traj_options, traj_relations) # 01
        build_traj_advances(next_layer, t1_move, t2_not_move, diameter_path1, diameter_path2, valid_traj_options, traj_relations) # 10
        build_traj_advances(next_layer, t1_move, t2_move, diameter_path1, diameter_path2, valid_traj_options, traj_relations) # 11

        if(not allow_00):
            traj_relations.add((layer, t1, t2, "00")) ## '00' woll not advance

### ---DEBUG---
# print(final_formulas)
# print(len(traj_relations))
### small debug print
# print("trajectory options built, in total: ", str(len(tau_traj_relations)))
# print(*tau_traj_relations,sep="\n")
### small debug print
# print("trajectory options built, in total: ", str(len(tau2_traj_relations)))
# print(*tau2_traj_relations,sep="\n")



#### var_dict is a dictionary containing all variables with their numerical representation
#### format--> VAR_NAME: VAR_NUMBER
var_dict = {}
### set up all variables needed, including the trajectory variables and formula variables
obs_keyword="obs"
def variable_setup(tau, tau_valid_traj_options):
    ### Build a dictionary to keep tract of the assigned number for each variables
    global var_dict
    for var in vars:
        var = (var.split(" "))
        var_dict[var[3]] = var[1];

    ### new_vars is an array for all New Variables
    # including trajectory varariables and the constraints formula
    global new_vars
    ### BUILD TRAJECTORY VARIABLES
    global traj_vars_tau
    for i in range(len_longest_trajectory):
        tau_bits = []
        tau_bits.append("tau_t1["+str(i)+"]")
        tau_bits.append("tau_t2["+str(i)+"]")
        traj_vars_tau.append(tau_bits)

        ### save it to assign new numbers
        new_vars.append("tau_t1["+str(i)+"]")
        new_vars.append("tau_t2["+str(i)+"]")

    ### ---DEBUG---
    # print(var_dict)
    # print(traj_vars_tau)
    # print(traj_vars_tau2)

    ### NEW build relational constraints formulas for tau
    ### (t1, t2, conditions....)
    ## build tau_1 constraints
    ## OLD
    # for i in range(tau_path1_diameter+1):
    #     for j in range(tau_path2_diameter+1):
    #         for k in range(len_longest_trajectory+1):
    #             name = tau+get_phi(i,j,k)
    #             new_vars.append(name)

    ## NEW
    for j in tau_valid_traj_options:
        for pos in tau_valid_traj_options[j]:
            name = tau+get_phi(pos[0], pos[1], j)
            # print(name)
            new_vars.append(name)


    ### Finally, assign new numbers for each variable to encode as QCIR format
    # 1. get the index of gates and variables
    nums = re.findall(r'\d+', QCIR)
    nums = list(map(int, nums))
    nums.sort()
    # print(nums)
    # 2. get the starting index of the extra variables
    global var_index
    var_index = nums[-1]+1
    # print(nums)
    # print("DEBUG")
    # print(var_index)

    # 3. add all new variables into var_dict as new dictionary items
    for v in new_vars:
        var_dict[v] = str(var_index)
        # NEW_QCIR.append("# "+ str(var_index) + " : " + v)
        var_index+=1

    # print("======= (variable setup finished) =========")
    ### ---DEBUG---
    # print(*observable_vars_formulas, sep="\n")
    # print(observable_vars_formulas)
    # print(*new_vars, sep="\n")s
    # print('==')
    # print(tau_formulas)
    # print(tau2_formulas)







## a little helper
def get_trajvar_expr(tau,t, time, movement):
    s = tau+t+"["+str(time)+"]"
    if (movement=='0'):
        s="-"+s
    return s

### convert the trajectory relations into given naming
def build_expressions(tau, final_expressions, traj_relations):
    for i in range(0, len(traj_relations), 4):
        r = traj_relations[i]
        print(r)
        if (len(r) == 3):
            for k in range(i, len(traj_relations)):
                r = traj_relations[k]
                # print(r)
                time = r[0]
                formula_t1=r[1]
                formula_t2=r[2]
                traces_timesteps= [formula_t1, formula_t2]
                pre = [get_phi(formula_t1, formula_t2, time), traces_timesteps]
                # pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", "phi"+ str(formula_t1)+str(formula_t2)]
                final_expressions.append(pre)
            ### finishing base case, break
            break
        else:
            time = r[0]
            formula_t1= r[1]
            formula_t2= r[2]
            traces_timesteps= [formula_t1, formula_t2]
            # pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", traces_timesteps]
            pre = [get_phi(formula_t1, formula_t2, time), traces_timesteps]
            next_move = []

            ### the "00 " case
            # next_move.append(i+1)
            # post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
            ## remove
            # post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
            # next_move.append(post)
            if (allow_00):
                for j in range(4):
                    # next = traj_relations[i+j+1]
                    next = traj_relations[i+j]
                    # print(next)
                    movement=next[3]
                    next_time=next[4]
                    next_formula_t1=next[5]
                    next_formula_t2=next[6]

                    t1 = get_trajvar_expr(tau, "t1", time, movement[0])
                    t2 = get_trajvar_expr(tau, "t2", time, movement[1])
                    # post = [t1, t2, "phi"+str(next_formula_t1)+str(next_formula_t2)+"["+ str(next_time)+"]"]
                    post = [t1, t2, get_phi(next_formula_t1, next_formula_t2, next_time)]
                    next_move.append(post)

                pre.extend(next_move)
                final_expressions.append(pre)

            if (not allow_00):
                for j in range(3):
                    next = traj_relations[i+j+1]
                    # next = traj_relations[i+j]
                    # print(next)
                    movement=next[3]
                    next_time=next[4]
                    next_formula_t1=next[5]
                    next_formula_t2=next[6]

                    t1 = get_trajvar_expr(tau, "t1", time, movement[0])
                    t2 = get_trajvar_expr(tau, "t2", time, movement[1])
                    # post = [t1, t2, "phi"+str(next_formula_t1)+str(next_formula_t2)+"["+ str(next_time)+"]"]
                    post = [t1, t2, get_phi(next_formula_t1, next_formula_t2, next_time)]
                    next_move.append(post)

                pre.extend(next_move)
                final_expressions.append(pre)


### check the halting conditions
# halt_M1_t1 = var_dict["path1-t1_halt_A[0]"]
# halt_M1_t2 = var_dict["path1-t2_halt_A[0]"]
# halt_M2_t1 = var_dict["path1-t1_halt_B[0]"]
# halt_M2_t2 = var_dict["path1-t2_halt_B[0]"]


## a smaller helper
def convert_trajvar_to_number(trajvar):
    if ("-" in trajvar):
        temp = trajvar.replace("-","")
        temp = "-"+str(var_dict[temp])
        s = temp
    else:
        s = str(var_dict[trajvar])

    return s

### QCIR build
### convert all expressions into QCIR format
# formula_cond = []
# dict_choices_eachlayer={}
To_find = [] # the trajectory constraint options to be find
def convert_to_qcir(tau, expressions, t1_ext, t2_ext, formula_cond, dict_choices_eachlayer):
    counter=0
    layer_choices=set()

    for e in expressions:
        # print(e)
        # time_t1 = e[0][3]
        time_t1= e[1][0]
        # print(time_t1)

        # time_t2 = e[0][4]
        time_t2= e[1][1]
        # print(time_t2)

        # exp=e
        name = tau+e[0]
        To_find.append(name)
        formula_antecedent = var_dict[name]

        curr_time = int(get_timestamp(e[0]))
        # print(curr_time)
        if (int(curr_time) > 0):
            for s in c_set:
                layer_choices.add(s)

            if ((curr_time) != int(counter)):
                dict_choices_eachlayer[tau+str(counter)] = list(layer_choices)
                layer_choices=set()
                counter +=1

        # some debug flag
        # NEW_QCIR.append("# exp:")
        NEW_QCIR.append("# "+ str(name))
        # NEW_QCIR.append("# "+ e)

        # print(name)
        if (len(e)==2):
            ### if it's the last layer
            # ending_group.add(tau+exp[0])
            continue;
        else:
            choices = []
            c_set = set()
            ## move 00
            ### EXISTS, to exclude 00
            # choices.append(["-"+str(var_dict[exp[2][1]]), "-"+str(var_dict[exp[2][2]]), NOT+str(var_dict["TRUE_A[0]"]) ])
            ### FORALL
            # choices.append(["-"+str(var_dict[exp[2][1]]), "-"+str(var_dict[exp[2][2]]), str(var_dict["TRUE_A[0]"]) ])
            ### try
            # choices.append(["-"+str(var_dict[e[2][1]]), "-"+str(var_dict[e[2][2]]) ])

            if(allow_00):
                ## move 00, 01, 10, 11
                for i in range(2,6):
                    move = e[i]
                    ###  extract t1
                    a = convert_trajvar_to_number(move[0])
                    ###  extract t2
                    b = convert_trajvar_to_number(move[1])
                    # if (int(get_timestamp(exp[0]))s != (len_longest_trajectory-2)):
                    c = str(var_dict[tau+move[2]])
                    c_set.add(tau+move[2])
                    # choices.append([a,b,c])
                    choices.append([a,b,c])
                    # else:s
                        # choices.append([a,b])
                    # build_AND3(a, b, c)
                c_list = list(c_set)

                choice_1 = build_AND_multi(choices[0]) #00
                choice_2 = build_AND_multi(choices[1]) #01
                choice_3 = build_AND_multi(choices[2]) #10
                choice_4 = build_AND_multi(choices[3]) #11

                selection = []
                choices = [choice_1, choice_2, choice_3, choice_4]
                free_choices = build_OR_multi(choices)
                selection.append(free_choices)


                selections = build_AND_multi(selection)
                formula = build_IMPLIES(formula_antecedent, selections)
                # formula = build_IMPLIES(formula_antecedent, free_choices)
                # index = build_IFF(formula_antecedent, selection) ## this is wrong
                formula_cond.append(formula)

            if(not allow_00):
                ## move 01, 10, 11
                for i in range(2,5):
                    move = e[i]
                    ###  extract t1
                    a = convert_trajvar_to_number(move[0])
                    ###  extract t2
                    b = convert_trajvar_to_number(move[1])
                    # if (int(get_timestamp(exp[0]))s != (len_longest_trajectory-2)):
                    c = str(var_dict[tau+move[2]])
                    c_set.add(tau+move[2])
                    # choices.append([a,b,c])
                    choices.append([a,b,c])
                    # else:s
                        # choices.append([a,b])
                    # build_AND3(a, b, c)
                c_list = list(c_set)

                choice_1 = build_AND_multi(choices[0]) #01
                choice_2 = build_AND_multi(choices[1]) #10
                choice_3 = build_AND_multi(choices[2]) #11


                selection = []
                choices = [choice_1, choice_2, choice_3]
                free_choices = build_OR_multi(choices)
                selection.append(free_choices)


                selections = build_AND_multi(selection)
                formula = build_IMPLIES(formula_antecedent, selections)
                # formula = build_IMPLIES(formula_antecedent, free_choices)
                # index = build_IFF(formula_antecedent, selection) ## this is wrong
                formula_cond.append(formula)


# valid_traj_options_tau.sort()
# valid_traj_options_tau2.sort()


### ---DEBUG---
# print(final_formulas)
# print("here")
# # print(*valid_traj_options_tau,sep="\n")
# print(valid_traj_options_tau)
# print("here2")
# print(*valid_traj_options_tau2,sep="\n")
# for key, value in valid_traj_options_tau:
#     print(key)


#### BUILD RELATIONAL CONDITIONS for OBSERBVABLE VARIABLES ####
### build relational constraints over all observable variables for tau
# valid_traj_options_tau.sort()
# valid_traj_options_tau2.sort()
# observable_vars_formulas = {}
# always_formulas ={}
# all_formula_pairs = []
def build_realtional_conditions(tau, tau2):
    # global observable_vars_formulas
    global all_formula_pairs
    gates_dict = {}
    for i in range(len_longest_trajectory+1):
        # print(tau_valid_traj_options[str(i)])
        # print(tau2_valid_traj_options[str(i)])
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        curr_time_step_tau2 = tau_valid_traj_options[str(i)]
        for A in curr_time_step_tau:
            for B in curr_time_step_tau2:
                tau_name = tau+get_phi(A[0], A[1], i)
                tau2_name = tau2+get_phi(B[0], B[1], i)
                NEW_QCIR.append("# key:"+tau_name+tau2_name)
                key_index = build_AND2(var_dict[tau_name], var_dict[tau2_name])
                # NEW_QCIR.append("# value")
                # print(A, AND, B)
                all_observable_pairs = []
                for k in range(num_observables):
                    proc_1 = [obs_vars_path1[k]+M1+timestamp(A[0]), obs_vars_path1[k]+M2+timestamp(B[0])]
                    proc_2 = [obs_vars_path2[k]+M1+timestamp(A[1]), obs_vars_path2[k]+M2+timestamp(B[1])]
                    #
                    a = proc_1[0]
                    b = proc_1[1]
                    if ((a+IFF+b) in gates_dict):
                        num=gates_dict[a+IFF+b]
                    else:
                        num = build_IFF(var_dict[a], var_dict[b])
                        gates_dict[a+IFF+b]=num
                    all_observable_pairs.append(num)

                    a = proc_2[0]
                    b = proc_2[1]
                    if ((a+IFF+b) in gates_dict):
                        num=gates_dict[a+IFF+b]
                    else:
                        num = build_IFF(var_dict[a], var_dict[b])
                        gates_dict[a+IFF+b]=num
                    all_observable_pairs.append(num)

                RS = build_AND_multi(all_observable_pairs)
                var_pair_formula = build_IMPLIES(key_index, RS)
                all_formula_pairs.append(var_pair_formula)

def build_inner_realtional_conditions(tau_name, ext, tau_all_formula_pairs):
    # global observable_vars_formulas
    global all_formula_pairs
    gates_dict = {}
    for i in range(len_longest_trajectory+1):
        # print(tau_valid_traj_options[str(i)])
        # print(tau2_valid_traj_options[str(i)])
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        # curr_time_step_tau2 = tau_valid_traj_options[str(i)]
        for A in curr_time_step_tau:
            # for B in curr_time_step_tau2:
            name = tau_name+get_phi(A[0], A[1], i)
            # tau2_name = tau2+get_phi(B[0], B[1], i)
            NEW_QCIR.append("# key:"+name)
            # key_index = build_AND2(var_dict[tau_name], var_dict[tau2_name])
            key_index = var_dict[name]
            # NEW_QCIR.append("# value")
            # print(A, AND, B)
            all_observable_pairs = []
            for k in range(num_observables):
                procs = [obs_vars_path1[k]+ext+timestamp(A[0]), obs_vars_path2[k]+ext+timestamp(A[1])]

                #
                a = procs[0]
                b = procs[1]
                if ((a+IFF+b) in gates_dict):
                    num=gates_dict[a+IFF+b]
                else:
                    num = build_IFF(var_dict[a], var_dict[b])
                    gates_dict[a+IFF+b]=num
                all_observable_pairs.append(num)



            RS = build_AND_multi(all_observable_pairs)
            var_pair_formula = build_IMPLIES(key_index, RS)
            tau_all_formula_pairs.append(var_pair_formula)

def build_terminating_traj(tau, eventually_halt, ext):
    # global observable_vars_formulas
    # global eventaully_halt
    # gates_dict = {}
    for i in range(len_longest_trajectory+1):
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        # print(curr_time_step_tau)
        for curr in curr_time_step_tau:
        # for B in curr_time_step_tau2:
            # tau_name = "tau_phi"+str(curr[0])+str(curr[1])+"["+str(i)+"]"
            tau_name = tau+get_phi(curr[0], curr[1], i)
            # NEW_QCIR.append("# key:"+tau_name)
            # print(tau_name)
            key_index = (var_dict[tau_name])
            # print(key_index)
            NEW_QCIR.append("# value")

            # both_terminated = [termination_flag+M1+timestamp(curr[0]), termination_flag+M2+timestamp(curr[1])]
            path1_term = path1_termination_flag+ext+timestamp(curr[0])
            path2_term = path2_termination_flag+ext+timestamp(curr[1])
            both_term = build_AND2(var_dict[path1_term], var_dict[path2_term])
            eventually_halt[tau_name] = both_term

def build_obs_always_match(tau_name, tau_all_obs_pair):
    # global observable_vars_formulas
    # global all_formula_pairs
    gates_dict = {}
    for i in range(len_longest_trajectory+1):
        # print(tau_valid_traj_options[str(i)])
        # print(tau2_valid_traj_options[str(i)])
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        # curr_time_step_tau2 = tau_valid_traj_options[str(i)]
        for A in curr_time_step_tau:
            # for B in curr_time_step_tau2:
            traj_name = tau_name+get_phi(A[0], A[1], i)
            NEW_QCIR.append("# key:"+traj_name)
            key_index = var_dict[traj_name]
            # NEW_QCIR.append("# value")
            # print(A, AND, B)
            all_observable_pairs = []
            for k in range(num_observables):
                a = obs_vars_path1[k]+"_A"+timestamp(A[0])
                b = obs_vars_path2[k]+"_B"+timestamp(A[1])
                if ((a+IFF+b) in gates_dict):
                    # print(traj_name, " Implies ", a, "IFF", b)
                    num=gates_dict[a+IFF+b]
                else:
                    num = build_IFF(var_dict[a], var_dict[b])
                    gates_dict[a+IFF+b]=num
                all_observable_pairs.append(num)
            RS = build_AND_multi(all_observable_pairs)
            var_pair_formula = build_IMPLIES(key_index, RS)
            tau_all_obs_pair.append(var_pair_formula)

def build_low_always_match(tau_name, tau_all_low_pair):
    # global observable_vars_formulas
    # global all_formula_pairs
    gates_dict = {}
    for i in range(len_longest_trajectory+1):
        # print(tau_valid_traj_options[str(i)])
        # print(tau2_valid_traj_options[str(i)])
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        # curr_time_step_tau2 = tau_valid_traj_options[str(i)]
        for A in curr_time_step_tau:
            # for B in curr_time_step_tau2:
            traj_name = tau_name+get_phi(A[0], A[1], i)
            NEW_QCIR.append("# key:"+traj_name)
            key_index = var_dict[traj_name]
            # NEW_QCIR.append("# value")
            # print(A, AND, B)
            all_low_pairs = []
            for k in range(num_low_vars):
                a = low_vars_path1[k]+"_A"+timestamp(A[0])
                b = low_vars_path2[k]+"_B"+timestamp(A[1])
                if ((a+IFF+b) in gates_dict):
                    # print(traj_name, " Implies ", a, "IFF", b)
                    num=gates_dict[a+IFF+b]
                else:
                    num = build_IFF(var_dict[a], var_dict[b])
                    gates_dict[a+IFF+b]=num
                all_low_pairs.append(num)
            RS = build_AND_multi(all_low_pairs)
            var_pair_formula = build_IMPLIES(key_index, RS)
            tau_all_low_pair.append(var_pair_formula)




def print_elapsed(str):
    global start
    elapsed = time.time()
    print("time elapsed: ", round((elapsed - start), 3), 's', str)

#### MAIN FUNCTION CALL ###
tau_name = "tau_"

###################### SETUP VARIABLES ######################



###################### BUILD TRANSITIONS ######################
## funtion call: recursively build valid transitions
# print("building trajectory options...")
tau_valid_traj_options={} ## all the 'reachable' trajectory
tau_traj_relations_set=set()
build_traj_advances(0, 0, 0, tau_path1_diameter, tau_path2_diameter, tau_valid_traj_options, tau_traj_relations_set)
print("total number of options:", len(tau_traj_relations_set)*2)
print_elapsed('(building trajectory options...)')

## convert set of tuples back to list of lists.
tau_traj_relations=[]
for r in tau_traj_relations_set:
    tau_traj_relations.append(list(r))
tau_traj_relations.sort()

print(*tau_traj_relations,sep="\n")


## funtion call: build all variables needed
traj_vars_tau=[]
traj_vars_tau2=[]
new_vars = []
var_index = 0
variable_setup(tau_name, tau_valid_traj_options)
print_elapsed('(variable setup...)')

print(*new_vars, sep="\n")






###################### BUILD EXPRESSIONS ######################
tau_exp=[]
tau_dict_choices_eachlayer={}
tau_formula_cond = []
build_expressions(tau_name, tau_exp, tau_traj_relations)
print_elapsed('(building tau expressions...)')

print(*tau_exp, sep="\n")


###################### BUILD QCIR ######################
convert_to_qcir(tau_name, tau_exp, "_A", "_B", tau_formula_cond, tau_dict_choices_eachlayer)
tau_all_formulas = build_AND_multi(tau_formula_cond)
print_elapsed('(building tau QCIR...)')
# elapsed = time.time()
# print("time elapsed: ", round((elapsed - start), 3), 's (building tau QCIR...)')




eventually_tau_halt={}
tau_halting_constraints=[]
build_terminating_traj(tau_name, eventually_tau_halt,"_A")

## the initial case
# tau_exclusive.append(str(var_dict["tau_phi00[0]"]))
# tau_exclusive.append(str(var_dict[tau_name+get_phi(0, 0, 0)]))
def build_exclusive(tau_dict_choices_eachlayer, tau_exclusive, eventually_halt, halting_constraints):
    for key, value in tau_dict_choices_eachlayer.items():
        index = build_exclusive_OR(value)
        tau_exclusive.append(index)
        # one_of_them_is_true=[]
        one_of_them_terminated=[]
        # print(key)
        # print(value)
        for v in value:
            # print(v, eventually_formulas[v])
            # index = build_AND2(str(var_dict[v]), eventually_formulas[v])
            # one_of_them_is_true.append(build_AND2(str(var_dict[v]), eventually_formulas[v]))
            # index = build_AND2(str(var_dict[v]), eventually_halt[v])
            one_of_them_terminated.append(build_AND2(str(var_dict[v]), eventually_halt[v]))

        # index=build_OR_multi(one_of_them_is_true)
        # eventually_formulas_constraints.append(build_OR_multi(one_of_them_is_true))
        halting_constraints.append(build_OR_multi(one_of_them_terminated))


###################### EXCLUSIVE CONSTRAINT ######################
tau_exclusive = []
build_exclusive(tau_dict_choices_eachlayer, tau_exclusive, eventually_tau_halt, tau_halting_constraints)
tau_exclusive_constraints = build_AND_multi(tau_exclusive)
print_elapsed('(building tau exclusive constraints...)')
# elapsed = time.time()
# print("time elapsed: ", round((elapsed - start), 3), 's (building tau exclusive constraints...)')



###################### TERMINATING CONSTRAINTS ######################
tau_eventually_terminated= build_OR_multi(tau_halting_constraints)
print_elapsed('(building tau terminating constraints.)')
# elapsed = time.time()
# print("time elapsed: ", round((elapsed - start), 3), 's (building tau terminating constraints.)')



###################### MOVING CONSTRAINTS ######################
tau_moving = []
for traj in traj_vars_tau:
    index = build_OR2(var_dict[traj[0]], var_dict[traj[1]])
    tau_moving.append(index)
tau_always_move = build_AND_multi(tau_moving)
print_elapsed('(building tau moving constraints...)')
# elapsed = time.time()
# print("time elapsed: ", round((elapsed - start), 3), 's (building tau moving constraints...)')/s




###################### BUILD FORMULAS for PROPERTY ######################
tau_obs_formula_pairs = []
build_obs_always_match(tau_name, tau_obs_formula_pairs)
tau_always_obs_match = build_AND_multi(tau_obs_formula_pairs)

# tau_low_formula_pairs = []
# build_low_always_match(tau_name, tau_low_formula_pairs)
# tau_always_low_match = build_AND_multi(tau_low_formula_pairs)
# print("time elapsed: ", round((elapsed - start), 3), 's (building property formuslas...)')
print_elapsed('(building property formuslas...)')



###################### START MODIFYING QCIR SCRIPT ######################
# print("-------------(TO ADD)------------")
open_QCIR = open("HQ.qcir", "r")
QCIR = open_QCIR.read()

## fetching old logics
OLD_header = re.findall('#QCIR.*', QCIR)
# OLD_forall = re.findall('forall.*', QCIR)
OLD_exists = re.findall('exists.*', QCIR)
OLD_forall = re.findall('forall.*', QCIR)
# OLD_forall[0].replace("exists", "forall")
OLD_logics = re.findall('.*=.*', QCIR)
OLD_vars = re.findall('#\s.*', QCIR)
OLD_output = re.findall('output.*', QCIR)

write_QCIR = open("HQ_async.qcir", "w")

### construct header
# write_QCIR.write(OLD_header[0]+ '\n')

# if(len(OLD_exists)!=0):
    # write_QCIR.write(OLD_exists[0]+ '\n')
# if(len(OLD_forall)!=0):
    # write_QCIR.write(OLD_forall[0]+ '\n')


### setup new added variables
vars_tau=[]
for tau_v in traj_vars_tau:
    vars_tau.extend(tau_v)
for v in To_find:
    if(tau_name in v):
        vars_tau.append(v)


# print(vars_tau)

# print(To_find)
# for v in To_find:
#     quant_exists.append(v)


vars_A=[]
vars_B=[]
for key, value in var_dict.items():
    if("_A" in key):
        # print(key)
        vars_A.append(var_dict[key])
    if("_B" in key):
        # print(key)
        vars_B.append(var_dict[key])
# print(vars_B)



# print(OLD_logics[-1])
# print(OLD_logics[-2])

### fetch the model outputs
M1 = OLD_logics[-1]
M1 = M1.split("(")
M1 = (M1[1].split(","))[0]


M2 = OLD_logics[-2]
M2 = M2.split("(")
M2 = M2[1].split(",")
old_phi = M2[1].replace(")","")
M2 = M2[0]

print("gate M1:", M1)
print("gate M2:", M2)
print("gate old varphi:", old_phi)




# M1="200"
# M2="6221"


###########################################  FORMULA ###########################################
# valid = build_IMPLIES(tau_moving_constraints, tau_eventually_terminated)

# good_traj = build_AND2(tau_all_formulas, tau_exclusive_constraints)

# valid = build_AND4(tau_moving_constraints, tau_eventually_terminated, tau_exclusive_constraints, tau_all_formulas)
# valid = build_AND3(tau_moving_constraints, tau_exclusive_constraints, tau_all_formulas)
# valid = build_AND2(tau_moving_constraints, tau_eventually_terminated, tau_exclusive_constraints, tau_all_formulas)
# valid = build_AND2(tau_eventually_terminated, tau_exclusive_constraints)
# valid = build_AND3(tau_moving_constraints, tau_exclusive_constraints, tau_all_formulas)

# output = OLD_output[0].replace("output(", "")
# output = output.replace(")", "")
# print(var_dict)

INITIAL_CONDITION_tau = str(var_dict[tau_name+get_phi(0, 0, 0)]) ##
TERM_tau = tau_eventually_terminated

VALID_tau = build_AND4(INITIAL_CONDITION_tau, TERM_tau, tau_all_formulas, tau_exclusive_constraints)


# Q_tau="exists"
# Q_tau2="forall"
# FORMULA = str(global_always_formula)
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, build_AND2(VALID_tau, build_IMPLIES(VALID_TAU2,NOT+FORMULA))))

# FORMULA_tau = str(tau_always_formula)
# FORMULA_TAU2 = str(tau2_always_formula)
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, build_AND2(VALID_tau, build_AND2(VALID_TAU2, build_AND2(old_phi,build_AND2(FORMULA_tau, NOT+FORMULA_TAU2))))))

# PROPERTY=build_AND_multi([NOT+str(tau_always_low_match), NOT+str(tau2_always_low_match)])
# PROPERTY=build_AND2(str(tau_always_obs_match), NOT+str(tau2_always_obs_match))

### observational determinism
PROPERTY = (tau_always_obs_match)

##EE
# Q_tau="exists"
# Q_tau2="exists"
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, build_AND2(VALID_tau, build_AND2(VALID_TAU2, PROPERTY))))


## AE
Q_pi1="forall"
Q_pi2="forall"
Q_tau="exists"
FINAL_FORMULA = build_IMPLIES(M1, build_IMPLIES(M2, build_AND2(VALID_tau, PROPERTY)))

## AE, negation?
# Q_pi1="exists"
# Q_pi2="exists"
# Q_tau="exists"
# Q_tau2="forall"
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, build_AND2(VALID_tau, build_IMPLIES(VALID_TAU2, NOT+str(PROPERTY)))))

# INITIAL_CONDITION: inital phi00[0] is always true
# tau_all_formulas: all formulas should be true (phi00[0] -> (....))
# tau_exclusive_constraints: each layer can only have one formula as true, exclusive
# tau_moving_constraints: exclude all 00 movements for t1 and t2

# exists = build_AND3(tau_all_formulas, tau_exclusive_constraints, tau_moving_constraints)
# exists = build_AND2(tau_exclusive_constraints, tau_moving_constraints)
# exists = tau_moving_constraints
## GOOD GOOD
# Q_tau="exists"
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, build_AND3(VALID, TERM, FORMULA)))

# ### CHECKING
# Q_tau="exists"
# Q_tau2="exists"
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, build_AND2(VALID_tau, build_AND2(VALID_TAU2,FORMULA))))
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, VALID_tau))
# FINAL_FORMULA = build_AND2(M1, build_AND2(M2, tau2_all_formulas))




### ADD Headers for path variables
header_M1=Q_pi1+"("
for a in vars_A:
    header_M1 = header_M1 + a + ","
header_M1 = header_M1[:-1] ## remove last ','
header_M1 += ")"
write_QCIR.write(header_M1 + '\n')

header_M2=Q_pi2+"("
for b in vars_B:
    header_M2 = header_M2 + b + ","
header_M2 = header_M2[:-1] ## remove last ','
header_M2 += ")"
write_QCIR.write(header_M2 + '\n')

#### ADD Headers for trajectory variables
tau = Q_tau+"("
for v in vars_tau:
    # print(v)
    tau += str(var_dict[v]) + ","
tau = tau[:-1] ## remove last ','
tau += ")"
write_QCIR.write(tau+ '\n')





#### create a new output gate
NEW_out = str(var_index) # last index used in the above FINAL_FORMULAconstruction
NEW_output_gate = "output("+str(var_index)+")"
write_QCIR.write(NEW_output_gate + '\n')


### try
# OLD_logics.pop()
# OLD_logics.pop()

### merge and write both logics to QCIR
for l in OLD_logics:
    write_QCIR.write(l + '\n')
for n in NEW_QCIR:
    write_QCIR.write(n + '\n')

### DEBUG cyclic!!
# for n in range(71):
#     write_QCIR.write(NEW_QCIR[n] + '\n')


#### DEBUG
# write_QCIR.write(OLD_forall[0].replace("forall", "exists")+ '\n')
# print("to find")
# print(quant_forall)
# print(quant_exists)
# DEBUG = "exists("
# for v in quant_forall:
#     DEBUG += str(var_dict[v]) + ","
# for v in quant_exists:
#     DEBUG += str(var_dict[v]) + ","
# DEBUG = DEBUG[:-1] ## remove last ','
# DEBUG += ")"
# write_QCIR.write(DEBUG+ '\n')


# print(*new_vars, sep="\n")
### plave the last formula
outputs = str(var_index) + " = and("
# outputs += str(output) +  ","
outputs += str(FINAL_FORMULA) +  ","
outputs = outputs[:-1]
outputs += ")"
write_QCIR.write(outputs + '\n')


### merge both var name and numbers for mapping
for v in OLD_vars:
    write_QCIR.write(v + '\n')
for v in new_vars:
    if ( ("t1" in v) | ("t2" in v) | (v in To_find)):
        write_QCIR.write("# "+ str(var_dict[v]) + " : " + v+ '\n')
    # print("# "+ str(var_dict[v]) + " : " + v)
    # debug_QCIR.write("# "+ str(var_dict[v]) + " : " + v+ '\n')




# final_time = round((elapsed - start), 3)
print_elapsed('FINISHED.')
print('Success! Trajectory built.')












#
# for new in new_vars:
#     print(new,  ": ", var_dict[new])
#
# debug_QCIR = open("debug.qcir", "w")
# # print(*NEW_QCIR, sep="\n")
# print("total num of new gates: ", len(NEW_QCIR))
# for n in NEW_QCIR:
#     debug_QCIR.write(n+ '\n')




#====================================================================================
#====================================================================================
#====================================================================================
#====================================================================================
#====================================================================================


### TO BE DECIDED whether convert_to_qcir need this....
## Only with existential quantifier needed this!!
# t1_halt = var_dict[(halt_t1+t1_ext+timestamp(time_t1))]
# t2_halt = var_dict[(halt_t2+t2_ext+timestamp(time_t2))]
# # ## if t1 halts
# only_t1_terminated = build_AND2(t1_halt, NOT+str(t2_halt))
# only_t1_has_halted = build_IMPLIES(only_t1_terminated, NOT+str(choice_3))
# selection.append(only_t1_has_halted)
# ## if t2 halts
# only_t2_terminated = build_AND2(NOT+str(t1_halt), t2_halt)
# only_t2_has_halted = build_IMPLIES(only_t2_terminated, NOT+str(choice_2))
# selection.append(only_t2_has_halted)
# ## if both halt
# both_terminated = build_AND2(t1_halt, t2_halt)
# both_halted = build_IMPLIES(both_terminated, choice_4)
# selection.append(both_halted)


### finally, build everything w.r.t. the quantifiers
# forall = build_AND2(tau_all_formulas, tau_exclusive_constraints)
# M1 = build_AND3(tau_moving_constraints, tau_all_formulas, tau_exclusive_constraints)


# exists = (tau2_all_formulas)
# exists = build_AND2(tau2_all_formulas, tau2_exclusive_constraints)
# M2 = build_AND3(tau2_moving_constraints, tau2_all_formulas, tau2_exclusive_constraints)

# FINAL_FORMULA =  build_AND3(output, forall, exists)
# FINAL_FORMULA =  build_AND3(forall, exists, global_observable_formulas)
# FORMULA = global_observable_formulas

# TRUE = NOT+str(var_dict["path1-halt_A[0]"]) ### a dummy true proposition
### global formula speculative execution v1, EEEA
### FINAL_FORMULA = build_AND3(output, forall, build_IMPLIES(exists, NOT+str(global_observable_formulas)))
# FINAL_FORMULA = output
# FINAL_FORMULA = build_AND2(output, build_AND2(forall, build_IMPLIES(exists, build_AND2(global_policies_formulas, NOT+str(global_observable_formulas)))))
# FINAL_FORMULA = build_AND2(output, build_IMPLIES(forall, build_AND2(exists, global_observable_formulas)))
# FINAL_FORMULA = build_IMPLIES(output, build_IMPLIES(forall, build_IMPLIES(global_policies_formulas, build_AND2(exists, global_observable_formulas))))

# FINAL_FORMULA = build_AND4(output, forall, exists, global_observable_formulas)
# FINAL_FORMULA = build_AND4(output, forall, exists, NOT+str(global_observable_formulas))
# FINAL_FORMULA = build_AND2(output, forall)
# FINAL_FORMULA = output


### global formula speculative execution v2, EEEA
# FINAL_FORMULA = build_AND3(output, forall, build_IMPLIES(exists, NOT+str(global_observable_formulas)))
# FINAL_FORMULA = build_AND3(output, forall, build_IMPLIES(exists, NOT+str(global_observable_formulas)))
### FINAL_FORMULA = build_AND4(output, forall, exists, NOT+str(global_observable_formulas))






### global formula speculative execution v3, EEEA
# FINAL_FORMULA = build_AND3(output, M1, build_AND2(M2, NOT+str(global_observable_formulas)))


# FINAL_FORMULA =  build_AND2(output, build_AND2(forall, exists))
# FINAL_FORMULA =  build_IMPLIES( build_IMPLIES(output, forall), build_AND2(exists, global_observable_formulas) )
# FINAL_FORMULA =  build_IMPLIES( build_AND2(output, forall), build_AND2(exists, global_observable_formulas))
# FINAL_FORMULA =  build_IMPLIES(output, TRUE)
# FINAL_FORMULA =  build_IMPLIES( build_AND2(output, forall), TRUE )


# FINAL_FORMULA = build_AND3(output, forall, build_IMPLIES(exists, str(global_observable_formulas)))


# antecedent = build_AND2(forall, output)
# consequence = build_AND2(exists, global_observable_formulas)
# consequence = build_IMPLIES(exists, global_observable_formulas)
# consequence = build_IMPLIES(exists, NOT+str(global_observable_formulas))
# consequence = build_AND2(exists, NOT+str(global_observable_formulas))

# FINAL_FORMULA = build_AND2(antecedent, consequence)

# MODELS =  build_AND2(output, forall)
# MODELS =  NOT+str(output)


# FINAL_FORMULA = build_AND2(forall, exists)
# FINAL_FORMULA = build_AND3(forall, exists, NOT+str(global_observable_formulas))
# FINAL_FORMULA = build_IMPLIES(forall, consequence)
# FINAL_FORMULA = build_AND2(forall, consequence)
# TRAJS = build_IMPLIES(forall, exists)
# TRAJS = build_AND2(forall, exists)
# FINAL_FORMULA = build_AND2(output, TRAJS)
# FINAL_FORMULA = build_AND3(output, TRAJS, NOT+str(FORMULA))
# FINAL_FORMULA = build_IMPLIES(antecedent, consequence)
# FINAL_FORMULA = build_IMPLIES(MODELS, exists)
# FINAL_FORMULA = build_IMPLIES(MODELS, TRAJS)

# FINAL_FORMULA = output


# FINAL_FORMULA = build_AND2(MODELS, TRAJS)
# FINAL_FORMULA = build_IMPLIES(forall, consequence)
# FINAL_FORMULA = build_IMPLIES(forall, global_observable_formulas)
# FINAL_FORMULA = build_AND2(forall, exists)

# FINAL_FORMULA = build_AND3(forall, exists, str(global_observable_formulas))
# FINAL_FORMULA = build_AND3(forall, exists, NOT+str(global_observable_formulas))



# FINAL_FORMULA = build_IMPLIES(antecedent, consequence)
# FINAL_FORMULA = antecedent
# FINAL_FORMULA = consequence
# print(str(global_observable_formulas))
# FINAL_FORMULA = build_AND4(global_all_formulas, global_moving_constraint, global_exclusive_constraints, global_observable_formulas)
# FINAL_FORMULA = build_AND3(global_all_formulas, global_exclusive_constraints, global_observable_formulas)


# exclusive_constraints = []
# exclusive_constraints.append(var_dict[tau_groups[0]])
# exclusive_constraints.append(var_dict[tau2_groups[0]])
# for tau in tau_groups:
#     if (isinstance(tau, list)):
#         index = build_exclusive_OR(tau)
#         exclusive_constraints.append(index)
# for tau2 in tau2_groups:
#     if (isinstance(tau2, list)):
#         index = build_exclusive_OR(tau2)
#         exclusive_constraints.append(index)


###  build observable matching formulas:
# for A in (valid_traj_options_tau):
#     for B in (valid_traj_options_tau2):
#         ## if same layer
#         if (A[0] == B[0]):
#             # print(A, AND, B)
#             tau_name = "tau_phi"+str(A[1])+str(A[2])+"["+str(A[0])+"]"
#             tau2_name = "tau2_phi"+str(B[1])+str(B[2])+"["+str(B[0])+"]"
#             key = tau_name + AND + tau2_name
#
#             relational_conditions = []
#             for k in range(num_observables):
#                 s_path1 = temp_obs_vars_path1[k]+M1+timestamp(A[1]) + IFF + temp_obs_vars_path1[k]+M2+timestamp(B[1])
#                 s_path2 = temp_obs_vars_path2[k]+M1+timestamp(A[2]) + IFF + temp_obs_vars_path2[k]+M2+timestamp(B[2])
#                 relational_conditions.append(s_path1)
#                 relational_conditions.append(s_path2)
#
#             observable_vars_formulas[key] = relational_conditions

        ## build constraint:
        # print(relational_conditions)
# print("finished.")
### ---DEBUG---
# print(valid_traj_options_tau)
# print(valid_traj_options_tau2)
# print(*observable_vars_formulas['tau_phi01[1] AND tau2_phi01[1]'], sep='\n')
# print(len(traj_relations))
# traj_relations.sort()
# print(*traj_relations,sep="\n")

#### matching the observables ####
# for key, value in observable_vars_formulas.items():
    # print("===", key, "===")
    # print(value)
# gates_dict = {}
# all_formula_pairs = []
# for key, value in observable_vars_formulas.items():
#     # NEW_QCIR.append("# key:"+key)
#     # NEW_QCIR.append("# value")
#     # DEBUG
#     # for v in value:
#         # NEW_QCIR.append("# "+v)
#
#     keys = key.split(' AND ')
#     # print(keys)
#     # print(value)
#     key_index = build_AND2(var_dict[keys[0]], var_dict[keys[1]])
#     # print("key", key_index)
#
#     fff=observable_vars_formulas[key]
#     all_observable_pairs = []
#     for i in range (num_procs*num_observables):
#         c = (fff[i].split(' IFF '))
#         a = var_dict[c[0]]
#         b = var_dict[c[1]]
#         if ((a+IFF+b) in gates_dict):
#             num=gates_dict[a+IFF+b]
#         else:
#             num = build_IFF(a, b)
#             gates_dict[a+IFF+b]=num
#         # print(num)
#         all_observable_pairs.append(num)
#     # print(all_observable_pairs)
#     RS = build_AND_multi(all_observable_pairs)
#     # print("values", RS)
#
#     var_pair_formula = build_IMPLIES((key_index), (RS))
#     # var_pair_formula = build_IFF(str(key_index), str(RS))
#     all_formula_pairs.append(var_pair_formula)

# print(all_formula_pairs)
# final_observable_formulas = build_AND_multi(all_formula_pairs)



# outputs += NOT+str(final_observable_formulas) + ","
# outputs += str(var_dict["tau_t1[0]"]) + ","
# outputs += str(var_dict["tau_t2[0]"]) + ","
# outputs += str(var_dict["tau_phi00[0]"])  +  ","
# outputs += str(var_dict["tau_phi01[1]"])  +  ","
# outputs += str(var_dict["tau_phi22[3]"])  +  ","
# outputs += NOT+str(var_dict["tau_phi03[3]"])  +  ","
# outputs += NOT+str(var_dict["tau_phi04[4]"])  +  ","
#
# outputs += NOT+str(var_dict["tau2_phi02[2]"])  +  ","
# outputs += NOT+str(var_dict["tau2_phi03[3]"])  +  ","
# outputs += NOT+str(var_dict["tau2_phi04[4]"])  +  ","
# outputs += "1193"  +  ","
# outputs += NOT+str(var_dict["tau_t1[2]"]) + ","
# outputs += str(var_dict["tau_t2[2]"]) + ","
# outputs += str(var_dict["tau_phi23[3]"])  +  ","
#
# outputs += NOT+str(var_dict["tau_t1[1]"]) + ","
# outputs += str(var_dict["tau_t2[1]"]) + ","
#
# outputs += str(var_dict["tau_t1[2]"]) + ","
# outputs += NOT+str(var_dict["tau_t2[2]"]) + ","
# outputs += str(var_dict["tau2_t1[0]"]) + ","
# outputs += str(var_dict["tau2_t2[0]"]) + ","
#
# outputs += str(var_dict["tau_t1[1]"]) + ","
# outputs += str(var_dict["tau_t2[1]"]) + ","
# outputs += str(var_dict["tau2_t1[1]"]) + ","
# outputs += str(var_dict["tau2_t2[1]"]) + ","
#
# outputs += str(var_dict["tau_t1[2]"]) + ","
# outputs += str(var_dict["tau_t2[2]"]) + ","
# outputs += str(var_dict["tau2_t1[2]"]) + ","
# outputs += str(var_dict["tau2_t2[2]"]) + ","
# outputs += NOT+str(1252) + ","





# def calc_exclusive_constraints(tau, expressions, tau_groups):
#     # To_find=[]
#     find= []
#     for e in expressions:
#         exp=e
#         name = tau+exp[0]
#         To_find.append(name)
#         find.append(name)
#     counter=0
#     g=[]
#     for v in find:
#         # if ("tau2_phi00[0]" in v):
#         #     g=[]
#         #     counter=0
#         if (counter == 0):
#             tau_groups.append(v)
#             counter+=1
#         else:
#             if (int(get_timestamp(v))==counter):
#                 g.append(v)
#             else:
#                 # print(counter)
#                 # print(g)
#                 tau_groups.append(g)
#                 g=[]
#                 g.append(v)
#                 counter+=1



# # print(dict_choices_eachlayer)
# phi_formulas = []
#
# counter=0
# groups=[]
# g=[]
# for v in To_find:
#     if ("tau2_phi00[0]" in v):
#         g=[]
#         counter=0
#
#     if (counter == 0):
#         groups.append(v)
#         counter+=1
#     else:
#         if (int(get_timestamp(v))==counter):
#             g.append(v)
#         else:
#             print(counter)
#             print(g)
#             groups.append(g)
#             g=[]
#             g.append(v)
#             counter+=1
#
#
# def build_exclusive_OR(lst):
#     picks=[]
#     for v_picked in lst:
#         pick_v=[]
#         for v in lst:
#             if (v==v_picked):
#                 # pick_v.append(str(v))
#                 pick_v.append(str(var_dict[v]))
#             else:
#                 # pick_v.append(NOT+str(v))
#                 pick_v.append(NOT+str(var_dict[v]))
#         exclusive_pick_v = build_AND_multi(pick_v)
#         picks.append(exclusive_pick_v)
#
#     index = build_OR_multi(picks)
#     return index
#
#
# NEW_QCIR.append("# check")
# for g in groups:
#     if ("phi00" in g):
#         # phi_formulas.append(g)
#         phi_formulas.append(str(var_dict[g]))
#     # NEW_QCIR.append(g)
#     else:
#         # print(g)
#         choose_one = build_exclusive_OR(g)
#         phi_formulas.append(choose_one)
#
#
# choose_one = build_exclusive_OR(groups[1])
# phi_formulas.append(choose_one)
# print(groups)

# phi_formulas_constraints = build_AND_multi(phi_formulas)



#
# for key, value in dict_choices_eachlayer.items():
#     # picks = []
#     # for v in dict_choices_eachlayer[key]:
#     #     pick_v=[]
#     #     values = dict_choices_eachlayer[key]
#     #     for each in values:
#     #         if (each == v):
#     #             pick_v.append(str(each))
#     #         else:
#     #             pick_v.append(NOT+str(each))
#     #
#     #     # print(pick_v)
#     #     exclusive_pick_v = build_AND_multi(pick_v)
#     #     picks.append(exclusive_pick_v)
#
#     # print(picks)
#
#     # choose_one = build_OR_multi(value)
#     choose_one = build_exclusive_OR(value)
#
#     # choose_one = bsuild_OR_multi(picks)
#     phi_formulas.append(choose_one)
#
#
# # print("??")
# print(phi_formulas)
# phi_formulas_constraints = build_AND_multi(phi_formulas)

# print("??", global_formula_gate)
# global_formula = build_AND_multi(global_formula_gate)












    # for

    # print("(", time,")", "phi", formula_t1,formula_t2, ":", AND, movement, "implies (", next_time, next_formula_t1, next_formula_t2, ")" )
    # print("phi", formula_t1,formula_t2, time, "IFF", "phi", formula_t1,formula_t2, AND, "(", t1, t2, AND, "phi", next_formula_t1, next_formula_t2, ")" )

# print(traj_vars_tau)
# counter = 0
# print(tau_formulas)

### build final formulas
# for key, value in tau_formulas.items():
#     print(key)
#     # print(value)
#     ### concatenate four different transitions:
#     traj_advance = []
#     bits_vars = traj_vars_tau[counter]
#     traj_advance.append(" 00 --->"+ NOT+bits_vars[0]+NOT+bits_vars[1])
#     traj_advance.append(" 01 --->"+ NOT+bits_vars[0]+    bits_vars[1])
#     traj_advance.append(" 10 --->"+     bits_vars[0]+NOT+bits_vars[1])
#     traj_advance.append(" 11 --->"+ NOT+bits_vars[0]+NOT+bits_vars[1])
#     next_steps = ""
#     for traj in traj_advance:
#         next_steps = next_steps+traj
#
#
#     temp_value = value.append(next_steps)
#     tau_formulas[key] = temp_value
#     # print(*value, sep = "\n")
#     print(*traj_advance, sep = "\n")


# traj_advance = []
# for t in range(len_longest_trajectory):
#     bits_vars = traj_vars_tau[counter]
#     traj_advance.append("from"+str(t)+ ": 00 --->"+ NOT+bits_vars[0]+NOT+bits_vars[1]+AND+"TO:"+str(t+1))
#     traj_advance.append("from"+str(t)+ ": 01 --->"+ NOT+bits_vars[0]+    bits_vars[1]+AND+"TO:"+str(t+1))
#     traj_advance.append("from"+str(t)+ ": 10 ---> "+     bits_vars[0]+NOT+bits_vars[1]+AND+"TO:"+str(t+1))
#     traj_advance.append("from"+str(t)+ ": 11 --->"+ NOT+bits_vars[0]+NOT+bits_vars[1]+AND+"TO:"+str(t+1))
#     traj_advance.append("\n")







# for i in sorted (traj_relations.keys()) :
#      print(i, end = " ")

# print(traj_relations)
# ### build tau_1 trajectories
# for c in range(len_longest_trajectory):
#     for i in range(diameter_path1):
#         for j in range(diameter_path2):
#             print(c, i, j)







# new_vars["var"] = "123"
# print(new_vars)

































# open_QUABS = open("HQ.quabs", "r")
# QUABS = open_QUABS.read()
# # print(open_QUABS.read())
# print(QUABS)
# # print(QUABS.split())
# var_QUABS=QUABS.split()
# for q in var_QUABS:
#     if (q[0] == "V"):
#         print("")
#     elif (q[0]=="-"):
#         print(q)
#         name = var_dict[q.replace("-","")]
#         print(name, "= 0")
#     else:
#         name= var_dict[q]
#         print(name, "= 1")
