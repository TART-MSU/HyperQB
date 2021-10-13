# Welcome to HyperQube!

HyperQube is a home-grown tool of Bounded Model Checking for Hyperproperties. 


Hyperproperty specifies and reasons about important requirements among multiple traces. 
We implement our algorithm for Bounded Model Checking for Hyperproperty as a tool, HyperQube.

HyperQube includes three parts: 
	1. NuSMV model parsing and Boolean encoding of transition relation and specification,
	2. HyperLTL formula translation 
	2. QBF encoding of unfolding with bound k using specific semantics, 
	3. QBF solving with QBF solver QuAbs.  
	
Our code is under MIT license as presented in License.tex, 
while the existing tool, QuAbs, is under AGPL license.  
 


## HOW TO USE 
We have our source code all compiled as executable files which are placed in folder exec/

To run HyperQube, execute hyperqube.sh with two scenarios:

   1. BMC with single model:
	$ hyperqube.sh <model_file_name.smv> <formula_file_name.hq> <k>
   2. BMC with multi-model:
	$ hyperqube.sh <model_1_file_name.smv> <model_2_file_name.smv> <formula_file_name.hq> <k>

Note that <k> is a natural number specifies the length of unrolling.


To observe the output, 
- All model checking with counter example will output as *_OUTPUT_formatted.cex files in the directory: HyperQube_output/
- The parsed outputs have variables, time stamps, and values neatly presented.



### CASES SET from TACAS 2021 paper ###
(In directory tacas21_cases)

Our evaluations include the following cases,
	Case #0.1-#1.4:	Symmetry in the Bakery Algorithm
	Case #2.1-#2.2:	Linearizability in SNARK Algorithm
	Case #3.1-#3.2:	Non-interference in Typed Multi-threaded Programs
	Case #4.1-#4.2:	Fairness in Non-repudiation Protocols
	Case #5.1-#5.2:	Privacy-Preserving Path Synthesis for Robots
	Case #6.1:		Mutant Synthesis for Mutation Testing

In out paper, the detailed case studies decription can be found in section 5.
Outputs evaluations and BMC analysis can be found in section 6. 

To execute the experiments, execute corresponding run_<case_name>.sh script.


