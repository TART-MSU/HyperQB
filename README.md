# README for TACAS24 Tool Papaer Artifact Evaluation
Title:      HyperQB: A QBF-Based Bounded Model Checker for Hyperproperties
Authors:    Tzu-Han Hsu, Borzoo Bonakdarpour, César Sánchez
Paper:      #7708
Artifact:   #2082

## A hyperlink to the artifact
https://github.com/HyperQB/TACAS24-AE

## Additional requirements for the artifact
To provide the best evaluating experiemnce for the reviewers, 
we have tested this artifact on the provided virtual machine
tacas23-AEC.ova (reused in 2024). 
Each bin executable are pre-compiled by us, so they are garanteed
to work as expected on the same VM. 

By running the all-in-one ```setup.sh```, all required setup will be done. 

Following the state from this year's instruction 
"...allow reasonable network access from the VM...", 
our submission is partially using Docker image
to reduce the reviewer's burden on the dependencies installation.
Note:   The core technical parts of HyperQB BMC algorithm
        (i.e., encoding, unrolling, QBF-solving, etc.),
        are still self-contained in this artifact. 
        This docker image only resolve the initial parsing, 
        before the Boolean formula to QBF query transformation. 

      
## Detailed instruction for an early light review

The light-weight review can be done in following ways
to quickly show our tool is functional and runnable.

Step 0. unzip tacas.zip, then
        ```cd HyperQB```

Step 1. Complete setup, run: 
        ```sudo ./setup.sh```

Step 2. Test small models and simple formulas, run any line below: 
        ```sudo ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt```
        ```sudo ./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find```
        ```sudo ./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug```
        The script ```run_demo.sh```contains more examples such that 
        the reviewer can simply uncommand any case and execute it.

Step 3. Run a subset of lighter cases from our benchmark, run:
        ```sudo run_lightway.sh``` 
        The script contains all relatively smaller cases (i.e., < 2mins), 
        the entire script runs xxx cases and the expected time spent is xxx second.

## detailed instructions for use of the artifact and replication of results in the paper, including estimated resource use if non-trivial

Our goal is to present the experiments of Table 3 in the paper. 
We here provide a step by step instructions: 

Step 1. Complete setup, run: 
        ```sudo ./setup.sh```

Step 2. Test small models and simple formulas, run any line below: 
        ```sudo ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt```
        ```sudo ./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find```
        ```sudo ./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug```
        The script ```run_demo.sh```contains more examples such that 
        the reviewer can simply uncommand any case and execute it.

Step 3. Run a subset of lighter cases to test HyperQB, run:
        ```sudo run_lightway.sh``` 

Step 4. Setup other tools for comparison: AutoHyper(AH) and AutoHyperQ(QAH)
        We have compiled both tool in advance, the reviewers only need to:
        - go to spot-2.11.5/bin, make sure there are two binarys named: 
            autfilt, ltl2gba
        - now, oepn AutoHyper/app/paths.js
        - make sure paths are pointing to *correct absolute* path of the two binarys:
            ``...tacas/spot-2.11.5/bin/autfilt``
            ``...tacas/spot-2.11.5/bin/ltl2tgba``
        - next, open AutoHyperQ/app/paths.js
        - make sure in this file, the paths are also pointing to the right binaries.
        Note: the default locations in the files are from our building process.
        The actual absolute path depends on where the reviewers locate this artifact, 
        Please do not change the reletive structure of HyperQB with AH or QAH.
         
Step 5. Experiments Replication
        We provide a one-click script ```run_benchmarks.sh``` for replicating all results in the paper. For the reviewers convenienet, we provide several useful flags:
        - ```sudo run_benchmarks.sh <case#>``` runs a specific case from the paper,
        - ```sudo run_benchmarks.sh -all``` runs all cases from the paper
        - addition flags ```-AH``` and ```-QAH``` runs one/all cases together with AutoHyper and AutoHyperQ, respectively, for the comparison presented in the paper. 
        For example, run
        ```sudo run_benchmarks.sh 1.3 -AH``` 
        It runs case #1.3 from the table (i.e., Bakery) with HyperQB, 
        accompany by the outcome of the exact same case run on AutoHyper. 
        Similarly, run
        ```sudo run_benchmarks.sh 9.2 -QAH``` 
        It runs case #9.2 (shared buffer) with HyperQB abd AutoHyperQ. 
        To replicate the entire table, including all comparisons run:
        ```run_benchmarks.sh -all -AH -QAH``` 
        The cases that take HyperQB longer than a minute are:
        #2.1, #2.2, #8.2, #13.2
        AutoHyper usually take longer than the timeout bound, to save time, 
        the reviewer can change the parameter on line10 of ```run_bencmarks.sh```


Thank you for using HyperQB!


(BELOW are old general README for last release, TACAS reviewers can ignore from here)


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


## People
Authors:
- [Tzu-Han Hsu](https://tzuhancs.github.io/), Michigan State University.
- [Borzoo Bonakdarpour](http://www.cse.msu.edu/~borzoo/), Michigan State University.
- [César Sánchez](https://software.imdea.org/~cesar/), IMDEA Software Institute.

## Publication
- [Bounded Model Checking for Hyperproperties (TACAS'21)](https://link.springer.com/content/pdf/10.1007/978-3-030-72016-2_6.pdf)

