import sys
import pynusmv
import string
import re
import os.path
import os
from os import path
from pynusmv.model 			import *
from pynusmv.fsm 			import BddTrans
from pynusmv.fsm 			import BddEnc
from pynusmv.fsm 			import BddFsm
from pynusmv.fsm 			import SymbTable
from pynusmv 				import prop
from pynusmv.mc 			import check_ctl_spec
from pynusmv.mc 			import check_ltl_spec
from pynusmv.bmc 			import *
from pynusmv 				import node
from pynusmv 				import glob
from pynusmv.glob 			import *
from pynusmv.bmc 			import glob as bmcGlob
from pynusmv.node          	import *
from pynusmv.parser			import parse_ltl_spec
from pynusmv.bmc.utils      import DumpType , BmcModel
from pynusmv.be.expression  import Be
from pynusmv.bmc.glob       import go_bmc, bmc_exit, master_be_fsm, BmcSupport
from pynusmv.bmc.invarspec  import *
from pynusmv.utils          import StdioFile
from pynusmv.wff 			import Wff
from pynusmv.be.encoder 	import BeEnc
from pynusmv.be.fsm        	import BeFsm
from pynusmv.prop 			import PropDb
from pynusmv.bmc           	import utils as bmcutils
from pynusmv.be.manager		import BeManager
from pynusmv.sexp.fsm  		import *



#########################
#  little helpe methods #
#########################

## Logical operators ##
NOT = "~"
AND = "/\\"
OR = "\\/"
IMPLIES = "->"
IFF = "<->"
NEXT = "'"



#################
#  remove stuff #
#################

## TBD: can we have the name includes dot in genQBF? ##
def remove_dot(s):
	s =  s.replace(".", "-")
	return s
########################################################

############################
#  Check type of variables #
############################

## value is boolean ##
def isBoolean(key):
	return ((key == 'TRUE') or (key == 'FALSE'))

## if values is digits ##
def isNumber(key):
	return key.isdigit()


###################################
#  Initial Condition Construction #
###################################

## assign a boolean value to a variable key ##
def assignBool(key, dict):
	var_name = remove_dot(key)
	if (dict[key] == 'TRUE'):
		return var_name
	else:
		return NOT+var_name

## assign a digit value to a variable key ##
def assignBin(key, dict, bitblasting_dict):
	bits = []
	var_name = remove_dot(key)

	# num_bits = bitblasting_dict[key]
	num_bits = bitblasting_dict[key]
	binary = format(int(dict[key]), '0'+str(num_bits)+'b').replace("0b", "")
	counter = num_bits-1
	for b in binary:
		if (b == '1'):
			bits.append(var_name+"_"+str(counter))
		else:
			bits.append(NOT+var_name+"_"+str(counter))
		counter-=1
	return conjunct(bits)

## assign a bool variable value to a variable key ##
def assignVal(key, dict):
	var_name = remove_dot(key)
	return var_name+IFF+dict[key]



#####################################
#  Transition Relation Construction #
#####################################


# print out all atomic propositions in dict
## All AP values in the current state ##
def getDictAP(dict, bitblasting_dict):
	all_ap = []
	for ap in dict:
		value = dict[ap]
		if(isBoolean(value)):
			all_ap.append(assignBool(ap, dict))
		elif(isNumber(value)):
			all_ap.append(assignBin(ap, dict, bitblasting_dict))
		else:
			name = remove_dot(ap)
			all_ap.append(name+IFF+dict[ap])
	return conjunct(all_ap)

## All AP values in the post state ##
def getNextDictAP(dict, bitblasting_dict):
	all_ap = []
	for ap in dict:
		value = dict[ap]
		if(isBoolean(value)):
			all_ap.append(assignNextBool(ap, dict))
		elif(isNumber(value)):
			all_ap.append(assignNextBin(ap, dict, bitblasting_dict))
		else:
			all_ap.append(assignNextVal(ap, dict))
	return conjunct(all_ap)

## All AP values in the post state ##
def assignNextBin(key, dict, bitblasting_dict):
	bits = []
	name = remove_dot(key)
	num_bits = bitblasting_dict[key]
	binary = format(int(dict[key]),'0'+str(num_bits)+'b').replace("0b", "")
	counter = num_bits-1
	for b in binary:
		if (b == '1'):
			bits.append(name+"_"+str(counter)+NEXT)
		else:
			bits.append(NOT+name+"_"+str(counter)+NEXT)
		counter-=1
	return conjunct(bits)

def assignNextBool(key, dict):
	var_name = remove_dot(key)
	if (dict[key] == 'TRUE'):
		return var_name+NEXT
	else:
		return NOT+var_name+NEXT

def assignNextVal(key, dict):
	var_name = remove_dot(key)
	return var_name+NEXT+IFF+dict[key]+NEXT


####################################
#  Logical Expression Construction #
####################################
## conjunct of all element in a list ##
def conjunct(list):
	if(len(list) == 1):
		return list[0]
	output = ""
	for i in range(len(list)-1):
		output += list[i] + AND
	return "(" + output + list[len(list)-1] + ")"

## disjunct of all element in a list ##
def disjunct(list):
	if(len(list) == 1):
		return list[0]
	output = ""
	for i in range(len(list)-1):
		output += list[i] + OR
	return "(" + output + list[len(list)-1] + ")"

def conjunct_trans(list):
	output = ""
	for i in range(len(list)-1):
		output += list[i] + AND + "\n"
	return output + list[len(list)-1]

def trans(pre, post):
	return "(("+pre+")"+IMPLIES+"("+post+"))"
	# return "(~("+pre+")"+OR+"("+post+"))"



###############################
#  HLTL Property Construction #
###############################
### TODO
def model_sus(str, sus):
	# str = re.sub('\d', '\d', str)
	# for ele in str:
	#     if ele.isdigit():
	#         str = str.replace(ele, ele+sus)
	return str

# bin_eq: two variables matches each bit
def binary_eq(var_l, var_r,  num_bits):
	# num_bits = bitblasting_dict[var_l[0]]
	result = []
	for i in range(num_bits):
		left = var_l[0] + "_"+str(i) +var_l[1]
		right = var_r[0] + "_"+str(i) +var_r[1]
		# print(left + " <-> " + right)
		# result.append("("+left + IFF + right+")")
		result.append("((~"+left + OR + right+")/\\(~" + right + OR + left + "))");
	return conjunct(result)

def binary_assign(var, num, bitblasting_dict):
	num_bits = bitblasting_dict[var[0]]
	binary = format(num, '0'+str(num_bits)+'b').replace("0b", "")
	result = []
	counter = num_bits-1
	for b in binary:
		if (b == '1'):
			result.append(var[0] + "_"+str(counter) +var[1])
		else:
			result.append(NOT+var[0] + "_"+str(counter) +var[1])
		counter-=1
	return conjunct(result)

##################
#  Error message #
##################
def error_exit(msg):
	print("\n(!) HyperQB error: ", msg)
	sys.exit()



#########################
#      Parse Model      #
#########################
def main_model_parse(smv_file_name, bitblasting_dict, parsed_madel_file_I_name, parsed_madel_file_R_name, PARSE_INDEX):
	# pynusmv.init.init_nusmv()
	pynusmv.glob.load_from_file(smv_file_name)
	pynusmv.glob.compute_model()
	fsm = pynusmv.glob.prop_database().master.bddFsm
	enc = fsm.bddEnc
	# ### DEBUG
	# print("====================================")
	# print("FSM Model info:")
	# print("\n============ Parse M1 ============")
	# state_variables = list(enc.stateVars)
	# print("all state variables: ", state_variables)
	# num_states = fsm.count_states(fsm.reachable_states)
	# print("total number of reachable states: ", num_states)
	# inputs = list(enc.inputsVars)
	# print("input variables", inputs)
	# atomics = list(enc.definedVars)
	# print("atomic propositions", atomics)

	state_variables = list(enc.stateVars)
	num_states = fsm.count_states(fsm.reachable_states)
	############################
	#  bit-blasting dictionary #
	############################
	# bitblasting_dict = {}
	# build bitblasting_dict
	smv_file = open(smv_file_name, 'r')
	lines = smv_file.readlines()
	for line in lines:
		# print(line)
		if(re.findall("\.\.", line)):
			# line = line.split("--") #remove comments
			line = line.split("--", 1)[0].replace("\t","") #remove tail comments
			# print(line.isspace())
			line = line.strip()
			if (line): # if it's not empty
				key = re.findall(".*:", line)[0].replace(":","")
				num = re.findall("[\d]*;", line)[0].replace(";","")
				# print(line)
				# print(key)
				# print(num)
				value = int(num).bit_length()
				# bitblasting_dict[key] = value
				for var in state_variables:
					if(key.replace(" ", "") in var):
						bitblasting_dict[var] = value
		# TODO: new data type: unsigned word
		elif(re.findall("unsigned word", line)):
			line = line.split("--", 1)[0].replace("\t","") #remove comments
			# print(line)
			if (line): # if it's not empty
				key = re.split("unsigned", line)[0].replace(":","")
				num_bits = re.split("word", line)[1].replace("[","").replace("]","")
				# print(key)
				# print(num_bits)
				for var in state_variables:
					if(key.replace(" ", "") in var):
						M1_bitblasting_dict[var] = int(num_bits.replace("\n","").replace(";",""))

	# print("Dictionary for bit-blasting: ")
	# print(bitblasting_dict)




	###################################
	#  Initial Condition Construction #
	###################################
	# def gen_I():
	init_states = []
	# print(fsm.pick_all_states(fsm.init))
	for state in fsm.pick_all_states(fsm.init):
		init_conditions = []
		dictionary = state.get_str_values()
		# print(dictionary)
		for ap in dictionary:
			value = dictionary[ap]
			# print(value)
			if(isBoolean(value)):
				init_conditions.append(assignBool(ap, dictionary,))
			elif(isNumber(value)):
				init_conditions.append(assignBin(ap, dictionary, bitblasting_dict))
			# TODO: new data type: unsigned word
			elif("0ud" in value):
				dictionary[ap] = re.split("_", value)[1]
				init_conditions.append(assignBin(ap, dictionary, bitblasting_dict))
			else:
				init_conditions.append(assignVal(ap, dictionary, bitblasting_dict))
		init_states.append(conjunct(init_conditions))
	##  write to I_bool file
	I_bool = open(parsed_madel_file_I_name, "w")
	I_bool.write(disjunct(init_states))
	I_bool.close()


	#####################################
	#  Transition Relation Construction #
	#####################################
	### try to fix oveflow issue
	# def gen_R():
	counter = 0;
	all_transitions = []
	# print(fsm.pick_all_states(fsm.reachable_states))
	R_bool = open(parsed_madel_file_R_name, "w")
	for state in fsm.pick_all_states(fsm.reachable_states):
		if (counter!=0):
			R_bool.write(AND + "\n")

		transitions = []
		curr = state.get_str_values()
		post_list = [] # list of all next states
		for post_state in fsm.pick_all_states(fsm.post(state)):
			post = post_state.get_str_values()
			# print(post)
			post_list.append(getNextDictAP(post,  bitblasting_dict))
		transitions.append(trans(getDictAP(curr, bitblasting_dict), disjunct(post_list)))
		R_bool.write(trans(getDictAP(curr,  bitblasting_dict), disjunct(post_list)))
		counter = counter+1
		# R_bool.write(conjunct_trans(transitions))

	# R_bool.write("TRUE"); ##DUMMY
	# R_bool.replace(AND + "\n" + TRUE, "")

	##  write to R_bool file
	# R_bool = open("test_R.bool", "w")
	# R_bool.write(conjunct_trans(all_transitions))
	# print(str(counter), ", ")
	global SUCCESS_OUT
	SUCCESS_OUT += "|M" + str(PARSE_INDEX) + "|=" + str(counter) + "	"
	R_bool.close()



##################################
#  HyperLTL Formula Construction #
##################################
def main_formula_construct(formula_file_name, dictionaries, translated_formula_file_name, QS_file_name, To_Negate_formula):
	text = ""
	file = open(formula_file_name, 'r')
	Lines = file.readlines()
	for line in Lines:
		if ("#" not in line):
			text += line
	# print(formula_file_name)

	### read quantifier selection, store in QS.hq
	Quants=""
	for char in text:
		if (char == 'f'):
			# print("forall")
			if(To_Negate_formula):
				Quants+="E"
			else:
				Quants+="A"
		elif(char == 'e'):
			# print("exists")
			if(To_Negate_formula):
				Quants+="A"
			else:
				Quants+="E"
		elif(char == '('):
			break;
	global PARSE_INDEX
	# error check if num of models and quants conform
	if (len(Quants) != (PARSE_INDEX)):
		# print(len(Quants))
		# print((PARSE_INDEX))
		error_exit("number of models and number of quantifiers must match.")
	Quants="QS="+Quants
	# print(Quants)
	QS = open(QS_file_name, "w")
	QS.write(Quants)
	QS.close()




	# get all tid after all the quantifiers, subsitute all [...] into _...
	tid_list = re.findall("[a-z, A-Z]\.", text)
	Mindex = ["_A", "_B", "_C", "_D", "_E", "_F", "_G", "_H", "_I", "_J", "_K", "_L", "_M", "_N", "_O", "_P", "_Q", "_R", "_S", "_T", "_U", "_V", "_W", "_X", "_Y", "_Z"]
	for i in range(len(tid_list)):
		tid = tid_list[i].replace(".","")
		text = re.sub("\["+tid+"\]", Mindex[i], text)

	## make '!' as "~" for genqbf
	text = text.replace("!","~")

	## parse iff operations
	iff_ops = re.findall("\(.*?<->.*?\)", text)
	for op in iff_ops:
		convert_iff = ""
		op = op.replace("(", "")
		op = op.replace(")", "")
		op = op.replace(" ", "")
		vars = op.split("<->")
		# print(op)
		convert_iff = "((~" + vars[0] + OR + vars[1] + ")/\\" + "(~" + vars[1] + OR + vars[0] + "))";
		# print(convert_iff)
		text = text.replace(op, convert_iff)
		# print(text)

	text = text.replace(" ","")

	## parse arithmetic operations
	arith_ops = re.findall("\*.*?\*", text)


	for op in arith_ops:
		op = op.replace(" ", "")
		op = op.replace("*", "")
		if("!="in op):
			vars = op.split("!=")
		else:
			vars = op.split("=")
		var_l = str(vars[0]).rsplit('_', 1)
		var_r = str(vars[1]).rsplit('_', 1)

		blasted = ""
		if (var_l[0].isdigit() and var_r[0].isdigit()):
			error_exit("arithmetic operation is not correctly constructed.")

		# case 1: (num)=(var)
		elif (var_l[0].isdigit()):
			try:
				var_r[1] = "_"+var_r[1]
				blasted = binary_assign(var_r, int(var_l[0]), DICTIONARIES[int(Mindex.index(var_r[1]))])
			except KeyError as ke:
			    	error_exit("incorrect arithmetic assignment. please check:" + str(ke))

		# case 2: (var)=(num)
		elif (var_r[0].isdigit()):
			try:
				var_l[1] = "_"+var_l[1]
				blasted = binary_assign(var_l, int(var_r[0]), DICTIONARIES[int(Mindex.index(var_l[1]))])
			except KeyError as ke:
			    	error_exit("incorrect arithmetic assignment. please check:" + str(ke))

		# case 3: (var)=(var)
		else:
			try:
				var_r[1] = "_"+var_r[1]
				var_l[1] = "_"+var_l[1]
				dict_l=DICTIONARIES[int(Mindex.index(var_l[1]))]
				num_bits_left=dict_l[var_l[0]]
				dict_r=DICTIONARIES[int(Mindex.index(var_r[1]))]
				num_bits_right=dict_r[var_r[0]]
			except KeyError as ke:
			    	# print('Key Not Found in Employee Dictionary:', ke)
					error_exit("incorrect arithmetic assignment. please check:"+ str(ke))


			if(num_bits_left != num_bits_right):
				error_exit("arithmetic operation requires two variables with same number of bits in binary representations.")
			else:
				if("!="in op):
					blasted = "~"+binary_eq(var_l, var_r, num_bits_left)
				else:
					blasted = binary_eq(var_l, var_r, num_bits_left)

		text = text.replace(op, blasted)




	# clea up quantifiers
	text = text.replace("forall","")
	text = text.replace("exists","")
	text = re.sub("[a-z, A-Z]\.", "", text)
	# clean up arith operations
	text = text.replace("*","")
	text = remove_dot(text)
	# text = text.replace(" ","")

	if(To_Negate_formula):
		text= "~("+ text + ")"

	### finally, write to P file
	P_bool = open(translated_formula_file_name , "w")
	text = text.replace(' ', '').replace('\t', '').replace('\n', '')
	P_bool.write(text)
	P_bool.close()




#################
#      Main	    #
#################
ARGS=(sys.argv)
# print("ARGS: ", ARGS)
OUTPUT_LOCATION=ARGS[1]
PARSE_INDEX=0
DICTIONARIES = []
SUCCESS_OUT=""
smv_file_name=""

# get the mode first
FLAG=""
if ("-find" in ARGS):
	FLAG="-find"
else:
	FLAG="-bughunt"

# print("\nparsing models... ")
for i in range(0, len(ARGS)):
	# print(ARGS[i])
	if (".smv" in str(ARGS[i])):
		PARSE_INDEX = PARSE_INDEX + 1
		parsed_madel_file_I_name = OUTPUT_LOCATION + '/I_'+ str(PARSE_INDEX)+'.bool'
		parsed_madel_file_R_name = OUTPUT_LOCATION + '/R_'+ str(PARSE_INDEX)+'.bool'

		### if model is repeating
		if (smv_file_name == ARGS[i]):
			prev_I = OUTPUT_LOCATION + '/I_'+ str(PARSE_INDEX-1)+'.bool'
			prev_R = OUTPUT_LOCATION + '/R_'+ str(PARSE_INDEX-1)+'.bool'
			os.system("cp" + " " + prev_I + " " + parsed_madel_file_I_name)
			os.system("cp" + " " + prev_R + " " + parsed_madel_file_R_name)
			DICTIONARIES.append(bitblasting_dict)
		else:
			### start a new parsing
			smv_file_name = ARGS[i]
			pynusmv.init.init_nusmv()
			bitblasting_dict = {}
			main_model_parse(smv_file_name, bitblasting_dict, parsed_madel_file_I_name, parsed_madel_file_R_name, PARSE_INDEX)
			DICTIONARIES.append(bitblasting_dict)
			pynusmv.init.deinit_nusmv() # release pynusmv instance for next parsing


	elif ((".hq" in str(ARGS[i])) and (str(ARGS[i]) != "P.hq")):
		# print("\nparsing HyperLTL formula... ")
		formula_file_name = ARGS[i]
		translated_formula_file_name = OUTPUT_LOCATION + '/P.hq'
		QS_file_name = OUTPUT_LOCATION + '/QS'
		# print(DICTIONARIES)
		# FLAG = sys.argv[4]
		To_Negate_formula=(FLAG=="-bughunt")
		# print("\ntranslating HyperLTL formula...")
		# main_formula_construct(formula_file_name, M1_bitblasting_dict, M2_bitblasting_dict, translated_formula_file_name)
		# global DICTIONARIES
		main_formula_construct(formula_file_name, DICTIONARIES, translated_formula_file_name, QS_file_name, To_Negate_formula)
		break

print(SUCCESS_OUT) # parsing successfully completed. return.





# print("\ntranslating SMV model(s)...")
# pynusmv.init.init_nusmv()
# M1_bitblasting_dict = {}
# main_model_parse(M1_smv_file_name, M1_bitblasting_dict, M1_parsed_madel_file_I_name, M1_parsed_madel_file_R_name)
# # RESET PUNUSMV and init it again.
# pynusmv.init.deinit_nusmv()
# pynusmv.init.init_nusmv()
# M2_bitblasting_dict = {}
# main_model_parse(M2_smv_file_name, M2_bitblasting_dict, M2_parsed_madel_file_I_name, M2_parsed_madel_file_R_name)


# print("\ntranslating HyperLTL formula...")
# main_formula_construct(formula_file_name, M1_bitblasting_dict, M2_bitblasting_dict, translated_formula_file_name)
