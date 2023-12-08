# Welcome to HyperQB!!!
HyperQB is a home-grown tool of Bounded Model Checking for Hyperproperties with QBF-based algorithm.

HyperQB includes several parts:
- NuSMV model parsing and Boolean encoding of transition relation,
- HyperLTL formula translation,
- QBF encoding of unfolding with bound k using bounded semantics,
- QBF solving with QBF solver (currently QuAbs).  

## Licence
Our code is under MIT license as presented in LICENSE.tex,
while the existing tool, QuAbs, is under AGPL license.  

## Get Started
You can start using HyperQB in 2 simple steps:
1. First install [docker](https://docs.docker.com/get-docker/). 
HyperQB will automatically pull the image and execute the scripts to avoid possible hassle of compiling dependencies.

2. Next, clone the repository and step into the repo:<br/>
- ```git clone https://github.com/TART-MSU/HyperQB.git```
- ```cd HyperQB```

You are now ready to run HyperQB!:D

## First Demo
To check if HyperQB runs correctly, simply execute the following lines (in HyperQB/): <br/>
```sudo ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt```<br/>
```sudo ./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find```<br/>
```sudo ./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug```<br/>

(The script ```run_demo.sh``` contains more small demo examples)


## How to Use
To run HyperQB, execute ```hyperqb.sh``` with the following inputs:
- `<list of models>` written in NuSMV format (as .smv files),
- `<formula>` written in the grammar described in Sec. 4 (as a .hq file),
- `<k>` a natural number 0, specifying the unrolling bound,
- `<sem>` the semantics, which can be -pes, -opt, -hpes or -hopt, and
- `<mode>` the mode of performing classic BMC (negating the formula) or not (original formula), which can be `-bughunt` or `-find` (we use `-bughunt` as default value).  
    
In general, run HyperQB in the following format:<br/>
```./hyperqube <list of models> <formula> <k> <sem> <mode>```

Examples from our benchmarks:
1. [demo 1: run bakery algorithm with symmetry property]<br/>
```./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 -pes -bughunt```

2. [dem0 2: run SNARK algorithm with linearizability propoerty]<br/>
```./hyperqb.sh demo/snark_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -bughunt```    


## Experiments
All models and formulas are located in directory `benchmarks/`.

We provide a one-click script ```run_benchmarks.sh``` for running our benchmarks.
- ```./run_benchmarks.sh <case#>``` runs a specific case,
- ```./run_benchmarks.sh -all``` runs all cases

For example: 
```./run_benchmarks.sh 0.3 ``` runs case #0.3 
```./run_benchmarks.sh 9.2 ``` runs case #9.2 

Our evaluations include the following cases:<br/>
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

## Additional Information 

### NuSMV Models 
HyperQB supports most features/keywords of `NuSMV` for `VAR`, `ASSIGN`, `DEFINE`, etc. Currently, we do not support the keyword “`process`” for asynchronicity. However, this limitation can be easily resolved by having nondeterministic `next()` in `TRANS` (e.g., `next(proc_PC) := {0,1}`; for a nondeterministic movement of a process).

### HyperLTL Formulas 
We do not include `SPEC` keyword in `NuSMV` as the formula specification, since it does not cover HyperLTL. Instead, HyperQB takes formulas written in our own grammar. 

## People
- [Tzu-Han Hsu](https://tzuhancs.github.io/), Michigan State University.
- [Borzoo Bonakdarpour](http://www.cse.msu.edu/~borzoo/), Michigan State University.
- [César Sánchez](https://software.imdea.org/~cesar/), IMDEA Software Institute.

## Publication
- [Bounded Model Checking for Hyperproperties (TACAS'21)](https://link.springer.com/content/pdf/10.1007/978-3-030-72016-2_6.pdf)


