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
diameter_proc1 = 2
diameter_proc2 = 3

len_longest_trajectory = diameter_proc1 + diameter_proc2


open_QCIR = open("HQ.qcir", "r")
# print(f.read())
QCIR=open_QCIR.read()
vars = re.findall('#\s.*', QCIR)
# print(re.findall('#\s.*', QCIR))
####### NEW THING ADDED to QCIR
NEW_QCIR = []

#### containing all variables with their numerical representation
#### format: NAME: NUMBER
var_dict = {}
for var in vars:
    var = (var.split(" "))
    # print(var)
    # var_dict[var[1]] = var[3];
    var_dict[var[3]] = var[1];


### fetch variables for tau1 and tau2
M1_proc1 = []
M1_proc2 = []
M2_proc1 = []
M2_proc2 = []
for key, value in var_dict.items():
    if ("print" not in key):
        continue;
    if ("_A" in key):
        if ("proc1" in key):
            time_stemp = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(time_stemp) < diameter_proc1):
                M1_proc1.append(value)

        elif ("proc2" in key):
            time_stemp = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(time_stemp) < diameter_proc2):
                M1_proc2.append(value)

    if ("_B" in key):
        if ("proc1" in key):
            time_stemp = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(time_stemp) < diameter_proc2):
                M2_proc1.append(value)

        elif ("proc2" in key):
            time_stemp = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(time_stemp) < diameter_proc2):
                M2_proc2.append(value)

# print(*M1_proc1,sep="\n")
# print(*M1_proc2,sep="\n")


### OLD build relational constraints for tau
# tau1_formulas = {}
# tau2_formulas = {}
# ### build tau_1 constraints
# for i in range(diameter_proc1):
#     for j in range(diameter_proc2):
#             phi = "tau1_phi"+str(i)+str(j)
#             # print(phi)
#             # print(phi, 'implies all following is true')
#             relational_conditions = []
#             for k in range(num_observables):
#                 s = M1_proc1[diameter_proc1*k+i] + IFF + M1_proc2[diameter_proc2*k+j]
#                 relational_conditions.append(s)
#                 # print(s)
#             tau1_formulas[phi] = relational_conditions
### build tau_2 constraints
# for i in range(diameter_proc1):
#     for j in range(diameter_proc2):
#             phi = "tau2_phi"+str(i)+str(j)
#             # print(phi, 'implies all following is true')
#             relational_conditions = []
#             for k in range(num_observables):
#                 s = M2_proc1[diameter_proc1*k+i] + IFF + M2_proc2[diameter_proc2*k+j]
#                 relational_conditions.append(s)
#                 # print(s)
#             tau2_formulas[phi] = relational_conditions
# print(tau1_formulas)
# print(tau2_formulas)

### New Variables
new_vars = []

### NEW build relational constraints for tau
### (t1, t2, conditions....)
tau1_formulas = {}
# tau2_formulas = {}
# ### build tau_1 constraints
for i in range(diameter_proc1):
    for j in range(diameter_proc2):
        for k in range(len_longest_trajectory):
            # phi = "tau1_phi"+str(i)+str(j)
            # phi = str(i)+str(j)
            # print(phi)
            # print(phi, 'implies all following is true')
            name = "phi"+str(i)+str(j)+"["+str(k)+"]"
            new_vars.append(name)
            relational_predicate = "phi"+str(i)+str(j)
            # relational_conditions = ["phi"+str(i)+str(j)]
            relational_conditions = []
            for k in range(num_observables):
                s = M1_proc1[diameter_proc1*k+i] + IFF + M1_proc2[diameter_proc2*k+j]
                relational_conditions.append(s)
                # print(s)
            tau1_formulas[relational_predicate] = relational_conditions
            # tau1_formulas.append(relational_conditions)
### build tau_2 constraints
# for i in range(diameter_proc1):
#     for j in range(diameter_proc2):
#             phi = "tau2_phi"+str(i)+str(j)
#             # print(phi, 'implies all following is true')
#             relational_conditions = []
#             for k in range(num_observables):
#                 s = M2_proc1[diameter_proc1*k+i] + IFF + M2_proc2[diameter_proc2*k+j]
#                 relational_conditions.append(s)
#                 # print(s)
#             tau2_formulas[phi] = relational_conditions
# print(*tau1_formulas,sep="\n")
# print(tau2_formulas)

### build trajectory variables
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

##################  build a new dictionary to contain all variables
# print(new_vars)
# new_vars_dict={}
for v in new_vars:
    var_dict[v] = var_index
    # NEW_QCIR.append("# "+ str(var_index) + " : " + v)
    var_index+=1



# print(traj_vars_tau1)
print(new_vars)
print("================")
# print(var_dict)


# print(*traj_advance, sep = "\n")

traj_relations=[]
## transitions:
## (curr_layer, curr_t1, curr_t2, t1&t2(movements), next_layer, next_t1, next_t2)


def build_traj_advances(layer, t1, t2):
    ## move to the next unrolling
    next_layer=layer+1

    if (t1==diameter_proc1-1 or t2==diameter_proc2-1):
        # next_0 = [layer, t1, t2, "00", next_layer, t1, t2]
        # traj_relations.append(next_0)
        # print("STOP at;", layer, t1, t2)
        return
    elif (layer==len_longest_trajectory-1):
        # print("STOP at", layer,  t1, t2)
        return
    else:

        ## 00 situation _1
        next_t1_1=t1
        next_t2_1=t2
        # print("00, to", next_layer,":", next_t1_1,  next_t2_1, "\n")
        next_1 = [layer, t1, t2, "00", next_layer, next_t1_1, next_t2_1]
        traj_relations.append(next_1)



        ## 01 situation _2
        next_t1_2=t1
        if (t2<diameter_proc2-1):
            next_t2_2=t2+1
        else: next_t2_2=t2
        # print("from", curr)
        # print("01, to", next_layer,":", next_t1_2,  next_t2_2, "\n")
        next_2 = [layer, t1, t2, "01", next_layer, next_t1_2, next_t2_2]
        traj_relations.append(next_2)


        ## 10 situation _3
        if (t1<diameter_proc1-1):
            next_t1_3=t1+1
        else: next_t1_3=t1
        next_t2_3=t2
        # print("from", curr)
        # print("10, to", next_layer,":", next_t1_3,  next_t2_3, "\n")
        next_3 = [layer, t1, t2, "10", next_layer, next_t1_3, next_t2_3]
        traj_relations.append(next_3)


        ## 11 situation _4
        if (t1<diameter_proc1-1):
            next_t1_4=t1+1
        else: next_t1_4=t1
        if (t2<diameter_proc2-1):
            next_t2_4=t2+1
        else: next_t2_4=t2
        # print("from", curr)
        # print("11, to", next_layer,":", next_t1_4,  next_t2_4, "\n")
        next_4 = [layer, t1, t2, "11", next_layer, next_t1_4, next_t2_4]
        traj_relations.append(next_4)

        ## try all four directions
        build_traj_advances(next_layer, next_t1_1, next_t2_1)
        build_traj_advances(next_layer, next_t1_2, next_t2_2)
        build_traj_advances(next_layer, next_t1_3, next_t2_3)
        build_traj_advances(next_layer, next_t1_4, next_t2_4)

build_traj_advances(0, 0, 0)


res = []
for i in traj_relations:
    if i not in res:
        res.append(i)
traj_relations = res
traj_relations.sort()
# print(len(traj_relations))
print(*traj_relations,sep="\n")


def build_expressions(tau, final_formulas):
    for i in range(0, len(traj_relations), 4):
        r = traj_relations[i]
        time = r[0]
        formula_t1=r[1]
        formula_t2=r[2]

        pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", "phi"+ str(formula_t1)+str(formula_t2)]

        # next_move = ""
        next_move = []
        for j in range(4):
            next = traj_relations[i+j]

            movement=next[3]
            next_time=next[4]
            next_formula_t1=next[5]
            next_formula_t2=next[6]

            if (movement[0]=='0'):
                t1="-"+tau+"t1["+str(time)+"]"
            else:
                t1=tau+"t1["+str(time)+"]"

            if (movement[1]=='0'):
                t2="-"+tau+"t2["+str(time)+"]"
            else:
                t2=tau+"t2["+str(time)+"]"

            # post = "(" + str(movement)+ AND + str(next_formula_t1) + str(next_formula_t2) + " @" + str(next_time)  + ")"
            post = [t1, t2, "phi"+str(next_formula_t1)+str(next_formula_t2)+"["+ str(next_time)+"]"]
            # if (j != 3): post = post + OR


            # next_move = next_move+post
            next_move.append(post)

        # print(pre)
        # print(next_move)
        pre.extend(next_move)
        final_formulas.append(pre)




### build QCIR gates:
def build_AND2(a, b):
    global var_index
    s = str(var_index)+" = and("+str(a)+ ","+ str(b)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)


def build_AND3(a, b, c):
    global var_index
    s = str(var_index)+" = and("+str(a)+","+str(b)+","+str(c)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)


def build_AND4(a, b, c, d):
    global var_index
    s = str(var_index)+" = and("+str(a)+","+str(b)+","+str(c)+","+str(d)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)


def build_OR2(a,b):
    global var_index
    s = str(var_index)+" = or("+str(a)+","+str(b)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    # return s


def build_OR4(a,b,c,d):
    global var_index
    s = str(var_index)+" = or("+str(a)+","+str(b)+","+str(c)+","+str(d)+")"
    var_index+=1
    NEW_QCIR.append(s)
    # print(s)


def build_IFF(a,b):
    global var_index
    not_a = "-"+a
    not_b = "-"+b
    index1=var_index
    LR=build_OR2(not_a, b)
    index2=var_index
    RL=build_OR2(not_b, a)
    s = (str(var_index) + " = and(" + str(index1) +"," + str(index2) +")")
    # print(s)
    NEW_QCIR.append(s)
    var_index+=1



tau1_exp=[]
build_expressions("tau1_", tau1_exp)
# print(tau1_exp)
### convert all expressions in tau1
for e in tau1_exp:
    # exp=tau1_exp[0]
    # print(var_dict[exp[0]])
    exp=e
    print(exp)

    formula_prefix = var_dict[exp[0]]
    formula_suffix = []

    ## relational constraints
    # print(tau1_formulas[exp[1]])

    RC = tau1_formulas[exp[1]]
    all_c = []
    for j in range(num_observables):
        c = (RC[j].split(' IFF '))
        # build_IFF(c[0], c[1])
        a = c[0]
        b = c[1]

        not_a = "-"+a
        not_b = "-"+b
        index1=var_index
        LR=build_OR2(not_a, b)
        index2=var_index
        RL=build_OR2(not_b, a)
        s = (str(var_index) + " = and(" + str(index1) +"," + str(index2) +")")
        # print(s)
        NEW_QCIR.append(s)
        all_c.append(var_index)
        var_index+=1

    formula_suffix.append(var_index)

    #### NEED to be more flexible
    build_AND4(all_c[0], all_c[1], all_c[2], all_c[3])
    # print(var_dict)



    OR_together = []
    for i in range(2,6):
        e = exp[i]
        # print(e)
        ###  extract t1
        if ("-" in e[0]):
            temp = e[0].replace("-","")
            temp = "-"+str(var_dict[temp])
            # print(temp)
            a = temp
        else:
            # print(var_dict[e[0]])
            a = var_dict[e[0]]

        ###  extract t2
        if ("-" in e[1]):
            temp2 = e[1].replace("-","")
            temp2 = "-"+str(var_dict[temp2])
            # print(temp2)
            b = temp2
        else:
            # print(var_dict[e[1]])
            b = var_dict[e[1]]

        ###  extract next phi
        # print(var_dict[e[2]])
        c = var_dict[e[2]]
        build_AND3(a, b, c)
        OR_together.append(var_index)
        # var_index+=1
    formula_suffix.append(var_index)
    build_OR4(OR_together[0], OR_together[1],OR_together[2], OR_together[3])

    # print(formula_suffix)
    ## final build
    build_AND2(formula_suffix[0], formula_suffix[1])
    build_IFF(str(formula_prefix), str(var_index))



print("-------------(TO ADD)------------")
quant_forall=[]
for tau1_v in traj_vars_tau1:
    quant_forall.extend(tau1_v)

print("FORALL: ", quant_forall)

quant_exists=[]
for tau2_v in traj_vars_tau2:
    quant_exists.extend(tau2_v)

print("EXISTS: ", quant_exists)


FORALL = "forall("
for v in quant_forall:
    FORALL += str(var_dict[v]) + ","
FORALL = FORALL[:-1] ## remove last ','
FORALL += ")"
print(FORALL)

EXISTS = "exists("
for v in quant_exists:
    EXISTS += str(var_dict[v]) + ","
EXISTS = EXISTS[:-1] ## remove last ','
EXISTS += ")"
print(EXISTS)



for v in new_vars:
    NEW_QCIR.append("# "+ str(var_dict[v]) + " : " + v)

# print(*NEW_QCIR, sep="\n")









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
