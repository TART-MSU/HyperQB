# This file IS NOT COMPLETE! It DOES NOT DO ANYTHING YET
print("This file doesn't fully work yet!")

# TIL python does not contain a simple sign function
# https://stackoverflow.com/questions/1986152/why-doesnt-python-have-a-sign-function
def sign(a):
	return (a > 0) - (a < 0)

# Renumbers the model to adjust all variables by new_base.
# Model should be a file named infile stored in QCIR format.
def renumber_model(infile, new_base):
	for line in infile:
		line = line.strip("\n")
		if "#" in line:
			# don't change comments
			yield line 
			continue
		elif line.startswith(("exists","forall","output")):
			# instruction_type(gate1[, gate2, ...])
			line_prefix = line[:line.find("(")+1]
		elif line and line[0].isdigit():
			# Line is assumed to be of following form:
			# gate_id = gate_type(gate1, gate2[, ...])
			gate_id, gate_actual = line.split("=")
			gate_id = str(int(gate_id) + new_base)
			gate_actual = gate_actual[:gate_actual.find("(")+1]
			line_prefix = gate_id + " =" + gate_actual
		else:
			# Default to not modifying unknown lines
			# print("UNK: "+line)
			yield line
			continue
		
		# This is shared code for any line with gate numbers
		# get just the gate numbers only
		gates = line[line.find("(")+1:line.find(")")].split(",")
		# Convert to integers and offset by new_base (make sure to adjust for '-')
		gates = [sign(int(g))*(abs(int(g)) + new_base) for g in gates]
		# Convert back to strings and combine into new line
		new_line = line_prefix+",".join([str(g) for g in gates])+")"
		yield new_line

with open("mini-model.qcir") as f:
	for l in renumber_model(f, 1):
		print(l)
