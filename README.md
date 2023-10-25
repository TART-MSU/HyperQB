# Welcome to HyperQB!!!

HyperQB is a home-grown tool of Bounded Model Checking for Hyperproperties.

Hyperproperty specifies and reasons about important requirements among multiple traces.
We implement our QBF-based algorithm for Bounded Model Checking for Hyperproperty as a tool, HyperQB.

HyperQB includes several parts:
- NuSMV model parsing and Boolean encoding of transition relation and specification,
- HyperLTL formula translation,
- QBF encoding of unfolding with bound k using specific semantics,
- QBF solving with QBF solver QuAbs.  

Our code is under MIT license as presented in LICENSE.tex,
while the existing tool, QuAbs, is under AGPL license.  


## Get Started
You can start using HyperQB in 2 simple steps:
1. First install [docker](https://docs.docker.com/get-docker/). 
HyperQB will automatically pull the image and execute the scripts to avoid possible hassle of compiling dependencies!

2. Next, clone the repository and step into the repo:
- ```git clone https://github.com/TART-MSU/HyperQB.git```
- ```cd HyperQB```

You are now ready to run HyperQB!:D

## How to Use
To run HyperQB, execute ```hyperqb.sh``` with the following inputs:
- `<list of models>` written in NuSMV format (as .smv files),
- `<formula>` written in the grammar described in Sec. 4 (as a .hq file),
- `<k>` a natural number 0, specifying the unrolling bound,
- `<sem>` the semantics, which can be -pes, -opt, -hpes or -hopt, and
- `<mode>` to say performing classic BMC (i.e., negating the formula) or not, which can be -bughunt or -find (we use the former as default value).  
    
RUN HyperQB in the following format:
    ```./hyperqube <list of models> <formula> <k> <sem> <mode>```

Demo Examples:
1. [demo 1: run bakery algorithm with symmetry property]
```./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 -pes -bughunt```

2. [dem0 2: run SNARK algorithm with linearizability propoerty]
```./hyperqb.sh demo/snark_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -bughunt```    


## Experiments
(all models and formulas are in directory benchmarks)

Our evaluations include the following cases,<br/>
- Case #1.1-#1.4: Symmetry in the Bakery Algorithm<br/>
- Case #2.1-#2.2: Linearizability in SNARK Algorithm<br/>
- Case #3.1-#3.2: Non-interference in Typed Multi-threaded Programs<br/>
- Case #4.1-#4.2: Fairness in Non-repudiation Protocols<br/>
- Case #5.1-#5.2: Privacy-Preserving Path Synthesis for Robots<br/>
- Case #6.1: Mutant Synthesis for Mutation Testing<br/>
- Case #7.1: Co-termination of multiple programs<br/>
- Case #6.1: Mutant Synthesis for Mutation Testing<br/>
- Case #6.1: Mutant Synthesis for Mutation Testing<br/>
- Case #6.1: Mutant Synthesis for Mutation Testing<br/>
- Case #6.1: Mutant Synthesis for Mutation Testing<br/>
- Case #7.1: Co-termination<br/>
- Case #8.1: Deniability<br/>
- Case #9.1 - #9.3: Intransitive Non-interference<br/>
- Case #10.1 - #10.2: TINI and TSNI
- Case #11.1: K-safety
- Case #12.1: MapSynth
- Case #12.2: MapSynth
- Case #13.1: TeamLTL
- Case #13.2: TeamLTL
- Case #14.1: Non-deterministic init
- Case #14.2: Non-deterministic trans


## One-click Scripts
We provide several convenient scripts for executing exeperiments. 
- ```./run_demos.sh``` to run demo examples
- ```./run_table <{case#, -all}> ``` to run either a specific case, or all cases. (additional flags -AH and -QAH provides comparisons with other model checkers.)



## People
Authors:
- [Tzu-Han Hsu](https://tzuhancs.github.io/), Michigan State University.
- [Borzoo Bonakdarpour](http://www.cse.msu.edu/~borzoo/), Michigan State University.
- [César Sánchez](https://software.imdea.org/~cesar/), IMDEA Software Institute.

## Publication
- [Bounded Model Checking for Hyperproperties (TACAS'21)](https://link.springer.com/content/pdf/10.1007/978-3-030-72016-2_6.pdf)

