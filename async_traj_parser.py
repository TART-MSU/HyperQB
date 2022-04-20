import re

# open_QCIR = open("OUTPUT_byName.cex", "r")
open_QCIR = open("OUTPUT_byTime.cex", "r")
# print("???")
# print(f.read())
QCIR=open_QCIR.read()
vars = re.findall('tau.*', QCIR)

# print(vars)

phi = []
for v in vars:
    if ("pos" in v):
        phi.append(v)
    # else:
    #     # if ("tau1" in v):
    #     if("tau2_t1[0]" in v):
    #         print()
    #     print(v)


def atoi(text):
    return int(text) if text.isdigit() else text

def natural_keys(text):
    '''
    alist.sort(key=natural_keys) sorts in human order
    http://nedbatchelder.com/blog/200712/human_sorting.html
    (See Toothy's implementation in the comments)
    '''
    pre=(re.findall('tau.*\[', text))
    text = text.replace(pre[0], "")
    text = text.replace("]", "")
    # text = re.compile('\((\[\d+\])\)')
    print(text)
    return [ text ]




for v in vars:
    if ("tau_t1" in v):
        print(v)

for v in vars:
    if ("tau_t2" in v):
        print(v)


for v in vars:
    if (("pos" in v) and ("= 1" in v)):
        print(v)


print()
for p in phi:
    if ("tau1" in p):
        # print(p)
        if ("= 1" in p):
            print(p)
print()
# print("[ tau2 trajectories ]")
for v in vars:
    if ("tau2_t" in v):
        print(v)
print()
for p in phi:
    if ("tau2" in p):
        # print(p)
        if ("= 1" in p):
            print(p)
# print(phi)
