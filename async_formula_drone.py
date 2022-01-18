import re
import sys
import time

### logical operators
IFF = " IFF "
AND = " AND "
OR = " OR "
NOT = " -"

M1 = "_A"
M2 = "_B"

### parameters setup
# temp_obs_vars_proc1 = ["proc1-var_temp_proc1_0", "proc1-var_temp_proc1_1", "proc1-var_temp_proc1_2"]
# temp_obs_vars_proc2 = ["proc2-var_temp_proc2_0", "proc2-var_temp_proc2_1", "proc2-var_temp_proc2_2"]
# temp_obs_vars = ["y_axis_0","y_axis_1","y_axis_2","x_axis_0","x_axis_1","x_axis_2"]
temp_obs_vars = []
# temp_obs_vars = ["low_0"]
termination_flag = "halt"
num_observables = 6


# temp_obs_vars_ONE = ["y_axis_ONE_0","y_axis_ONE_1","y_axis_ONE_2","x_axis_ONE_0","x_axis_ONE_1","x_axis_ONE_2"]
# temp_obs_vars_TWO = ["y_axis_TWO_0","y_axis_TWO_1","y_axis_TWO_2","x_axis_TWO_0","x_axis_TWO_1","x_axis_TWO_2"]

# temp_obs_vars_TWO = ["y_axis_TWO_0","y_axis_TWO_1","y_axis_TWO_2","x_axis_TWO_0","x_axis_TWO_1","x_axis_TWO_2"]

# obs_vars_M1_ONE =["y_axis_ONE_0","y_axis_ONE_1","y_axis_ONE_2","x_axis_ONE_0","x_axis_ONE_1","x_axis_ONE_2"]
# obs_vars_M1_TWO =["y_axis_TWO_0","y_axis_TWO_1","y_axis_TWO_2","x_axis_TWO_0","x_axis_TWO_1","x_axis_TWO_2"]
# obs_vars_M2 = ["y_axis_0","y_axis_1","y_axis_2","x_axis_0","x_axis_1","x_axis_2"]

obs_vars_M1_ONE =["y_axis_neighbor_0","y_axis_neighbor_1","y_axis_neighbor_2","x_axis_neighbor_0","x_axis_neighbor_1","x_axis_neighbor_2"]
obs_vars_M1_TWO =["y_axis_0","y_axis_1","y_axis_2","x_axis_0","x_axis_1","x_axis_2"]
obs_vars_M2 = ["y_axis_0","y_axis_1","y_axis_2","x_axis_0","x_axis_1","x_axis_2"]

# num_observables = 3
# num_observables = 6
# num_procs = 2

# halt_t1 = "proc1-halt"
# halt_t2 = "proc2-halt"

halt_t1 = "halt"
halt_t2 = "halt"

# print(sys.argv)
# diameter_proc1 = int(sys.argv[1])
# diameter_proc2 = int(sys.argv[2])
# tau1_proc1_diameter =int(sys.argv[1])
# tau1_proc2_diameter =int(sys.argv[2])
# tau2_proc1_diameter =int(sys.argv[3])
# tau2_proc2_diameter =int(sys.argv[4])


tau_proc1_diameter = int(sys.argv[1])
tau_proc2_diameter = int(sys.argv[2])

len_longest_trajectory = tau_proc1_diameter + tau_proc2_diameter
# len_longest_trajectory = int(sys.argv[3])

print("diameter_proc1: ", tau_proc1_diameter)
print("diameter_proc2: ", tau_proc2_diameter)
print("len_longest_trajectory: ", len_longest_trajectory)
start = time.time()

# print("diameter_tau1_proc1: ", tau1_proc1_diameter)
# print("diameter_tau1_proc2: ", tau1_proc2_diameter)
# print("diameter_tau2_proc1: ", tau2_proc1_diameter)
# print("diameter_tau2_proc2: ", tau2_proc2_diameter)

# print("start! ", start)
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



#### var_dict is a dictionary containing all variables with their numerical representation
#### format--> VAR_NAME: VAR_NUMBER
var_dict = {}
### set up all variables needed, including the trajectory variables and formula variables
def variable_setup():
    #### fetch all observatable variables nedded (i.e., with given keyword "print") for tau1 and tau2
    # M1_proc1 = []
    # M1_proc2 = []
    # M2_proc1 = []
    # M2_proc2 = []
    global var_dict
    for var in vars:
        var = (var.split(" "))
        var_dict[var[3]] = var[1];

    tau_proc1 = []
    tau_proc2 = []
    for key, value in var_dict.items():
        if ("print" not in key):
            continue;

        # if ("_A" in key):
        #     if ("proc1" in key):
        #         if (get_timestamp(key) <= tau1_proc1_diameter):
        #             M1_proc1.append(key)
        #     elif ("proc2" in key):
        #         if (get_timestamp(key) <= tau1_proc2_diameter):
        #             M1_proc2.append(key)
        #
        # if ("_B" in key):
        #     if ("proc1" in key):
        #         if (get_timestamp(key) <= tau2_proc1_diameter):
        #             M2_proc1.append(key)
        #     elif ("proc2" in key):
        #         if (get_timestamp(key) <= tau2_proc2_diameter):
        #             M2_proc2.append(key)

        if ("_A" in key):
            if (get_timestamp(key) <= tau_proc1_diameter):
                tau_proc1.append(key)
        if ("_B" in key):
            if (get_timestamp(key) <= tau_proc2_diameter):
                tau_proc2.append(key)
    ### ---DEBUG---
    # print(*M1_proc1,sep="\n")
    # print(*M1_proc2,sep="\n")
    # print(*M2_proc1,sep="\n")
    # print(*M2_proc2,sep="\n")


    ### new_vars is an array for all New Variables
    # including trajectory varariables and the constraints formula
    global new_vars

    ### BUILD TRAJECTORY VARIABLES
    # global traj_vars_tau1
    # global traj_vars_tau2
    global traj_vars_tau
    for i in range(len_longest_trajectory):
        # tau1_bits = []
        # tau1_bits.append("tau1_t1["+str(i)+"]")
        # tau1_bits.append("tau1_t2["+str(i)+"]")
        # traj_vars_tau1.append(tau1_bits)
        # tau2_bits = []
        # tau2_bits.append("tau2_t1["+str(i)+"]")
        # tau2_bits.append("tau2_t2["+str(i)+"]")
        # traj_vars_tau2.append(tau2_bits)
        #
        # ### save it to assign new numbers
        # new_vars.append("tau1_t1["+str(i)+"]")
        # new_vars.append("tau1_t2["+str(i)+"]")
        # new_vars.append("tau2_t1["+str(i)+"]")
        # new_vars.append("tau2_t2["+str(i)+"]")

        tau_bits = []
        tau_bits.append("tau_t1["+str(i)+"]")
        tau_bits.append("tau_t2["+str(i)+"]")
        traj_vars_tau.append(tau_bits)
        new_vars.append("tau_t1["+str(i)+"]")
        new_vars.append("tau_t2["+str(i)+"]")



    ### ---DEBUG---
    # print(var_dict)
    # print(traj_vars_tau1)
    # print(traj_vars_tau2)

    ### NEW build relational constraints formulas for tau
    ### (t1, t2, conditions....)
    # tau1_formulas = {}
    # tau2_formulas = {}
    ### build tau_1 constraints
    # for i in range(tau1_proc1_diameter+1):
    #     for j in range(tau1_proc2_diameter+1):
    #         for k in range(len_longest_trajectory+1):
    #             name = "tau1_phi"+str(i)+str(j)+"["+str(k)+"]"
    #             new_vars.append(name)
    #             # relational_predicate = "phi"+str(i)+str(j)
    #             ### if any realational predicate between program 1 and 2 is needed, use relational_predicate
    #
    #
    # ### build tau_2 constraints
    # for i in range(tau2_proc1_diameter+1):
    #     for j in range(tau2_proc2_diameter+1):
    #         for k in range(len_longest_trajectory+1):
    #             name = "tau2_phi"+str(i)+str(j)+"["+str(k)+"]"
    #             new_vars.append(name)
    #             # relational_predicate = "phi"+str(i)+str(j)
    #             ### if any realational predicate between program 1 and 2 is needed, use relational_predicate
    #

    for i in range(tau_proc1_diameter+1):
        for j in range(tau_proc2_diameter+1):
            for k in range(len_longest_trajectory+1):
                name = "tau_phi"+str(i)+str(j)+"["+str(k)+"]"
                new_vars.append(name)

    ### ---DEBUG---
    # print(*observable_vars_formulas, sep="\n")
    # print(observable_vars_formulas)
    # print(*new_vars, sep="\n")
    # print('==')
    # print(tau1_formulas)
    # print(tau2_formulas)


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


### BUILD TRANSITIONS
# this function recursively find all possible reachable trajectory options with four possible
# moves for two concurrent threads: 00, 01, 10, 11
# -----------------------------------------
# for example, tau1_phi00[0] is the initial state
# tau1_phi10[1] is a reachable trajectory option, with program 1 moves and program 2 doesn't
# however, tau1_phi22[1] is an unreachable trajectory option, since it's impossible for both programs
# to move two steps within only one time unit.
# ------------------------------------------
# The built transitions are presented in the following formats:
# (curr_layer, curr_t1, curr_t2, t1&t2(movements), next_layer, next_t1, next_t2)
def build_traj_advances(layer, t1, t2, diameter_proc1, diameter_proc2, valid_traj_options, traj_relations):
    ### collect valid trajectory options (i.e., valid trajectory)
    # valid_traj_options_tau1.append([layer, t1, t2])
    if (not(str(layer) in valid_traj_options)):
        valid_traj_options[str(layer)] = []
    if ([t1, t2] not in valid_traj_options[str(layer)]):
        valid_traj_options[str(layer)].append([t1, t2])


    # if (not(str(layer) in valid_traj_options_tau1)):
    #     valid_traj_options_tau1[str(layer)] = []
    # if ([t1, t2] not in valid_traj_options_tau1[str(layer)]):
    #     valid_traj_options_tau1[str(layer)].append([t1, t2])
    #
    # if (not(str(layer) in valid_traj_options_tau2)):
    #     valid_traj_options_tau2[str(layer)] = []
    # if ([t1, t2] not in valid_traj_options_tau2[str(layer)]):
    #     valid_traj_options_tau2[str(layer)].append([t1, t2])

    # valid_traj_options_tau2.append([layer, t1, t2])

    next_layer=layer+1 # next layer

    # Last layer
    if (layer==len_longest_trajectory):
        # traj_relations.append([layer, t1, t2])
        traj_relations.add((layer, t1, t2))
        # print(layer, t1, t2)
        return
    else:
        ## calculate next positions for both threads with four possible trajevtories
        t1_not_move = t1
        t2_not_move = t2

        if (t1 < diameter_proc1):
            t1_move = t1 + 1
        else: t1_move = t1
        if (t2 < diameter_proc2):
            t2_move = t2 + 1
        else: t2_move = t2

        ### let's ban this to make the exploration more efficient
        # 00 situation
        # next_1 = [layer, t1, t2, "00", next_layer]
        # next_1 = [layer, t1, t2, "00", next_layer, t1_not_move, t2_not_move]
        # next_1 = (layer, t1, t2, "00", next_layer, t1_not_move, t2_not_move)
        # traj_relations.append(next_1)
        # if (next_1 not in traj_relations):
            # traj_relations.append(next_1)
        traj_relations.add((layer, t1, t2, "00", next_layer, t1_not_move, t2_not_move))

        ## 01 situation
        # next_2 = [layer, t1, t2, "01", next_layer, t1_not_move, t2_move]
        # next_2 = (layer, t1, t2, "01", next_layer, t1_not_move, t2_move)
        # traj_relations.append(next_2)
        traj_relations.add((layer, t1, t2, "01", next_layer, t1_not_move, t2_move))

        ## 10 situation
        # next_3 = [layer, t1, t2, "10", next_layer, t1_move, t2_not_move]
        # next_3 = (layer, t1, t2, "10", next_layer, t1_move, t2_not_move)
        # traj_relations.append(next_3)
        traj_relations.add((layer, t1, t2, "10", next_layer, t1_move, t2_not_move))

        ## 11 situation
        # next_4 = [layer, t1, t2, "11", next_layer, t1_move, t2_move]
        # next_4 = (layer, t1, t2, "11", next_layer, t1_move, t2_move)
        # traj_relations.append(next_4)
        traj_relations.add((layer, t1, t2, "11", next_layer, t1_move, t2_move))

        ## recursivly try all four directions for the next time step
        build_traj_advances(next_layer, t1_not_move, t2_not_move, diameter_proc1, diameter_proc2, valid_traj_options, traj_relations) ## banned 00 for efficiency purpose
        build_traj_advances(next_layer, t1_not_move, t2_move, diameter_proc1, diameter_proc2, valid_traj_options, traj_relations) # 01
        build_traj_advances(next_layer, t1_move, t2_not_move, diameter_proc1, diameter_proc2, valid_traj_options, traj_relations) # 10
        build_traj_advances(next_layer, t1_move, t2_move, diameter_proc1, diameter_proc2, valid_traj_options, traj_relations) # 11

        ### if we don't expand this
        # traj_relations.append([layer, t1, t2, "00"])


### No need now
# res = []
# for i in tau_traj_relations:
#     if i not in res:
#         res.append(i)
# tau_traj_relations = res
# tau_traj_relations.sort()
# print("finished remove trajectory option dups.")
# elapsed = time.time()
# print(round((elapsed - start), 3))
# print(len(tau_traj_relations))
#
# print("trajectory options built, in total: ", str(len(tau1_traj_relations)))
# print("trajectory options built, in total: ", str(len(tau2_traj_relations)))

### ---DEBUG---
# print(final_formulas)
# print(len(traj_relations))
### small debug print
# print("trajectory options built, in total: ", str(len(tau_traj_relations)))
# print(*tau_traj_relations,sep="\n")
### small debug print
# print("trajectory options built, in total: ", str(len(tau2_traj_relations)))
# print(*tau2_traj_relations,sep="\n")



### convert the trajectory relations into given naming
def build_expressions(tau, final_expressions, traj_relations):
    for i in range(0, len(traj_relations), 4):
        r = traj_relations[i]
        if (len(r) == 3):
            for k in range(i, len(traj_relations)):
                r = traj_relations[k]
                # print(r)
                time = r[0]
                formula_t1=r[1]
                formula_t2=r[2]
                pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", "phi"+ str(formula_t1)+str(formula_t2)]
                final_expressions.append(pre)
            ### finishing base case, break
            break
        else:
            time = r[0]
            formula_t1= r[1]
            formula_t2= r[2]
            pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", "phi"+ str(formula_t1)+str(formula_t2)]
            # print(pre)
            next_move = []

            ### the "00 " case
            # next_move.append(i+1)
            # post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
            ## remove
            # post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
            # next_move.append(post)

            for j in range(4):
                # next = traj_relations[i+j+1]
                next = traj_relations[i+j]
                # print(next)
                movement=next[3]
                next_time=next[4]
                next_formula_t1=next[5]
                next_formula_t2=next[6]

                t1 = tau+"t1["+str(time)+"]"
                if (movement[0]=='0'):
                    t1="-"+t1

                t2 = tau+"t2["+str(time)+"]"
                if (movement[1]=='0'):
                    t2="-"+t2


                post = [t1, t2, "phi"+str(next_formula_t1)+str(next_formula_t2)+"["+ str(next_time)+"]"]
                next_move.append(post)

            pre.extend(next_move)
            final_expressions.append(pre)


### check the halting conditions
# halt_M1_t1 = var_dict["proc1-t1_halt_A[0]"]
# halt_M1_t2 = var_dict["proc1-t2_halt_A[0]"]
# halt_M2_t1 = var_dict["proc1-t1_halt_B[0]"]
# halt_M2_t2 = var_dict["proc1-t2_halt_B[0]"]


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
        time_t1 = e[0][3]
        # print(time_t1)

        time_t2 = e[0][4]
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
        NEW_QCIR.append("# exp:")
        NEW_QCIR.append("# "+ str(name))

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

            ## move 01, 10, 11
            for i in range(2,6):
                move = e[i]

                ###  extract t1
                if ("-" in move[0]):
                    temp = move[0].replace("-","")
                    temp = "-"+str(var_dict[temp])
                    a = temp
                else:
                    a = str(var_dict[move[0]])

                ###  extract t2
                if ("-" in move[1]):
                    temp2 = move[1].replace("-","")
                    temp2 = "-"+str(var_dict[temp2])
                    b = temp2
                else:
                    b = str(var_dict[move[1]])


                # if (int(get_timestamp(exp[0]))s != (len_longest_trajectory-2)):
                c = str(var_dict[tau+move[2]])
                c_set.add(tau+move[2])
                # choices.append([a,b,c])
                choices.append([a,b,c])
                # else:s
                    # choices.append([a,b])
                # build_AND3(a, b, c)
            # print(choices)
            # c_set = { choices[1][2], choices[2][2], choices[3][2]}
            c_list = list(c_set)

            choice_1 = build_AND_multi(choices[0]) #00
            choice_2 = build_AND_multi(choices[1]) #01
            choice_3 = build_AND_multi(choices[2]) #10
            choice_4 = build_AND_multi(choices[3]) #11

            selection = []
            choices = [choice_1, choice_2, choice_3, choice_4]
            free_choices = build_OR_multi(choices)
            selection.append(free_choices)


            # ## Only with existential quantifier needed this!!
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



            selections = build_AND_multi(selection)
            formula = build_IMPLIES(formula_antecedent, selections)
            # formula = build_IMPLIES(formula_antecedent, free_choices)
            # index = build_IFF(formula_antecedent, selection) ## this is wrong
            formula_cond.append(formula)



# valid_traj_options_tau1.sort()
# valid_traj_options_tau2.sort()


### ---DEBUG---
# print(final_formulas)
# print("here")
# # print(*valid_traj_options_tau1,sep="\n")
# print(valid_traj_options_tau1)
# print("here2")
# print(*valid_traj_options_tau2,sep="\n")


# for key, value in valid_traj_options_tau1:
#     print(key)


#### BUILD RELATIONAL CONDITIONS for OBSERBVABLE VARIABLES ####
### build relational constraints over all observable variables for tau
# valid_traj_options_tau1.sort()
# valid_traj_options_tau2.sort()
# observable_vars_formulas = {}
eventually_formulas ={}
all_formula_pairs = []
def build_realtional_conditions():
    # global observable_vars_formulas
    global all_formula_pairs
    gates_dict = {}
    for i in range(len_longest_trajectory+1):
        # print(tau1_valid_traj_options[str(i)])
        # print(tau2_valid_traj_options[str(i)])
        # curr_time_step_tau1 = tau1_valid_traj_options[str(i)]
        # curr_time_step_tau2 = tau2_valid_traj_options[str(i)]
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        # print(curr_time_step_tau)
        for curr in curr_time_step_tau:
        # for B in curr_time_step_tau2:
            tau_name = "tau_phi"+str(curr[0])+str(curr[1])+"["+str(i)+"]"
            # tau2_name = "tau2_phi"+str(B[0])+str(B[1])+"["+str(i)+"]"
            NEW_QCIR.append("# key:"+tau_name)
            # print(tau_name)
            key_index = (var_dict[tau_name])
            # print(key_index)
            NEW_QCIR.append("# value")
            # print(A, AND, B)
            # print(curr)
            if (len(temp_obs_vars) != 0):
                all_observable_pairs = []
                for k in range(num_observables):
                    ### relational predicates among one trajectories
                    proc = [temp_obs_vars[k]+M1+timestamp(curr[0]), temp_obs_vars[k]+M2+timestamp(curr[1])]
                    # print(proc)
                    a = proc[0]
                    b = proc[1]
                    if ((a+IFF+b) in gates_dict):
                        num=gates_dict[a+IFF+b]
                    else:
                        num = build_IFF(var_dict[a], var_dict[b])
                        gates_dict[a+IFF+b]=num
                    all_observable_pairs.append(num)

                RS = build_AND_multi(all_observable_pairs)
                # print(tau_name)
                # print(proc)
                eventually_formulas[tau_name] = RS

            else:
                all_observable_pairs_ONE = []
                all_observable_pairs_TWO = []
                for k in range(num_observables):
                    ### relational predicates among one trajectories
                    proc1 = [obs_vars_M1_ONE[k]+M1+timestamp(curr[0]), obs_vars_M2[k]+M2+timestamp(curr[1])]
                    proc2 = [obs_vars_M1_TWO[k]+M1+timestamp(curr[0]), obs_vars_M2[k]+M2+timestamp(curr[1])]


                    a = proc1[0]
                    b = proc1[1]
                    if ((a+IFF+b) in gates_dict):
                        num=gates_dict[a+IFF+b]
                    else:
                        num = build_IFF(var_dict[a], var_dict[b])
                        gates_dict[a+IFF+b]=num
                    all_observable_pairs_ONE.append(num)

                    a = proc2[0]
                    b = proc2[1]
                    if ((a+IFF+b) in gates_dict):
                        num=gates_dict[a+IFF+b]
                    else:
                        num = build_IFF(var_dict[a], var_dict[b])
                        gates_dict[a+IFF+b]=num
                    all_observable_pairs_TWO.append(num)

                # print(all_observable_pairs_ONE)

                RS = build_OR2(build_AND_multi(all_observable_pairs_ONE), build_AND_multi(all_observable_pairs_TWO))
                eventually_formulas[tau_name] = RS





eventually_halt={}
def build_terminating_traj():
    # global observable_vars_formulas
    global eventaully_halt
    # gates_dict = {}
    for i in range(len_longest_trajectory+1):
        curr_time_step_tau = tau_valid_traj_options[str(i)]
        # print(curr_time_step_tau)
        for curr in curr_time_step_tau:
        # for B in curr_time_step_tau2:
            tau_name = "tau_phi"+str(curr[0])+str(curr[1])+"["+str(i)+"]"
            # NEW_QCIR.append("# key:"+tau_name)
            # print(tau_name)
            key_index = (var_dict[tau_name])
            # print(key_index)
            NEW_QCIR.append("# value")

            # both_terminated = [termination_flag+M1+timestamp(curr[0]), termination_flag+M2+timestamp(curr[1])]
            M1_term = termination_flag+M1+timestamp(curr[0])
            M2_term = termination_flag+M2+timestamp(curr[1])

            both_term = build_AND2(var_dict[M1_term], var_dict[M2_term])
            eventually_halt[tau_name] = both_term



# policies = ["var_Y_0", "var_Y_1", "var_Y_2", "var_size_0", "var_size_1", "var_size_2"]
# num_policies = len(policies)
# # policies_formulas = {}
# all_policies_pairs = []
# def build_policies_conditions():
#     # global policies_formulas
#     global all_policies_pairs
#     gates_dict = {}
#     for i in range(len_longest_trajectory+1):
#         # print(tau1_valid_traj_options[str(i)])
#         # print(tau2_valid_traj_options[str(i)])
#         curr_time_step_tau1 = tau1_valid_traj_options[str(i)]
#         curr_time_step_tau2 = tau2_valid_traj_options[str(i)]
#         for A in curr_time_step_tau1:
#             for B in curr_time_step_tau2:
#                 tau1_name = "tau1_phi"+str(A[0])+str(A[1])+"["+str(i)+"]"
#                 tau2_name = "tau2_phi"+str(B[0])+str(B[1])+"["+str(i)+"]"
#                 NEW_QCIR.append("# key:"+tau1_name+tau2_name)
#                 key_index = build_AND2(var_dict[tau1_name], var_dict[tau2_name])
#                 NEW_QCIR.append("# value")
#                 # print(A, AND, B)
#                 all_observable_pairs = []
#                 for k in range(num_policies):
#                     ### relational predicates among multiple trakectories
#                     # proc_1 = [policies[k]+M1+timestamp(A[0]), policies[k]+M2+timestamp(B[0])]
#                     # proc_1 = [policies[k]+M1+timestamp(A[1]), policies[k]+M2+timestamp(B[1])]
#                     # proc_2 = [policies[k]+M1+timestamp(A[1]), policies[k]+M2+timestamp(B[1])]
#                     # print(proc_1)
#
#                     ### relational predicates among one trajectories
#                     proc_1 = [policies[k]+M1+timestamp(A[0]), policies[k]+M2+timestamp(B[0])]
#                     proc_2 = [policies[k]+M1+timestamp(A[1]), policies[k]+M2+timestamp(B[1])]
#                     print(proc_1)
#                     print(proc_2)
#                     #
#                     # NEW_QCIR.append(proc_1[0])
#                     # NEW_QCIR.append(proc_1[1])
#                     # NEW_QCIR.append(proc_2[0])
#                     # NEW_QCIR.append(proc_2[1])
#
#                     a = proc_1[0]
#                     b = proc_1[1]
#                     if ((a+IFF+b) in gates_dict):
#                         num=gates_dict[a+IFF+b]
#                     else:
#                         num = build_IFF(var_dict[a], var_dict[b])
#                         gates_dict[a+IFF+b]=num
#                     all_observable_pairs.append(num)
#
#                     a = proc_2[0]
#                     b = proc_2[1]
#                     if ((a+IFF+b) in gates_dict):
#                         num=gates_dict[a+IFF+b]
#                     else:
#                         num = build_IFF(var_dict[a], var_dict[b])
#                         gates_dict[a+IFF+b]=num
#                     all_observable_pairs.append(num)
#
#                 RS = build_AND_multi(all_observable_pairs)
#                 var_pair_formula = build_IMPLIES(key_index, RS)
#                 all_policies_pairs.append(var_pair_formula)


#### MAIN FUNCTION CALL ###

## funtion call: build all variables needed
traj_vars_tau=[]
new_vars = []
var_index = 0
variable_setup()
print("variable setup...")
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')


## funtion call: recursively build valid transitions
print("building trajectory options...")
tau_valid_traj_options={}
tau_traj_relations_set=set()
build_traj_advances(0, 0, 0, tau_proc1_diameter, tau_proc2_diameter, tau_valid_traj_options, tau_traj_relations_set)
print("total number of options:", len(tau_traj_relations_set))
elapsed = time.time()
print("time elapsed: ",round((elapsed - start), 3), 's')

## conver set of tuples back to list of lists.
tau_traj_relations=[]
for r in tau_traj_relations_set:
    tau_traj_relations.append(list(r))
tau_traj_relations.sort()

print("building tau expressions...")
tau_exp=[]
tau_dict_choices_eachlayer={}
tau_formula_cond = []
build_expressions("tau_", tau_exp, tau_traj_relations)
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')


print("finished building tau QCIR...")
convert_to_qcir("tau_", tau_exp, "_A", "_B", tau_formula_cond, tau_dict_choices_eachlayer)
tau_all_formulas = build_AND_multi(tau_formula_cond)
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')

# tau1_exp=[]
# tau1_dict_choices_eachlayer={}
# tau1_formula_cond = []
# build_expressions("tau1_", tau1_exp, tau1_traj_relations)
# convert_to_qcir("tau1_", tau1_exp, "_A", tau1_formula_cond, tau1_dict_choices_eachlayer)
# tau1_all_formulas = build_AND_multi(tau1_formula_cond)
# print("finished building tau1")
#
# tau2_exp=[]
# tau2_dict_choices_eachlayer={}
# tau2_formula_cond = []
# build_expressions("tau2_", tau2_exp, tau2_traj_relations)
# convert_to_qcir("tau2_", tau2_exp, "_B", tau2_formula_cond, tau2_dict_choices_eachlayer)
# tau2_all_formulas = build_AND_multi(tau2_formula_cond)
# print("finished building tau2")
#
# global_observable_formulas = build_AND_multi(all_formula_pairs)
# print("finished observable formulas")

# print(eventually_formulas)
# build_policies_conditions()
# global_policies_formulas = build_AND_multi(all_policies_pairs)
# print("finished policies formulas")


# for key, value in tau_dict_choices_eachlayer.items():
#     print(key, "  ", value)
build_realtional_conditions()
build_terminating_traj()

eventually_formulas_constraints = []
eventually_halt_constraints = []
tau_exclusive = []

print("building exclusive constraints...")
NEW_QCIR.append("# tau layer constraints:")
## the initial case
tau_exclusive.append(str(var_dict["tau_phi00[0]"]))
index = build_AND2(str(var_dict["tau_phi00[0]"]), eventually_formulas["tau_phi00[0]"])
eventually_formulas_constraints.append(index)





for key, value in tau_dict_choices_eachlayer.items():
    index = build_exclusive_OR(value)
    tau_exclusive.append(index)
    one_of_them_is_true=[]
    one_of_them_terminated=[]
    # print(key)
    # print(value)
    for v in value:
        # print(v, eventually_formulas[v])
        # index = build_AND2(str(var_dict[v]), eventually_formulas[v])
        one_of_them_is_true.append(build_AND2(str(var_dict[v]), eventually_formulas[v]))
        # index = build_AND2(str(var_dict[v]), eventually_halt[v])
        one_of_them_terminated.append(build_AND2(str(var_dict[v]), eventually_halt[v]))

    # index=build_OR_multi(one_of_them_is_true)
    eventually_formulas_constraints.append(build_OR_multi(one_of_them_is_true))
    eventually_halt_constraints.append(build_OR_multi(one_of_them_terminated))

tau_exclusive_constraints = build_AND_multi(tau_exclusive)
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')


print("building formula (eventually) relational constraints.")
global_eventually_formula= build_OR_multi(eventually_formulas_constraints)
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')


print("building terminating constraints.")
tau_eventually_terminated= build_OR_multi(eventually_halt_constraints)
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')


print("building moving constraints...")
tau_moving = []
for traj in traj_vars_tau:
    index = build_OR2(var_dict[traj[0]], var_dict[traj[1]])
    tau_moving.append(index)
tau_moving_constraints = build_AND_multi(tau_moving)
elapsed = time.time()
print("time elapsed: ", round((elapsed - start), 3), 's')


# for key, value in dict_choices_eachlayer.items():
    # print(key, "  ", value)
# NEW_QCIR.append("# tau1 layer constraints:")
# tau1_exclusive = []
# tau1_exclusive.append(str(var_dict["tau1_phi00[0]"]))
# for key, value in tau1_dict_choices_eachlayer.items():
#     index = build_exclusive_OR(value)
#     tau1_exclusive.append(index)
# tau1_exclusive_constraints = build_AND_multi(tau1_exclusive)
#
#
# NEW_QCIR.append("# tau2 layer constraints:")
# tau2_exclusive = []
# tau2_exclusive.append(str(var_dict["tau2_phi00[0]"]))
# for key, value in tau2_dict_choices_eachlayer.items():
#     # print(key, "  ", value)
#     index = build_exclusive_OR(value)
#     tau2_exclusive.append(index)
# tau2_exclusive_constraints = build_AND_multi(tau2_exclusive)


# print(traj_vars_tau1)
#### NEW: correctness of trajs


# all_moving_constraints = []
## 1. at least one traj moves on each step
# tau1_moving = []
# for traj1 in traj_vars_tau1:
#     index = build_OR2(var_dict[traj1[0]], var_dict[traj1[1]])
#     tau1_moving.append(index)
# tau1_moving_constraints = build_AND_multi(tau1_moving)
#
#
# tau2_moving = []
# for traj2 in traj_vars_tau2:
#     index = build_OR2(var_dict[traj2[0]], var_dict[traj2[1]])
#     tau2_moving.append(index)
# tau2_moving_constraints = build_AND_multi(tau2_moving)




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


### setup new added things
quant_forall=[]
quant_exists=[]
for tau_v in traj_vars_tau:
    quant_forall.extend(tau_v)
for v in To_find:
    quant_exists.append(v)

#### global formula for drone test
# trace = tau_moving_constraints
# trace = build_AND3(tau_all_formulas, tau_moving_constraints, tau_exclusive_constraints)
# traj= build_AND2(tau_all_formulas, tau_exclusive_constraints)



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

header_M1="exists("
for a in vars_A:
    header_M1 = header_M1 + a + ","
header_M1 = header_M1[:-1] ## remove last ','
header_M1 += ")"
write_QCIR.write(header_M1 + '\n')

header_M2="forall("
for b in vars_B:
    header_M2 = header_M2 + b + ","
header_M2 = header_M2[:-1] ## remove last ','
header_M2 += ")"
write_QCIR.write(header_M2 + '\n')


# print(OLD_logics[-1])
# print(OLD_logics[-2])

### fetch the model outputs
M1 = OLD_logics[-1]
M1 = M1.split("(")
M1 = (M1[1].split(","))[0]
print(M1)

M2 = OLD_logics[-2]
M2 = M2.split("(")
M2 = M2[1].split(",")
old_phi = M2[1].replace(")","")
print(old_phi)
M2 = M2[0]
print(M2)



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

FORMULA = str(global_eventually_formula)

# exists = build_AND3(tau_all_formulas, tau_exclusive_constraints, tau_moving_constraints)
# exists = build_AND2(tau_exclusive_constraints, tau_moving_constraints)
# exists = tau_moving_constraints
### GOOD GOOD
Q_tau="exists"
valid = build_AND3(tau_moving_constraints, tau_exclusive_constraints, tau_all_formulas)
FINAL_FORMULA = build_AND2(M1, build_IMPLIES(M2, build_AND2(valid, FORMULA)))


# Q_tau="forall"
# valid = build_AND4(tau_moving_constraints, tau_eventually_terminated, tau_exclusive_constraints, tau_all_formulas)
# valid = build_AND3(tau_eventually_terminated, tau_exclusive_constraints, tau_all_formulas)

# FINAL_FORMULA = build_AND2(M1, build_IMPLIES(M2, build_IMPLIES(valid, FORMULA)))

# FINAL_FORMULA = build_AND2(old_phi, build_AND2(M1, build_AND2(M2, build_IMPLIES(valid, EVENTUALLY))))


# FINAL_FORMULA = build_AND2(output, tau_eventually_terminated)
# FINAL_FORMULA = build_IMPLIES(output, global_eventually_formula)
# FINAL_FORMULA = build_IMPLIES(output, tau_moving_constraints)
# FINAL_FORMULA = output
# FINAL_FORMULA = build_AND3(M1, M2, build_AND2(valid, NOT+EVENTUALLY))

# FINAL_FORMULA = build_AND2(output, str(valid))
# FINAL_FORMULA = build_AND2(output, build_AND2(valid, NOT+EVENTUALLY))
# FINAL_FORMULA = build_IMPLIES(output, build_IMPLIES(valid, EVENTUALLY))
## exists
# FINAL_FORMULA = build_AND2(output, build_AND2(valid, build_AND2(good_traj, EVENTUALLY)))

# FINAL_FORMULA = build_AND2(output, build_IMPLIES(valid, NOT+EVENTUALLY))
# FINAL_FORMULA = build_AND2(output, valid)



# FINAL_FORMULA = build_IMPLIES(output, build_IMPLIES(valid, NOT+EVENTUALLY))
# FINAL_FORMULA = build_IMPLIES(output, valid)
# FINAL_FORMULA = build_AND2(output, build_IMPLIES(valid, build_AND2(good_traj, EVENTUALLY)))
# FINAL_FORMULA = output


# print(To_find)


### good!
# Q_tau="forall"
# # consequence = build_IMPLIES(trace, var_dict["TRUE_A[0]"])
# # consequence = build_IMPLIES(trace, build_AND2(tau_exclusive_constraints, var_dict["TRUE_A[0]"]))
# # consequence = build_IMPLIES(trace, build_AND2(tau_all_formulas, global_eventually_formula))
# # consequence = build_IMPLIES(trace, NOT+str(global_eventually_formula))
# consequence = build_IMPLIES(trace, str(global_eventually_formula))
# # consequence = build_IMPLIES(exists,  str(global_eventually_formula))

# consequence = build_IMPLIES(tau_moving_constraints, str(global_eventually_formula))
# FINAL_FORMULA=build_AND2(output,consequence)

#### ADD Headers
FORALL = Q_tau+"("
for v in quant_forall:
    FORALL += str(var_dict[v]) + ","
# for v in quant_exists:
#     FORALL += str(var_dict[v]) + ","
FORALL = FORALL[:-1] ## remove last ','
FORALL += ")"
# print(FORALL)
write_QCIR.write(FORALL+ '\n')

EXISTS = "exists("
for v in quant_exists:
    EXISTS += str(var_dict[v]) + ","
EXISTS = EXISTS[:-1] ## remove last ','
EXISTS += ")"
# print(EXISTS)
write_QCIR.write(EXISTS+ '\n')




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





### finally, build everything w.r.t. the quantifiers
# forall = build_AND2(tau1_all_formulas, tau1_exclusive_constraints)
# M1 = build_AND3(tau1_moving_constraints, tau1_all_formulas, tau1_exclusive_constraints)


# exists = (tau2_all_formulas)
# exists = build_AND2(tau2_all_formulas, tau2_exclusive_constraints)
# M2 = build_AND3(tau2_moving_constraints, tau2_all_formulas, tau2_exclusive_constraints)

# FINAL_FORMULA =  build_AND3(output, forall, exists)
# FINAL_FORMULA =  build_AND3(forall, exists, global_observable_formulas)
# FORMULA = global_observable_formulas

# TRUE = NOT+str(var_dict["proc1-halt_A[0]"]) ### a dummy true proposition
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
# exclusive_constraints.append(var_dict[tau1_groups[0]])
# exclusive_constraints.append(var_dict[tau2_groups[0]])
# for tau1 in tau1_groups:
#     if (isinstance(tau1, list)):
#         index = build_exclusive_OR(tau1)
#         exclusive_constraints.append(index)
# for tau2 in tau2_groups:
#     if (isinstance(tau2, list)):
#         index = build_exclusive_OR(tau2)
#         exclusive_constraints.append(index)


###  build observable matching formulas:
# for A in (valid_traj_options_tau1):
#     for B in (valid_traj_options_tau2):
#         ## if same layer
#         if (A[0] == B[0]):
#             # print(A, AND, B)
#             tau1_name = "tau1_phi"+str(A[1])+str(A[2])+"["+str(A[0])+"]"
#             tau2_name = "tau2_phi"+str(B[1])+str(B[2])+"["+str(B[0])+"]"
#             key = tau1_name + AND + tau2_name
#
#             relational_conditions = []
#             for k in range(num_observables):
#                 s_proc1 = temp_obs_vars_proc1[k]+M1+timestamp(A[1]) + IFF + temp_obs_vars_proc1[k]+M2+timestamp(B[1])
#                 s_proc2 = temp_obs_vars_proc2[k]+M1+timestamp(A[2]) + IFF + temp_obs_vars_proc2[k]+M2+timestamp(B[2])
#                 relational_conditions.append(s_proc1)
#                 relational_conditions.append(s_proc2)
#
#             observable_vars_formulas[key] = relational_conditions

        ## build constraint:
        # print(relational_conditions)
# print("finished.")
### ---DEBUG---
# print(valid_traj_options_tau1)
# print(valid_traj_options_tau2)
# print(*observable_vars_formulas['tau1_phi01[1] AND tau2_phi01[1]'], sep='\n')
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
# outputs += str(var_dict["tau1_t1[0]"]) + ","
# outputs += str(var_dict["tau1_t2[0]"]) + ","
# outputs += str(var_dict["tau1_phi00[0]"])  +  ","
# outputs += str(var_dict["tau1_phi01[1]"])  +  ","
# outputs += str(var_dict["tau1_phi22[3]"])  +  ","
# outputs += NOT+str(var_dict["tau1_phi03[3]"])  +  ","
# outputs += NOT+str(var_dict["tau1_phi04[4]"])  +  ","
#
# outputs += NOT+str(var_dict["tau2_phi02[2]"])  +  ","
# outputs += NOT+str(var_dict["tau2_phi03[3]"])  +  ","
# outputs += NOT+str(var_dict["tau2_phi04[4]"])  +  ","
# outputs += "1193"  +  ","
# outputs += NOT+str(var_dict["tau1_t1[2]"]) + ","
# outputs += str(var_dict["tau1_t2[2]"]) + ","
# outputs += str(var_dict["tau1_phi23[3]"])  +  ","
#
# outputs += NOT+str(var_dict["tau1_t1[1]"]) + ","
# outputs += str(var_dict["tau1_t2[1]"]) + ","
#
# outputs += str(var_dict["tau1_t1[2]"]) + ","
# outputs += NOT+str(var_dict["tau1_t2[2]"]) + ","
# outputs += str(var_dict["tau2_t1[0]"]) + ","
# outputs += str(var_dict["tau2_t2[0]"]) + ","
#
# outputs += str(var_dict["tau1_t1[1]"]) + ","
# outputs += str(var_dict["tau1_t2[1]"]) + ","
# outputs += str(var_dict["tau2_t1[1]"]) + ","
# outputs += str(var_dict["tau2_t2[1]"]) + ","
#
# outputs += str(var_dict["tau1_t1[2]"]) + ","
# outputs += str(var_dict["tau1_t2[2]"]) + ","
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

# print(traj_vars_tau1)
# counter = 0
# print(tau1_formulas)

### build final formulas
# for key, value in tau1_formulas.items():
#     print(key)
#     # print(value)
#     ### concatenate four different transitions:
#     traj_advance = []
#     bits_vars = traj_vars_tau1[counter]
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
#     tau1_formulas[key] = temp_value
#     # print(*value, sep = "\n")
#     print(*traj_advance, sep = "\n")


# traj_advance = []
# for t in range(len_longest_trajectory):
#     bits_vars = traj_vars_tau1[counter]
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
#     for i in range(diameter_proc1):
#         for j in range(diameter_proc2):
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
