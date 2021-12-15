import re

### logical operators
IFF = " IFF "
AND = " AND "
OR = " OR "
NOT = " -"

M1 = "_A"
M2 = "_B"

### parameter setup
num_observables = 4
num_procs = 2
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


### little helper
def timestamp(t):
    return "["+str(t)+"]"

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
            marktimep = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(marktimep) <= diameter_proc1):
                M1_proc1.append(key)

        elif ("proc2" in key):
            marktimep = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(marktimep) <= diameter_proc2):
                M1_proc2.append(key)

    if ("_B" in key):
        if ("proc1" in key):
            marktimep = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(marktimep) <= diameter_proc1):
                M2_proc1.append(key)

        elif ("proc2" in key):
            marktimep = ((re.findall('\[.*\]', key)[0]).replace('[',"")).replace(']',"");
            if (int(marktimep) <= diameter_proc2):
                M2_proc2.append(key)


# print(*M1_proc1,sep="\n")
# print(*M1_proc2,sep="\n")
# print(*M2_proc1,sep="\n")
# print(*M2_proc2,sep="\n")

### New Variables
new_vars = []

### NEW build relational constraints for tau
### (t1, t2, conditions....)
tau1_formulas = {}
tau2_formulas = {}
# ### build tau_1 constraints
for i in range(diameter_proc1+1):
    for j in range(diameter_proc2+1):
        for k in range(len_longest_trajectory):
            # phi = "tau1_phi"+str(i)+str(j)
            # phi = str(i)+str(j)
            # print(phi)
            # print(phi, 'implies all following is true')
            name = "tau1_phi"+str(i)+str(j)+"["+str(k)+"]"
            new_vars.append(name)
            relational_predicate = "phi"+str(i)+str(j)
            # relational_conditions = ["phi"+str(i)+str(j)]
            relational_conditions = []
            for k in range(num_observables):
                # s = M1_proc1[(diameter_proc1+1)*k+i] + IFF + M1_proc2[(diameter_proc2+1)*k+j]
                ### DUMMY
                s = M1_proc1[(diameter_proc1+1)*k+i] + IFF + M1_proc1[(diameter_proc1+1)*k+i]

                relational_conditions.append(s)
                # print(s)
            tau1_formulas[relational_predicate] = relational_conditions
            tau1_formulas[name] = relational_conditions
            # print(name)
            # print(relational_conditions)

### build tau_2 constraints
for i in range(diameter_proc1+1):
    for j in range(diameter_proc2+1):
        for k in range(len_longest_trajectory):
            # phi = "tau1_phi"+str(i)+str(j)
            # phi = str(i)+str(j)
            # print(phi)
            # print(phi, 'implies all following is true')
            name = "tau2_phi"+str(i)+str(j)+"["+str(k)+"]"
            new_vars.append(name)
            relational_predicate = "phi"+str(i)+str(j)
            # relational_conditions = ["phi"+str(i)+str(j)]
            relational_conditions = []
            for k in range(num_observables):
                # s = M2_proc1[(diameter_proc1+1)*k+i] + IFF + M2_proc2[(diameter_proc2+1)*k+j]
                ### DUMMY
                s = M2_proc1[(diameter_proc1+1)*k+i] + IFF + M2_proc1[(diameter_proc1+1)*k+i]

                relational_conditions.append(s)
                # print(s)
            tau2_formulas[relational_predicate] = relational_conditions
            tau2_formulas[name] = relational_conditions


# print(*observable_vars_formulas, sep="\n")
# print(observable_vars_formulas)
# print(*new_vars, sep="\n")


# print('==')
# print(tau1_formulas)
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

##################  build a new dictionary items to contain all new variables
for v in new_vars:
    var_dict[v] = var_index
    # NEW_QCIR.append("# "+ str(var_index) + " : " + v)
    var_index+=1
# print(new_vars)
print("================")

## transitions:
## (curr_layer, curr_t1, curr_t2, t1&t2(movements), next_layer, next_t1, next_t2)
traj_relations=[]
observable_vars_tau1=[]
observable_vars_tau2=[]
def build_traj_advances(layer, t1, t2):
    ### collect valid trajectory options
    observable_vars_tau1.append([layer, t1, t2])
    observable_vars_tau2.append([layer, t1, t2])

    ## move to the next unrolling
    next_layer=layer+1

    if (t1==diameter_proc1 and t2==diameter_proc2):
        traj_relations.append([layer, t1, t2])
        # print(layer, t1, t2)
        return
    elif (layer==len_longest_trajectory-1):
        traj_relations.append([layer, t1, t2])
        # print(layer, t1, t2)
        return
    else:

        ### let's ban this
        # 00 situation _1
        next_t1_1=t1
        next_t2_1=t2
        # next_1 = [layer, t1, t2, "00", next_layer, next_t1_1, next_t2_1]
        next_1 = [layer, t1, t2, "00"]
        traj_relations.append(next_1)

        ## 01 situation _2
        next_t1_2=t1
        if (t2<diameter_proc2):
            next_t2_2=t2+1
        else: next_t2_2=t2
        next_2 = [layer, t1, t2, "01", next_layer, next_t1_2, next_t2_2]
        traj_relations.append(next_2)


        ## 10 situation _3
        if (t1<diameter_proc1):
            next_t1_3=t1+1
        else: next_t1_3=t1
        next_t2_3=t2
        next_3 = [layer, t1, t2, "10", next_layer, next_t1_3, next_t2_3]
        traj_relations.append(next_3)


        ## 11 situation _4
        if (t1<diameter_proc1):
            next_t1_4=t1+1
        else: next_t1_4=t1
        if (t2<diameter_proc2):
            next_t2_4=t2+1
        else: next_t2_4=t2
        # print("from", curr)
        # print("11, to", next_layer,":", next_t1_4,  next_t2_4, "\n")
        next_4 = [layer, t1, t2, "11", next_layer, next_t1_4, next_t2_4]
        traj_relations.append(next_4)

        ## try all four directions
        # build_traj_advances(next_layer, next_t1_1, next_t2_1)
        build_traj_advances(next_layer, next_t1_2, next_t2_2)
        build_traj_advances(next_layer, next_t1_3, next_t2_3)
        build_traj_advances(next_layer, next_t1_4, next_t2_4)

## recursively build valid transitions
build_traj_advances(0, 0, 0)
print(len(traj_relations))

### remove dups and sort
res = []
for i in traj_relations:
    if i not in res:
        res.append(i)
traj_relations = res
traj_relations.sort()

print(len(traj_relations))
print(*traj_relations,sep="\n")


### build relational constraints over all observable variables
# print(*M1_proc1,sep="\n")
# print(*M2_proc1,sep="\n")
# print(*M1_proc2,sep="\n")
# print(*M2_proc2,sep="\n")
observable_vars_tau1.sort()
observable_vars_tau2.sort()
# print(observable_vars_tau1)
# print(observable_vars_tau2)
temp_obs_vars_proc1 = ["proc1-printA", "proc1-printB", "proc1-printC", "proc1-printD"]
temp_obs_vars_proc2 = ["proc2-printA", "proc2-printB", "proc2-printC", "proc2-printD"]
observable_vars_formulas = {}
###  build observable matching formulas:
for A in (observable_vars_tau1):
    for B in (observable_vars_tau2):
        ## if same layer
        if (A[0] == B[0]):
            # print(A, AND, B)
            tau1_name = "tau1_phi"+str(A[1])+str(A[2])+"["+str(A[0])+"]"
            tau2_name = "tau2_phi"+str(B[1])+str(B[2])+"["+str(B[0])+"]"
            key = tau1_name + AND + tau2_name

            relational_conditions = []
            for k in range(num_observables):
                s_proc1 = temp_obs_vars_proc1[k]+M1+timestamp(A[1]) + IFF + temp_obs_vars_proc1[k]+M2+timestamp(B[1])
                s_proc2 = temp_obs_vars_proc2[k]+M1+timestamp(A[2]) + IFF + temp_obs_vars_proc2[k]+M2+timestamp(B[2])
                relational_conditions.append(s_proc1)
                relational_conditions.append(s_proc2)

            observable_vars_formulas[key] = relational_conditions

# print(*observable_vars_formulas['tau1_phi01[1] AND tau2_phi01[1]'], sep='\n')








global_formula_gate = []


def build_expressions(tau, final_formulas):
    for i in range(0, len(traj_relations), 4):
        r = traj_relations[i]
        # print((r))
        if (len(r) == 3):
            for k in range(i, len(traj_relations)):
                r = traj_relations[k]
                # print(r)
                time = r[0]
                formula_t1=r[1]
                formula_t2=r[2]
                pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", "phi"+ str(formula_t1)+str(formula_t2)]
                final_formulas.append(pre)
            ### finishing base case, break
            break
        else:
            time = r[0]
            formula_t1=r[1]
            formula_t2=r[2]
            pre = ["phi"+str(formula_t1)+str(formula_t2)+"["+str(time)+"]", "phi"+ str(formula_t1)+str(formula_t2)]
            # print(pre)
            next_move = []

            ### the "00 " case
            # next_move.append(i+1)
            # post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
            post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
            next_move.append(post)

            for j in range(3):
                next = traj_relations[i+j+1]
                # print(next)
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

                #
                # if (movement[0]=='0' and movement[1]=='0'):
                #     post = [NOT, tau+"t1["+str(time)+"]", tau+"t2["+str(time)+"]"]
                #     next_move.append(post)
                # else:
                # post = "(" + str(movement)+ AND + str(next_formula_t1) + str(next_formula_t2) + " @" + str(next_time)  + ")"
                post = [t1, t2, "phi"+str(next_formula_t1)+str(next_formula_t2)+"["+ str(next_time)+"]"]
                next_move.append(post)
                # next_move = next_move+post

            # print(pre)
            # print(next_move)
            pre.extend(next_move)
            final_formulas.append(pre)





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
    s = str(var_index)+" = and("+str(a)+","+str(b)+","+str(c)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return s


def build_AND4(a, b, c, d):
    global var_index
    s = str(var_index)+" = and("+str(a)+","+str(b)+","+str(c)+","+str(d)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)

# def build_AND8(a, b, c, d, e, f, g, h):
#     global var_index
#     index=var_index
#     s = str(index)+" = and("+str(a)+","+str(b)+","+str(c)+","+str(d)+","+str(e)+","+str(f)+","+str(g)+","+str(h)+")"
#     var_index+=1
#     NEW_QCIR.append(s)
#     print(s)
#     print(index)
#     return index



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

def build_OR2(a,b):
    global var_index
    s = str(var_index)+" = or("+str(a)+","+str(b)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return s

def build_OR3(a,b,c):
    global var_index
    s = str(var_index)+" = or("+str(a)+","+str(b)+","+str(c)+")"
    var_index+=1
    # print(s)
    NEW_QCIR.append(s)
    return s

def build_OR4(a,b,c,d):
    global var_index
    s = str(var_index)+" = or("+str(a)+","+str(b)+","+str(c)+","+str(d)+")"
    var_index+=1
    NEW_QCIR.append(s)
    return s
    # print(s)

def build_IFF(a,b):
    global var_index
    not_a = "-"+a
    not_b = "-"+b
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
    print(s)
    NEW_QCIR.append(s)
    var_index+=1
    return index


gates_dict = {}

# print(len(traj_relations))
# print(*traj_relations,sep="\n")

#### matching the observables
for key, value in observable_vars_formulas.items():
    print("===", key, "===")
    # print(value)



all_pair_formulas = []
for key, value in observable_vars_formulas.items():
    keys = key.split(' AND ')
    print(keys)
    print(value)
    key_index = build_AND2(var_dict[keys[0]], var_dict[keys[1]])
    # print("key", key_index)

    fff=observable_vars_formulas[key]
    all_observable_pairs = []
    for i in range (num_procs*num_observables):
        c = (fff[i].split(' IFF '))
        a = var_dict[c[0]]
        b = var_dict[c[1]]
        if ((a+IFF+b) in gates_dict):
            num=gates_dict[a+IFF+b]
        else:
            num = build_IFF(a, b)
            gates_dict[a+IFF+b]=num
        # print(num)
        all_observable_pairs.append(num)
    # print(all_observable_pairs)
    RS = build_AND_multi(all_observable_pairs)
    print("values", RS)

    var_pair_formula = build_IMPLIES(key_index, RS)
    all_pair_formulas.append(var_pair_formula)

# print(all_pair_formulas)
final_observable_formulas = build_AND_multi(all_pair_formulas)
# final_observable_formulas = "1"


### convert all expressions
def convert_to_qcir(tau, expressions):
    for e in expressions:
        exp=e
        # print()
        print(exp)
        formula_prefix = var_dict[tau+exp[0]]
        formula_suffix = []

        #### No we don't need this
        ## relational constraints
        # RC = tau1_formulas[exp[1]]
        # # print(RC)
        # all_c = []
        # for j in range(num_observables):
        #     global var_index
        #     c = (RC[j].split(' IFF '))
        #     # a = c[0]
        #     # b = c[1]
        #     a = var_dict[c[0]]
        #     b = var_dict[c[1]]
        #     not_a = "-"+a
        #     not_b = "-"+b
        #     index1=var_index
        #     LR=build_OR2(not_a, b)
        #     index2=var_index
        #     RL=build_OR2(not_b, a)
        #     s = (str(var_index) + " = and(" + str(index1) +"," + str(index2) +")")
        #     # print(s)
        #     NEW_QCIR.append(s)
        #     all_c.append(var_index)
        #     var_index+=1

        #### NEED to be more flexible
        ### if it's the last layer
        if (len(exp)==2):
            # print(exp)
            # RC = tau1_formulas[exp[1]]
            # all_c = []
            # for j in range(num_observables):
            #     global var_index
            #     c = (RC[j].split(' IFF '))
            #     # a = c[0]
            #     # b = c[1]
            #     a = var_dict[c[0]]
            #     b = var_dict[c[1]]
            #     not_a = "-"+a
            #     not_b = "-"+b
            #     index1=var_index
            #     LR=build_OR2(not_a, b)
            #     index2=var_index
            #     RL=build_OR2(not_b, a)
            #     s = (str(var_index) + " = and(" + str(index1) +"," + str(index2) +")")
            #     # print(s)
            #     NEW_QCIR.append(s)
            #     all_c.append(var_index)
            #     var_index+=1
            # s = str(formula_prefix) + " = and(" + str(all_c[0]) + "," + str(all_c[1]) + "," + str(all_c[2]) + "," + str(all_c[3]) + ")"
            ### DUMMY true
            s = str(formula_prefix) + " = or(-" + str(var_dict["LOCKED_A[0]"]) + "," + str(var_dict["LOCKED_A[0]"]) + ")"
            # print(s)
            NEW_QCIR.append(s)
            # continue;
        else:
            ## we don't allow 00 trajectory
            # print("--->", exp[2])
            not_00 = var_index
            build_AND2("-"+str(var_dict[exp[2][1]]), "-"+str(var_dict[exp[2][2]]))
            # print(tau+exp[1])
            # print(RC)
            # formula_suffix.append(var_index)
            # build_AND4(all_c[0], all_c[1], all_c[2], all_c[3])
            # print(var_dict)

            OR_together = []
            for i in range(3,6):
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
                # print(e[2])
                c = var_dict[tau+e[2]]
                OR_together.append(var_index)
                build_AND3(a, b, c)
                # var_index+=1
            formula_suffix.append(var_index)
            # build_OR4(OR_together[0], OR_together[1],OR_together[2], OR_together[3])
            ### move 01, 10, 11, or dummy 00 (no effect)
            build_OR4(OR_together[0], OR_together[1], OR_together[2], str(not_00))
            # print(formula_suffix)
            ## final build
            # print(exp)
            # print(var_dict[tau+exp[0]])
            # build_AND2(formula_suffix[0], formula_suffix[1])

            ### change
            # s = str(formula_prefix) + " = and(" + str(formula_suffix[0]) +"," + str(formula_suffix[1]) + ")"
            s = str(formula_prefix) + " = and(" + str(formula_suffix[0]) + ")"
            # print(s)
            NEW_QCIR.append(s)
            # index = build_IFF(str(formula_prefix), str(formula_suffix[0]))
            # global_formula_gate.append(index)

        # print(str(formula_prefix), str(var_index))

# print(*new_vars, sep="\n")

tau1_exp=[]
build_expressions("tau1_", tau1_exp)
# print(tau1_exp)
convert_to_qcir("tau1_", tau1_exp)

tau2_exp=[]
build_expressions("tau2_", tau2_exp)
convert_to_qcir("tau2_", tau2_exp)

# print("??", global_formula_gate)
# global_formula = build_AND_multi(global_formula_gate)


print("-------------(TO ADD)------------")

open_QCIR = open("HQ.qcir", "r")
QCIR = open_QCIR.read()

OLD_header = re.findall('#QCIR.*', QCIR)
OLD_forall = re.findall('forall.*', QCIR)[0]
OLD_logics = re.findall('.*=.*', QCIR)
OLD_vars = re.findall('#\s.*', QCIR)
OLD_output = re.findall('output.*', QCIR)

# a_file.writelines(list_of_lines)

write_QCIR = open("HQ_async.qcir", "w")
write_QCIR.write(OLD_header[0]+ '\n')
# write_QCIR.write(OLD_forall[0]+ '\n')


debug_QCIR = open("debug.qcir", "w")

quant_forall=[]
for tau1_v in traj_vars_tau1:
    quant_forall.extend(tau1_v)
print("FORALL: ", quant_forall)
quant_exists=[]
for tau2_v in traj_vars_tau2:
    quant_exists.extend(tau2_v)
print("EXISTS: ", quant_exists)

OLD_forall=OLD_forall.replace("forall(", "")
OLD_forall=OLD_forall.replace(")", "")

FORALL = "forall("
FORALL += OLD_forall
FORALL += ","
for v in quant_forall:
    FORALL += str(var_dict[v]) + ","
FORALL = FORALL[:-1] ## remove last ','
FORALL += ")"
# print(FORALL)
EXISTS = "exists("
for v in quant_exists:
    EXISTS += str(var_dict[v]) + ","
EXISTS = EXISTS[:-1] ## remove last ','
EXISTS += ")"
# print(EXISTS)

write_QCIR.write(FORALL+ '\n')
write_QCIR.write(EXISTS+ '\n')


#### create a new output gate
# print(OLD_output)
# print(var_dict["tau1_phi00[0]"])
# print(var_dict["tau2_phi00[0]"])
output = OLD_output[0].replace("output(", "")
output = output.replace(")", "")
out = str(var_index)
NEW_output_gate = "output("+out+")"
# NEW_output_formula = out + " = and(" + str(6581) +")"


write_QCIR.write(NEW_output_gate + '\n')


### merge both logics
for l in OLD_logics:
    write_QCIR.write(l + '\n')


### DEBUG cyclic!!
# for n in range(71):
#     write_QCIR.write(NEW_QCIR[n] + '\n')

for n in NEW_QCIR:
    write_QCIR.write(n + '\n')

for new in new_vars:
    print(new,  ": ", var_dict[new])

# print(*new_vars, sep="\n")
### plave the last formula
outputs = str(var_index) + " = and("
outputs += str(output) +  ","
outputs += str(var_dict["tau1_phi00[0]"])  +  ","
outputs += str(var_dict["tau2_phi00[0]"])  +  ","
outputs += str(final_observable_formulas)
outputs += ")"
write_QCIR.write(outputs + '\n')
# NEW_output_formula = str(var_index) + " = and(" + str(output)  + "," + str(var_dict["tau1_phi00[0]"]) + "," + str(var_dict["tau2_phi00[0]"]) + "," + str(final_observable_formulas) + ")"
# NEW_output_formula = str(var_index) + " = and(" + str(output)  + "," + str(var_dict["tau1_phi00[0]"]) + "," + str(var_dict["tau2_phi00[0]"]) + "," + str(var_dict["tau2_phi12[2]"]) + ")"
# write_QCIR.write(NEW_output_formula + '\n')

### merge both mappings
for v in OLD_vars:
    write_QCIR.write(v  + '\n')
for v in new_vars:
    write_QCIR.write("# "+ str(var_dict[v]) + " : " + v+ '\n')
    # print("# "+ str(var_dict[v]) + " : " + v)
    # debug_QCIR.write("# "+ str(var_dict[v]) + " : " + v+ '\n')


# print(*NEW_QCIR, sep="\n")

print("total num of new gates: ", len(NEW_QCIR))
for n in NEW_QCIR:
    debug_QCIR.write(n+ '\n')









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
