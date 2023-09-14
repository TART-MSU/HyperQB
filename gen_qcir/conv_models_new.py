import sys
import re

# This file IS NOT COMPLETE! It DOES NOT DO ANYTHING YET
print("This file doesn't fully work yet!")

# TIL python does not contain a simple sign function
# https://stackoverflow.com/questions/1986152/why-doesnt-python-have-a-sign-function
def sign(a):
	return (a > 0) - (a < 0)

# "# <number> : <name>_<trace>[<step>]
VAR_NAME_REGEX = r'# (\d+) : ([a-zA-Z_]+[a-zA-Z0-9]*)_([AB])\[([01])\]'
HELPER_NAME_REGEX = r"# (\d+) : ([a-zA-Z_]+[a-zA-Z0-9]*_[AB]'?)"

# Renumbers the model to adjust all variables by new_base.
# Model should be a file named infile stored in QCIR format.
def renumber_model(infile, new_base):
	for line in infile:
		line = line.strip("\n")
		if "#" in line:
			# check if this comment is a variable name line
			match = re.match(VAR_NAME_REGEX, line)
			if match:
				new_var_id = int(match.group(1)) + new_base
				var_name = match.group(2)
				var_trace = match.group(3)
				helper_step = "'" if match.group(4) == "1" else ""
				yield f"# {new_var_id} : {var_name}_{var_trace}{helper_step}"
			else:
				# do not change comments that do not correspond to variables
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

# Shared model processing code below into own function
# Takes in a model file and a variable adjustment value
# Returns a map of helper names to variable ids and 
def process_model(model_file, new_base):
	max_model_var = 0
	model_helpers = {}
	with open(model_file) as f:
		for new_line in renumber_model(f, new_base):
			if new_line and new_line[0].isdigit():
				gate_id = int(new_line.split("=")[0])
				max_model_var = max(gate_id, max_model_var)
			# Add map entries for helper names to variable ids
			match = re.match(HELPER_NAME_REGEX, new_line)
			if match:
				model_helpers[match.group(2)] = int(match.group(1))
			print(new_line)
	print(model_helpers)
	return max_model_var, model_helpers

if __name__ == "__main__":
	model_a = sys.argv[1]
	model_b = sys.argv[2]
	num_steps = int(sys.argv[3])
	
	# Iterate over lines and extracts helper variable info
	max_var_a, model_a_helpers = process_model(model_a, 0)
	
	# Note: max_model_a is the highest gate id of model a
	# Use this as an offset for model_b to ensure no conflict
	# Updates variable lines and collects helper variable information
	max_var_b, model_b_helpers = process_model(model_b, max_var_a)
	
	# Next: Output unrolling steps for mapping each model's real variables to
	# the new helper variable ids
	
	#TODO: Multiple time steps
	step = 0
	for model_helpers in [model_a_helpers, model_b_helpers]:
		for var in model_helpers:
			# Map var to comparable var for model
			# How to: Map a[0] to a, a[1] to a'?
			var_step = f"[{step+1}]" if var.endswith("'") else f"[{step}]"
			actual_var_name = var.strip("'") + var_step
			
			print(var, actual_var_name)

