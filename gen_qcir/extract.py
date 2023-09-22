import sys
import math
import re

DIR=sys.argv[1]
filename=DIR+"/AB.qcir"

ONESTEP_A = False
ONESTEP_B = False
if(sys.argv[2] == 'T'):
    ONESTEP_A = True 
if(sys.argv[3] == 'T'):
    ONESTEP_B = True     

Aline=""
Bline=""
A_vars_max = 0
B_vars_max = 0
with open(filename) as f:
        for newline in f:
            if newline.startswith('# '):
                if ('_A[1]' in newline):
                    var_num = newline[newline.find('# ')+2: newline.find(' :')]
                    if(int(var_num) > A_vars_max):
                        A_vars_max = int(var_num)
                if ('_B[1]' in newline):
                    var_num = newline[newline.find('# ')+2: newline.find(' :')]
                    if(int(var_num) > B_vars_max):
                        B_vars_max = int(var_num)
            else:
                Bline = Aline
                Aline = newline


AND_gate = Aline[Aline.find('(')+1: Aline.find(',')]
OR_gate = '-'+Bline[Bline.find('(')+1: Bline.find(',')]

A_TS = AND_gate
B_TS = OR_gate

# sys.exit()



file_A = open(DIR+"/A.qcir", "w")
file_B = open(DIR+"/B.qcir", "w")
switch_to_B=False
A_skip=False
B_skip=False
A_vars=[]
B_vars=[]
A_beyond_onestep=[]
B_beyond_onestep=[]
with open(filename) as f:
        for newline in f:
            gate_num = newline[:newline.find(" =")]
            if ('#' in newline):
                file_A.write(newline)
                file_B.write(newline)
            elif ('output' in newline):
                continue
            elif ('exists' in newline):
                vars = newline[7:-2]
                A_vars = vars.split(', ')
                if(ONESTEP_A):
                    onestep_A_vars = []
                    for var in A_vars:
                        if (int(var) <= A_vars_max):
                            onestep_A_vars.append(var)
                        else:
                            A_beyond_onestep.append(var)
                    to_print = ','.join([str(v) for v in onestep_A_vars])
                    file_A.write('exists('+to_print+')\n')
                else:
                    file_A.write(newline)

            elif ('forall' in newline):
                vars = newline[7:-2]
                B_vars = vars.split(', ')
                if(ONESTEP_B):
                    onestep_B_vars = []
                    for var in B_vars:
                        if (int(var) <= B_vars_max):
                            onestep_B_vars.append(var)
                        else:
                            B_beyond_onestep.append(var)
                    to_print = ','.join([str(v) for v in onestep_B_vars])
                    file_B.write('forall('+to_print+')\n')
                else:
                    file_B.write(newline)
            elif (not switch_to_B):
                curr_vars = newline[newline.find('(')+1:-2]
                curr_vars = curr_vars.split(',')
                for i in range(0, len(curr_vars)):
                    curr_vars[i] = curr_vars[i].replace("-", "")

                if(ONESTEP_A):
                    if (set(curr_vars) & set(A_beyond_onestep)):
                        A_skip=True
                if(not A_skip):
                    A_TS = gate_num
                    file_A.write(newline)
            else:
                curr_vars = newline[newline.find("(")+1:-2]
                curr_vars = curr_vars.split(',')
                for i in range(0, len(curr_vars)):
                    curr_vars[i] = curr_vars[i].replace("-", "")

                if(set(A_vars) & set(curr_vars)):
                    break # end when property starts

                if(ONESTEP_B):
                    if (set(curr_vars) & set(B_beyond_onestep)):
                        B_skip=True
                if(not B_skip):
                    B_TS = '-'+gate_num
                    file_B.write(newline)

            if gate_num == AND_gate:  
                switch_to_B=True    

print(A_TS)
print(B_TS)
file_A.close()
file_B.close()


# file_onestepA = open(DIR+"/A_onestep.qcir", "w")
# file_onestepB = open(DIR+"/B_onestep.qcir", "w")
