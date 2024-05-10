# README for ATVA24 Tool Papaer Artifact Evaluation
Title:    HyperQB: A QBF-Based Bounded Model Checker for Hyperproperties <br/>
Authors:  Tzu-Han Hsu, Borzoo Bonakdarpour, César Sánchez


## Hyperlink to the artifact
DOI: <br/>
Zenodo link: <br/>
checksum: <br/>


## Environment requirements for the artifact
- To provide the best evaluating experience for the reviewers, 
we have tested this artifact on the provided VM *Artifact_VM_Ubuntu_22.04.ova*

- Each binary executable is already pre-compiled by us on the VM!:D
(i.e., no further extra compilation needed from the reviewers).  

- Our artifact is partially using Docker image to 
reduce the reviewer's burden on the dependencies installation.
Note:   The core technical parts of HyperQB BMC algorithm
        (i.e., encoding, unrolling, QBF-solving, etc.),
        are still self-contained in this artifact. 


## Structure 
Our artifact is structured as follows:
- `benchmarks/` contains benchmarks we used for evaluation.
- `demo/` contains small instances as demonstrative examples.
- `exec/` contains all pre-compiled executable (for different platforms), including AutoHyper (AH), and AutoHyperQ (AHQ), which are the tools we did comparison with on the benchmarks.
- `src/` contains the source code of HyperQB. 
when running HyperQB the temp containers `build_today` and `build_cex` will also be created.

The goal of this artifact is to produce results of Table 3 in the paper, including:
1. Results produced by HyperQB (HQB).
2. Comparisons of HyperQB with [24], AutoHyper (AH)[2], and AutoHyperQ (AHQ) [3].

## Remarks on this Artifact
1. We notice that while running our experiments on the ATVA VM, the overall runtime *increase* a lot (in comparison to our reported runtime in Table 3, obtained from running on a MacBook M1 locally).
2. We notice that in several cases, AH/AHQ might report *“Unexpected exit code by spot”* when running on ATVA VM. This was not detected while running from a MacBook M1 locally, and since this is an issue (probably due to a different platform) from other tools we used for comparisons, and is not about HyperQB itself, we did not further debug on it. Please note this differences of the `error` cases we reported in Table 3.  
3. For fair comparison, we also make sure the models used by other tools 
are the same (all in NuSMV format). The only difference is the different .hq formula since the syntax of HyperQB vs AH/AHQ, are slightly different (however, the semantics of formula still conform). 




## Setting up the Artifact
We provide a one-click shell script to quickly setup everything on ATVA VM.<br/>
First, download and unzip *hyperqb-atva.zip*, step into the root directory:
```shell
cd hyperqb-atva/HyperQB/
```
Next, run the shell script to setup the environment on the VM:
```shell
sudo ./setup.sh
```
(ps. authentication might be needed here, enter "artifact" in the ATVA VM)
This script installed all required elements including docker and dotnet.
A succesful installation should shows the versions of docker and dotnet.

Note: in some rare cases, if any downloading issue happens, please run:
```sudo snap install docker``` and 
```sudo docker pull tzuhanmsu/hyperqube:latest```


## Quick Start for Smoke Test
(please make sure "Setting up the Artifact" was succesfully executed before continue!)<br/>
The smoke test review can be done in one-click
to quickly check if HyperQB is installable, functional and runnable.  

Run the following command for quick smoke testing our benchmarks:
```shell
sudo ./run_benchmarks.sh -light -alltools
```
(`-light` specify smaller instances, and `-alltools` runs over HyperQB and all tools for comparisons, as presented in Table 3)


## Detailed instructions for Full Review
(please make sure "Setting up the Artifact" was succesfully executed before continue!)<br/>
Our goal is to present the experiments in Table 3 of the paper. 
We provide different comamnds to:
1. run all cases using HyperQB,
2. run all cases using HyperQB and all other tools (AH/AHQ),
3. run specific case with selected tools.

### Run all cases using HyperQB in Table 3
Run all cases in Tables 3 using HyperQB with the following command:
```shell
sudo ./run_benchmarks -allcases -HQB
```
### Run all cases for Comparison with other Tools in Table 3
Compare all cases among all tools with the following command:
```shell
sudo ./run_benchmarks -allcases -alltools
```
### Test Specific Cases or Tools in Table 3
To make comparison easier for reviewers, we also provide convenient flags which the reviewer can specify what *case* and which *tools* to run:
```
sudo ./run_benchmarks '<-[case number]>' '<-[selected tool]>'
```
which
[selected tool] ranges from [-HQB | -OLDHQB | -AH | -AHQ]
[case number] ranges from 0.1 -- 17.1 (please refer to Table 3 for case numbers)

Here are some examples: 
To run case 9.1 using HyperQB(-HQB), Old-HyperQB(-OLDHQB), and AutoHyper(-AH):
```shell
sudo ./run_benchmarks -9.1 -HQB -OLDHQB -AH 
```

To run case 13.2 using all tools (HyperQB(-HQB), Old-HyperQB(-OLDHQB), and AutoHyper(-AH), AutoHyperQ(-AHQ)):
```shell
sudo ./run_benchmarks -13.2 -HQB -OLDHQB -AH 
```


## General Usage of HyperQB (Reusable Badge)
In the main directory, run the shell script that to setup the environment on the VM:
```shell
sudo ./setup.sh
```
To test small models and simple formulas, run any line below: 
```shell
sudo ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt
```
```shell
sudo ./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find
```
```shell
sudo ./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug
```
The script ```run_demo.sh``` contains more examples such that 
the reviewer can simply uncommand any case and execute it.


Thank you for using HyperQB!

# Welcome to HyperQB!!!
HyperQB is a home-grown tool of Bounded Model Checking for Hyperproperties.

Hyperproperty specifies and reasons about important requirements among multiple traces. We implement our QBF-based algorithm for Bounded Model Checking for Hyperproperty as a tool, HyperQB.

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
```shell
git clone https://github.com/TART-MSU/HyperQB.git
```
```shell
cd HyperQB
```
You are now ready to run HyperQB!:D

## How to Use
To run HyperQB, execute ```hyperqb.sh``` with the following inputs:
- `<list of models>` written in NuSMV format (as .smv files),
- `<formula>` written in the grammar described in Sec. 4 (as a .hq file),
- `<k>` a natural number 0, specifying the unrolling bound,
- `<sem>` the semantics, which can be -pes, -opt, -hpes or -hopt, and
- `<mode>` to say performing classic BMC (i.e., negating the formula) or not, which can be -bughunt or -find (we use the former as default value).  
    
RUN HyperQB in the following format:
```
./hyperqube <list of models> <formula> <k> <sem> <mode>
```

Demo Examples:
1. [demo 1: run bakery algorithm with symmetry property]
```shell
./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 -pes -bughunt
```

2. [dem0 2: run SNARK algorithm with linearizability propoerty]
```shell
./hyperqb.sh demo/snark_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -bughunt
```    


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
- Case #10.1 - #10.2: TINI and TSNI<br/>
- Case #11.1: K-safety<br/>
- Case #12.1: MapSynth1<br/>
- Case #12.2: MapSynth2<br/>
- Case #13.1: TeamLTL1<br/>
- Case #13.2: TeamLTL2<br/>
- Case #14.1: Non-deterministic init<br/>
- Case #14.2: Non-deterministic trans<br/>
- Case #15.1: CSRF<br/>
- Case #16.1: Bank<br/>
- Case #17.1: ATM<br/>


## People
Authors:
- [Tzu-Han Hsu](https://tzuhancs.github.io/), Michigan State University.
- [Borzoo Bonakdarpour](http://www.cse.msu.edu/~borzoo/), Michigan State University.
- [César Sánchez](https://software.imdea.org/~cesar/), IMDEA Software Institute.

## Publication
- [Bounded Model Checking for Hyperproperties (TACAS'21)](https://link.springer.com/content/pdf/10.1007/978-3-030-72016-2_6.pdf)

