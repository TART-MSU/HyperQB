import re

open_QCIR = open("OUTPUT_byName.cex", "r")
# print(f.read())
QCIR=open_QCIR.read()
vars = re.findall('tau.*', QCIR)

phi = []
for v in vars:
    if ("phi" in v):
        phi.append(v)
    else:
        print(v)


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
    return [ text ]



phi.sort(key=natural_keys)
for p in phi:
    if ("= 1" in p):
        print(p)
# print(phi)
