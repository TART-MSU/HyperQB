import re

### logical operators
IFF = " IFF "
AND = " AND "
OR = " OR "
NOT = " -"

### parameter setup
num_observables = 4;
# diameter_proc1 = 5
# diameter_proc2 = 6

### test
diameter_proc1 = 1
diameter_proc2 = 2


open_QCIR = open("HQ.qcir", "r")
# print(f.read())
QCIR=open_QCIR.read()
vars = re.findall('#\s.*', QCIR)
# print(re.findall('#\s.*', QCIR))

var_dict = {}
for var in vars:
    var = (var.split(" "))
    # print(var)
    var_dict[var[1]] = var[3];

M1_proc1 = []
M1_proc2 = []
M2_proc1 = []
M2_proc2 = []

new_vars = []

### fetch variables for tau1 and tau2
for key, value in var_dict.items():
    if ("print" not in value):
        continue;
    if ("_A" in value):
        if ("proc1" in value):
            time_stemp = ((re.findall('\[.*\]', value)[0]).replace('[',"")).replace(']',"");
            if (int(time_stemp) < diameter_proc1):
                # print(value)
                # print(time_stemp)
                M1_proc1.append(value)

        elif ("proc2" in value):
            # print(value)
            M1_proc2.append(value)

    if ("_B" in value):
        if ("proc1" in value):
            # print(value)
            time_stemp = ((re.findall('\[.*\]', value)[0]).replace('[',"")).replace(']',"");
            if (int(time_stemp) < diameter_proc2):
                # print(time_stemp)
                # print(value)
                M2_proc1.append(value)

        elif ("proc2" in value):
            # print(value)
            M2_proc2.append(value)


tau1_formulas = {}
tau2_formulas = {}
### build tau_1 constraints
for i in range(diameter_proc1):
    for j in range(diameter_proc2):
            phi = "tau1_phi"+str(i)+str(j)
            # print(phi)
            # print(phi, 'implies all following is true')
            relational_conditions = []
            for k in range(num_observables):
                s = M1_proc1[diameter_proc1*k+i] + IFF + M1_proc2[diameter_proc2*k+j]
                relational_conditions.append(s)
                # print(s)
            tau1_formulas[phi] = relational_conditions
### build tau_2 constraints
for i in range(diameter_proc1):
    for j in range(diameter_proc2):
            phi = "tau2_phi"+str(i)+str(j)
            # print(phi, 'implies all following is true')
            relational_conditions = []
            for k in range(num_observables):
                s = M2_proc1[diameter_proc1*k+i] + IFF + M2_proc2[diameter_proc2*k+j]
                relational_conditions.append(s)
                # print(s)
            tau2_formulas[phi] = relational_conditions

print(tau1_formulas)
# print(tau2_formulas)

### build trajectory variables
len_longest_trajectory = diameter_proc1 + diameter_proc2

traj_vars_tau1 =[]
traj_vars_tau2 =[]
# find the longest trajectory
for i in range(len_longest_trajectory):
    tau1_v1 = "tau1_t1["+str(i)+"]"
    tau1_v2 = "tau1_t2["+str(i)+"]"
    tau1_bits = []
    tau1_bits.append(tau1_v1)
    tau1_bits.append(tau1_v2)
    # traj_vars_tau1.append(tau1_v1)
    # traj_vars_tau1.append(tau1_v2)
    traj_vars_tau1.append(tau1_bits)
    # print(tau1_v1, tau1_v2)
    tau2_v1 = "tau2_t1["+str(i)+"]"
    tau2_v2 = "tau2_t2["+str(i)+"]"
    tau2_bits = []
    tau2_bits.append(tau2_v1)
    tau2_bits.append(tau2_v2)
    # traj_vars_tau2.append(tau2_v1)
    # traj_vars_tau2.append(tau2_v2)
    traj_vars_tau2.append(tau2_bits)
    # print(tau1_v1, tau2_v2)

    ### save it to assign new numbers
    new_vars.append(tau1_v1)
    new_vars.append(tau1_v2)
    new_vars.append(tau2_v1)
    new_vars.append(tau2_v2)
# print(var_dict)
# print(traj_vars_tau1)
# print(traj_vars_tau2)

## get the index of gates and variables
nums = re.findall(r'\d+', QCIR)
nums = list(map(int, nums))
nums.sort()
# print(nums)

## get the starting index of the extra variables
var_index = nums[-1]+1

##  build a new dictionary to contain all variables
# print(new_vars)
# new_vars_dict={}
for v in new_vars:
    var_dict[v] = var_index
    var_index = var_index+1


# print(traj_vars_tau1)
counter = 0

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


traj_advance = []
for t in range(len_longest_trajectory):
    bits_vars = traj_vars_tau1[counter]
    traj_advance.append("from"+str(t)+ ": 00 --->"+ NOT+bits_vars[0]+NOT+bits_vars[1]+AND+"TO:"+str(t+1))
    traj_advance.append("from"+str(t)+ ": 01 --->"+ NOT+bits_vars[0]+    bits_vars[1]+AND+"TO:"+str(t+1))
    traj_advance.append("from"+str(t)+ ": 10 ---> "+     bits_vars[0]+NOT+bits_vars[1]+AND+"TO:"+str(t+1))
    traj_advance.append("from"+str(t)+ ": 11 --->"+ NOT+bits_vars[0]+NOT+bits_vars[1]+AND+"TO:"+str(t+1))
    traj_advance.append("\n")


# print(*traj_advance, sep = "\n")

# traj_relations = {}
# for k in range(len_longest_trajectory):
#     traj_relations[k]=[]

traj_relations=[]

## transitions:
## (curr_layer, curr_t1, curr_t2, t1&t2, next_layer, next_t1, next_t2)


def build_traj_advances(layer, t1, t2):
    ## move to the next unrolling
    next_layer=layer+1

    if (t1>diameter_proc1 or t2>diameter_proc2):
        # print("STOP at;", layer, t1, t2)
        return
    elif (layer>len_longest_trajectory):
        # print("STOP at", layer,  t1, t2)
        return
    else:

        ## 00 situation _1
        next_t1_1=t1
        next_t2_1=t2
        # print("from", curr)
        # print("00, to", next_layer,":", next_t1_1,  next_t2_1, "\n")
        # next_1 = []
        next_1 = [layer, t1, t2, "00", next_layer, next_t1_1, next_t2_1]
        # curr_1=[]
        # curr_1.append(layer)
        # curr_1.append(t1)
        # curr_1.append(t2)
        # curr_1.append("00")
        # # next_1.append("00")
        # next_1.append(next_layer)
        # next_1.append(next_t1_1)
        # next_1.append(next_t2_1)
        # # print(curr_1, next_1)
        # curr_1.extend(next_1)
        # print(curr_1)
        traj_relations.append(next_1)



        ## 01 situation _2
        next_t1_2=t1
        if (t2<=diameter_proc2):
            next_t2_2=t2+1
        else: next_t2_2=t2
        # print("from", curr)
        # print("01, to", next_layer,":", next_t1_2,  next_t2_2, "\n")
        next_2 = []
        curr_2=[]
        curr_2.append(layer)
        curr_2.append(t1)
        curr_2.append(t2)
        curr_2.append("trajs: 01")
        # next_1.append("00")
        next_2.append(next_layer)
        next_2.append(next_t1_2)
        next_2.append(next_t2_2)
        # print(curr_2, next_2)
        # traj_relations[curr_2] = next_2


        ## 10 situation _3
        if (t1<=diameter_proc1):
            next_t1_3=t1+1
        else: next_t1_3=t1
        next_t2_3=t2
        # print("from", curr)
        # print("10, to", next_layer,":", next_t1_3,  next_t2_3, "\n")
        next_3 = []
        curr_3=[]
        curr_3.append(layer)
        curr_3.append(t1)
        curr_3.append(t2)
        curr_3.append("trajs: 10")
        next_3.append(next_layer)
        next_3.append(next_t1_3)
        next_3.append(next_t2_3)
        # print(curr_3, next_3)
        # traj_relations[curr_3] = next_3


        ## 11 situation _4
        if (t1<=diameter_proc1):
            next_t1_4=t1+1
        else: next_t1_4=t1
        if (t2<=diameter_proc2):
            next_t2_4=t2+1
        else: next_t2_4=t2
        # print("from", curr)
        # print("11, to", next_layer,":", next_t1_4,  next_t2_4, "\n")
        next_4 = []
        curr_4=[]
        curr_4.append(layer)
        curr_4.append(t1)
        curr_4.append(t2)
        curr_4.append("trajs: 11")
        next_4.append(next_layer)
        next_4.append(next_t1_4)
        next_4.append(next_t2_4)
        # print(curr_4, next_4)
        # traj_relations[curr_4] = next_4

        ## try all four directions
        build_traj_advances(next_layer, next_t1_1, next_t2_1)
        build_traj_advances(next_layer, next_t1_2, next_t2_2)
        build_traj_advances(next_layer, next_t1_3, next_t2_3)
        build_traj_advances(next_layer, next_t1_4, next_t2_4)


build_traj_advances(0, 0, 0)

traj_relations.sort()
print(*traj_relations,sep="\n")



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
