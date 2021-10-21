# Welcome to HyperQube!

HyperQube is a home-grown tool of Bounded Model Checking for Hyperproperties.


Hyperproperty specifies and reasons about important requirements among multiple traces.
We implement our algorithm for Bounded Model Checking for Hyperproperty as a tool, HyperQube.

HyperQube includes several parts:
- NuSMV model parsing and Boolean encoding of transition relation and specification,
- HyperLTL formula translation,
- QBF encoding of unfolding with bound k using specific semantics,
- QBF solving with QBF solver QuAbs.  

Our code is under MIT license as presented in LICENSE.tex,
while the existing tool, QuAbs, is under AGPL license.  



## HOW TO USE
To run HyperQube, execute ```hyperqube.sh``` with one of the following two scenarios:

   1. BMC with single model:<br/>
	```./hyperqube.sh <model_file_name.smv> <formula_file_name.hq> <k> ```

   2. BMC with multi-model:<br/>
	```./hyperqube.sh <model_1_file_name.smv> <model_2_file_name.smv> <formula_file_name.hq> <k>```

Note that <k> is a natural number specifies the length of unrolling.


To observe the tool outputs, all model checking results with counterexample will output as *_OUTPUT_formatted.cex. The parsed outputs have variables, time stamps, and values neatly presented.


## GET STARTED
You can start using HyperQube in 2 simple steps:		
	
1. First make sure DOCKER is installed from: https://docs.docker.com/get-docker/ . HyperQube will automatically pull the image and execute the scripts to avoid possible hassle of compiling dependencies.
2. Next, clone the repository and step into the repo:
- ```git clone https://github.com/TART-MSU/HyperQube.git```
- ```cd HyperQube```
	
You are now ready to run HyperQube!

We provide two demo examples. To run, execute each of the followings:
- (Example 1: bakery with symmetry:) <br/> ```./hyperqube.sh demo/bakery.smv demo/symmetry 10 pes```
- (Example 2: SNARK with linearizability) <br/> ```./hyperqube.sh demo/SNARK_conc.smv demo/SNARK_seq.smv demo/linearizability 18 pes ```



## Experiments
(all models and formulas are in directory tacas21_cases)

Our evaluations include the following cases,<br/>
- Case #1.1-#1.4: Symmetry in the Bakery Algorithm<br/>
- Case #2.1-#2.2: Linearizability in SNARK Algorithm<br/>
- Case #3.1-#3.2: Non-interference in Typed Multi-threaded Programs<br/>
- Case #4.1-#4.2: Fairness in Non-repudiation Protocols<br/>
- Case #5.1-#5.2: Privacy-Preserving Path Synthesis for Robots<br/>
- Case #6.1: Mutant Synthesis for Mutation Testing<br/>

We also provide all the command lines needed for each experiment in the document *RUN.txt*	
	



## People
Authors:
- [Tzu-Han Hsu](https://tzuhancs.github.io/), Michigan State University.
- [Borzoo Bonakdarpour](http://www.cse.msu.edu/~borzoo/), Michigan State University.
- [César Sánchez](https://software.imdea.org/~cesar/), IMDEA Software Institute.
