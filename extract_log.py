import re

f = open("outlog.txt", "r")
# print(f.read())

text = f.read()

# print(text)

times = re.findall(r"real.*", text)
# print(times)
pynusmv = times[0]
quabs = times[1]

time_pynusmv = pynusmv.replace("real\t0m", "")
time_pynusmv = time_pynusmv.replace("s", "")

## adjust to the docekr transimission time!! (testing locally would obtain better performance.)
time_pynusmv = (float(time_pynusmv)-1.5)
time_pynusmv = str(round(time_pynusmv, 3))

genqbf = re.findall(r"Time elapsed:.*", text)
# print(genqbf[3])
time_genqbf = genqbf[3]
time_genqbf = time_genqbf.replace("Time elapsed: ", "")
time_genqbf = time_genqbf.replace(" secs (done)", "")

time_quabs = quabs.replace("real\t0m", "")
time_quabs = time_quabs.replace("s", "")

time_total = float(time_pynusmv) + float(time_genqbf) + float(time_quabs)
time_total = str(round(time_total, 3))


infos = re.findall(r"\| .*", text)
# print(infos)

# for i in range(0,5):
#     infos[i] = infos[i].replace("|", "")
# print(infos)

QS = infos[0]
K = infos[1]
SEM = infos[2]
MODEL = infos[3]
HLTL = infos[4]


num_states = re.findall(r"Total number of reachable states:.*", text)
# print(num_states)
STATE = num_states[0]
STATE = STATE.replace("Total number of reachable states:", "")


# print("\n----------------------- Summary of Case ------------------------")
print("case model and formula: ")
print(MODEL)
print(HLTL)

print("\nmodel checking info:")
print(QS)
print(SEM)
print("|  num states:" + STATE)
print(K)

print("\nexecution time report: ")
print("|  parseSMV (sec): " + time_pynusmv)
print("|  genqbf   (sec): " + time_genqbf)
print("|  QuABs    (sec): " + time_quabs)
print("\n|  Total time    (sec): " + time_total)

print("---------------------------------------------------------------")
