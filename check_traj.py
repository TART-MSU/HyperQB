f = open("OUTPUT_formatted.cex", "r")
for x in f:
    if (('+' in x) or ('in_' in x)  or ('obs_' in x)):
        print(x)
