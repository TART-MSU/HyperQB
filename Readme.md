# Welcome to HyperQube!

HyperQube is a home-grown tool of Bounded Model Checking for Hyperproperties. 


Hyperproperty specifies and reasons about important requirements among multiple traces. 
We implement our algorithm for Bounded Model Checking for Hyperproperty as a tool, HyperQube.

HyperQube includes the following parts: <br/>
	1. NuSMV model parsing and Boolean encoding of transition relation and specification,<br/>
	2. HyperLTL formula translation, <br/>
	2. QBF encoding of unfolding with bound k using specific semantics, <br/>
	3. QBF solving with QBF solver QuAbs.  
	
Our code is under MIT license as presented in License.tex, 
while the existing tool, QuAbs, is under AGPL license.  
 


## HOW TO USE 
We have our source code all compiled as executable files which are placed in folder exec/

To run HyperQube, execute hyperqube.sh with two scenarios:

   1. BMC with single model:<br/>
	$ hyperqube.sh <model_file_name.smv> <formula_file_name.hq> <k> <br/>

   2. BMC with multi-model:<br/>
	$ hyperqube.sh <model_1_file_name.smv> <model_2_file_name.smv> <formula_file_name.hq> <k> <br/>

Note that <k> is a natural number specifies the length of unrolling.


To observe the tool outputs, all model checking with counter example will output as *_OUTPUT_formatted.cex file. The parsed outputs have variables, time stamps, and values neatly presented.

	
## GET STARTED
We provide two examples. To run, execute the following: 
- git clone https://github.com/TART-MSU/HyperQube.git
- cd HyperQube 
- ./hyperqube.sh bakery.smv symmetry 10 pes
- ./hyperqube.sh SNARK_conc.smv  SNARK_seq.smv linearizability 18 pes 


### CASES SET from TACAS 2021 paper ###
(In directory tacas21_cases)

Our evaluations include the following cases,<br/>
	Case #0.1-#1.4:	Symmetry in the Bakery Algorithm<br/>
	Case #2.1-#2.2:	Linearizability in SNARK Algorithm<br/>
	Case #3.1-#3.2:	Non-interference in Typed Multi-threaded Programs<br/>
	Case #4.1-#4.2:	Fairness in Non-repudiation Protocols<br/>
	Case #5.1-#5.2:	Privacy-Preserving Path Synthesis for Robots<br/>
	Case #6.1:	Mutant Synthesis for Mutation Testing<br/>

To execute the experiments, execute corresponding blocks in tacas21_cases.sh scripts.


