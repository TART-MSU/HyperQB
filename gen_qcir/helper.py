import sys
import math
import re

DIR=sys.argv[1]
filename=DIR+"/AB.qcir"

finalline=""
A_vars_max = 0
with open(filename) as f:
        for newline in f:
            if newline.startswith('# '):
                if ('_A[1]' in newline):
                    var_num = newline[newline.find('# ')+2: newline.find(' :')]
                    if(int(var_num) > A_vars_max):
                        A_vars_max = int(var_num)
            else:
                finalline = newline


AND_gate = finalline[finalline.find('(')+1: finalline.find(',')]



file_A = open(DIR+"/A.qcir", "w")
file_B = open(DIR+"/B.qcir", "w")
switch=False
skip=False
A_vars=[]
B_vars=[]
beyond_onestep=[]
with open(filename) as f:
        for newline in f:
            gate_num = newline[:newline.find(" =")]
            if ('#' in newline):
                file_A.write(newline)
                file_B.write(newline)
            elif ('output' in newline):
                continue
            elif ('exists' in newline):
                # file_A.write(newline)
                vars = newline[7:-2]
                A_vars = vars.split(', ')
                onestep_A_vars = []
                for var in A_vars:
                    if (int(var) <= A_vars_max):
                        onestep_A_vars.append(var)
                    else:
                        beyond_onestep.append(var)
                # print(str(A_vars))
                to_print = ','.join([str(v) for v in onestep_A_vars])
                file_A.write('exists('+to_print+')\n')
            elif ('forall' in newline):
                vars = newline[7:-2]
                B_vars = vars.split(', ')
                file_B.write(newline)
            elif (switch):
                curr_vars = newline[newline.find("(")+1:-2]
                curr_vars = curr_vars.split(',')
                # print(vars)
                # if gate_num == (int(AND_gate)-1):
                if(set(A_vars) & set(curr_vars)):
                    break
                file_B.write(newline)
            else:
                vars = newline[newline.find('(')+1:-2]
                vars = vars.split(',')
                for i in range(0, len(vars)):
                    vars[i] = vars[i].replace("-", "")
                # print(vars)
                if (set(vars) & set(beyond_onestep)):
                    skip=True
                if(not skip):
                    file_A.write(newline)

            if gate_num == AND_gate:  
                switch=True    
file_A.close()
file_B.close()

