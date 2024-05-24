# README for ATVA24 Tool Papaer Artifact Evaluation
Title:    HyperQB: A QBF-Based Bounded Model Checker for Hyperproperties <br/>
Authors:  Tzu-Han Hsu, Borzoo Bonakdarpour, César Sánchez



## Hyperlink to the artifact and ATVA VM
* HyperQB artifact DOI: [...](...) (please check the latest version) <br/>
* HyperQB artifact Zenodo link: [...](...) (please check the latest version) <br/>
* HyperQB artifact checksum: ... <br/>
* ATVA VM Zenodo link: https://zenodo.org/records/10928976 <br/>



## Introduction
HyperQB is a home-grown tool of Bounded Model Checking for Hyperproperties.Hyperproperty specifies and reasons about important requirements among multiple traces. We implement our QBF-based algorithm for Bounded Model Checking for Hyperproperty, HyperQB.

The dataflow of HyperQB includes several parts:
* NuSMV model parsing and Boolean encoding of transition relation and specification,
* HyperLTL formula translation,
* QBF encoding of unfolding with bound k using specific semantics,
* QBF solving with QBF solver QuAbs.  

HyperQB is under MIT license as presented in `LICENSE.txt`, while the existing solver we use, `QuAbs`, is under AGPL license.  


## Environment requirements for the artifact
* To provide the best evaluating experience for the reviewers, we have tested this artifact on the officially provided VM *Artifact_VM_Ubuntu_22.04.ova* (downloaded from https://zenodo.org/records/10928976).
* Each binary executable is already pre-compiled by us on the ATVA VM!:D No further extra compilation hassle needed from the reviewers.
* To show our artifact is self-contained, we have includes all the sources (include other tools that we use for comparison as zip files, in `src/`)  
* Our artifact is partially using Docker image to reduce the reviewer's burden on the dependencies installation. (Note: The core technical parts of HyperQB BMC algorithm (i.e., encoding, unrolling, QBF-solving, etc.), are still self-contained in this artifact.)


## Clarification on Internet Access: 
We aim for both "Available & Reusable" badges, as a result, the internet connection should be avoided as much as possible. To achieve this, we clarify the adjustment to a self-contained docker image, and the .NET installation due to other tools (but not the HyperQB itself). In general:

1. we packaged the Docker image as `hyperqb_docker.tar.gz`, and deploy the image by running `docker load < hyperqb_docker.tar.gz` (by executing `sudo ./setdocker.sh` right after `sudo setup.sh`).
2. the only installation that requires Internet access is `dotnet`, which is required by two other tools that we use for comparison (AutoHyper and AutoHyperQ), but not HyperQB itself. In order to have our artifact fully self-contained, we have pre-compiled all executable and remove the installtion of .NET in `setup.sh`.       



## Artifact Structure 
Our artifact `HyperQB/` is structured as follows:
* `benchmarks/` contains benchmarks we used for evaluation.
* `demo/` contains small instances as demonstrative examples.
* `exec/` contains all pre-compiled executable (for different platforms), including AutoHyper (AH), and AutoHyperQ (AHQ), which are the tools we did comparison with on the benchmarks.
* `src/` contains the source code of HyperQB. 
* when running HyperQB the temp containers `build_today` and `build_cex` will also be listed.

The goal of this artifact is to produce results of Table 3 and Table 4 in the paper, including:
* Results produced by HyperQB.
* Comparisons of HyperQB with previous prototype[24] , AutoHyper[2], and AutoHyperQ[3].



## Quick Remarks on this Artifact
1. We notice that while running our experiments on the ATVA VM, the overall runtime *increase* (in comparison to our reported runtime in Table 3 and Table 4, obtained from running on a MacBook M1 locally).
2. We notice that in some cases, AH/AHQ might report *“Unexpected exit code by spot”* when running on ATVA VM. This was not detected while running from a MacBook M1 locally, and since this is an issue (probably due to a different platform) from other tools we used for comparisons, and is not about HyperQB itself, we did not further debug on it. Please note this differences of the `error` cases we reported in Table 3 and Table 4.  
3. For fair comparison, we also make sure the models used by other tools 
are the same (all in NuSMV format). The only difference is the different .hq formula since the syntax of HyperQB vs AH/AHQ, are slightly different (however, the semantics of formula still conform). 



## Setting up the Artifact
For comparisons with other tools, we kindly ask the reviewers to extract the artifact in the *home* directory, i.e., with `${PWD}` as `/home/artifact/HyperQB` (this is due to AH and AHQ requiring absolute paths to spot).

We provide a one-click shell script to quickly setup everything on [ATVA VM](https://zenodo.org/records/10928976).<br/>
First, download and unzip *HyperQB.zip*, step into the root directory:
(!) Notice that since other tools we used for comparing with HyperQB requried absolute paht for sub-tool, please make sure the unzipped folder is located in the home directory (i.e., `home/artofact/HyperQB`)
```shell
cd HyperQB/
```
Next, run the shell script to setup the environment on the VM and load:
```shell
sudo ./setup.sh && newgrp docker
```
(ps. authentication might be needed here, enter "artifact" in the ATVA VM)
This script installed all required elements. A succesful installation should display the version of docker.
Note: in very rare cases, if any image reading issue happens in the VM (since this is the major update in revision so we want to make sure the reviewers still get the chance to run our tool), please run the following manually:
```sudo snap install docker``` and 
```sudo docker pull tzuhanmsu/hyperqube:latest```



## First Example and Tool Output Interpretation
We here provide a minimal example of how to interpret HyperQB's inputs and outputs by executing the following command:
```shell
./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt
```
This comammand is calling `hyperqb.sh` on the following arguments:
``<Model1>  <Model 1>  <Formula>  <bound k>  <semantics> <mode>``
(We provide brief desctions below; however please refer to paper page 7 "tool Usage and Unrolling of HyperQB for more details)

After a successful run, the HyperQB output will be displayed as follows:
```
-------( HyperQB START! )-------
NuSMV and HyperLTL parsing...0.092
BMC unrolling with genqbf....0.015
QBF solving with QuAbS.......0.007
 ------ Summary of HyperQB ------
|  Model:       demo/mini.smv
|  Model:       demo/mini.smv
|  Formula:     demo/mini.hq
|  Quantifiers: AE
|  QCIR size:   4 KB
|  QBF solving: r UNSAT
|  Mode:        -bughunt
|  Semantics:   PES
|  #States:     |M1|=2
|  Bound k:     3
 --------------------------------
TOTAL TIME: 0.100s 
--------( HyperQB END )---------
```

The displayed outcomes includes: 
* Time took for parsking NuSMV model and HyperLTL formula (in seconds)
* Time took for BMC unrooling and generating QCIR file (in seconds)
* Time took for QuAbS for solving the generating QCIR file (in seconds)
* In the Summry of HyperQB, we provide:
    - [ Model ]: the model to be check, in this case `demo/mini.smv` (note that this is a specific case, such that 2 quantifiers in the formula follows the same model, so you will see two lines of "Model" displyed the same name. However in the cases that quantifiers are instantiated by 2 different models (e.g., 2.1, 2.2, 7.1), the two lines of "Model" will display two diffeernt .smv files). 
    - [ Formula ]: the hyperproperty to check with the models, expressed as HyperLTL formula, in this case `demo/mini.hq`.
    - (Quantifiers): quaifier form of the formula, in this case `AE` means "forall. exists."
    - [ QCIR size ]: the size of generate QCIR file
    - [ QBF solving ]: the QBF soling result, in this case it's UNSAT, meaning unsatisfiable.
    - [ Mode ]: the mode of HyperQB, in this case `-bughunt` means the formula is negated as traditional BMC. (please refer to paper page 7 "Tool Outputs")
    - [ Semantics ]: the chosen bounded semantics, in this case `PES` means "pessimistic semantic". (please refer to paper page 4 and Table 1 about "Bounded Semantics for HyperLTL")
    - [ #States ]: the state space of the given models, in this case `2` since this mini.smv only has 2 states (note that two models are the same, so we only display size of M1 here for clarity)
    - [ Bound k ]: the number of execution steps to unroll (i.e., the "bound" in BMC), in this case `3`.

We here provide a general overview of how to interpret the output information, for more details please refer to Section 5 in our paper.


## Quick Start for Smoke Test
(Note: please make sure "Setting up the Artifact" was succesfully executed before continue!)<br/>
The smoke test review can be done in one-click, to quickly check if HyperQB is installable, functional and runnable, by running the following command:
```shell
./run_benchmarks.sh -light -alltools
```
The options `-light` specify a subset of smaller instances for the entire benchmarks, and `-alltools` runs over HyperQB and all other tools for comparisons, as presented in Table 3 and Table 4. 


## Detailed instructions for Full Review
(Note: please make sure "Setting up the Artifact" was succesfully executed before continue!)<br/>

### Convenient Flags for `run_benchmarks.sh`
To provide a smooth and enjoyable review experience, we design different flags for `run_benchmarks.sh` to allow the reviewer customize each desirable test with selected case(s) and selected tool(s), including:
1. Case selections:
* `-allcases`: run the full benchmarks (all cases on `Table 3` and `Table 4` in the paper).
* `-light`: run a subset of smaller cases in the benchmarks (same as smoke test).
* `-<case#>`: run one specific case from the benchmark, where `<case#>` can ranges from `0.1` to `17.1`.

2. Tool selections:
* `-HQB`: run with HyperQB only
* `-OLDHQB`: run with a previous prototype[24] for comparison purpose.
* `-AH`: run with AutoHyper[2] for comparison purpose.
* `-AHQ`: run with AutoHyperQ[3] for comparison purpose.
* `-alltools`: run with all four aforementioned tools.
(Note that we also allow different conbinations for the 4 tools. For example, if the reviewer would like to compare only HyperQB and AutoHyper, add both `-HQB` and `-AH`. Please see below for example)

### Test Full Benchmark (Table3 and 4 in the Paper)
To run **all cases** using **HyperQB** only:
```shell
./run_benchmarks -allcases -HQB
```
To run **all cases** among **all tools**:
```shell
./run_benchmarks -allcases -alltools
```
To run **all cases** using only **HyperQB** and **AutoHyper**: 
```shell
./run_benchmarks -allcases -HQB -AH
```


### Test Specific Cases on Specific Tool(s)
To avoid long execution time and to make comparison easier for reviewers, the reviewr can also freely combine the convenient flags to specify what *case* and which *tools* to run, as follows:
```
sudo ./run_benchmarks '-<case#>' '[ -<selected tool> ] '
```
which
`<case#>` ranges from 0.1 -- 17.1 (please refer to Table 3 and Table 4 for specific case numbers).
`<selected tool>` ranges from [ -HQB | -OLDHQB | -AH | -AHQ ]


Here are some examples: <br/>
To run `case 9.1` using `HyperQB(-HQB)`, `Old-HyperQB(-OLDHQB)`, and `AutoHyper(-AH)`:
```shell
./run_benchmarks -9.1 -HQB -OLDHQB -AH 
```

To run `case 13.2` using all tools (`HyperQB(-HQB)`, `Old-HyperQB(-OLDHQB)`, and `AutoHyper(-AH)`, `AutoHyperQ(-AHQ)`):
```shell
./run_benchmarks -13.2 -HQB -OLDHQB -AH -AHQ
```


### General Usage of HyperQB (reusable badge)
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

#### Demo Examples:

To test small models and simple formulas, run any line below: 
```shell
./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt
```
```shell
./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find
```
```shell
./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug
```
The script ```run_demo.sh``` contains more examples such that the reviewer can simply uncommand any case and execute it. 


## Summary of Experiments
Our evaluations include the following cases, covering all instances in `Table 3` and `Table 4` in the paper. (All models and formulas are in the folder`benchmarks/`)

| Case # | Case Description |
| :---        | :----       |
| #1.1 - 1.4 |  Symmetry in the Bakery Algorithm |
| #2.1 - 2.2 |  Linearizability in SNARK Algorithm |
| #3.1 - 3.2 |  Non-interference in Multi-threaded Program |
| #4.1 - 4.2 |  Fairness in Non-repudiation Protocols |
| #5.1 - 5.2 |  Privacy-Preserving Path Synthesis for Robots |
| #6.1 |  Mutant Synthesis for Mutation Testing |
| #7.1 |  Co-termination of multiple programs |
| #6.1 |  Mutant Synthesis for Mutation Testing |
| #6.1 |  Mutant Synthesis for Mutation Testing |
| #6.1 |  Mutant Synthesis for Mutation Testing |
| #6.1 |  Mutant Synthesis for Mutation Testing |
| #7.1 |  Co-termination between Two Programs|
| #8.1 |  Deniability of Digital Wallet|
| #9.1 - 9.3 |  Intransitive Non-interference of Shared Butter|
| #10.1 - 10.2 |  TINI and TSNI with Non-determinism|
| #11.1 |  K-safety on DoubleSquare Program|
| #12.1 |  Mapping Synthesis -- small |
| #12.2 |  Mapping Synthesis -- large |
| #13.1 |  TeamLTL on example program 1 |
| #13.2 |  TeamLTL on example program 2 |
| #14.1 |  Non-deterministic of Initial Conditions |
| #14.2 |  Non-deterministic of Transition Relations |
| #15.1 |  Cross-site-request-forgery Web Attack |
| #16.1 |  Information Leaking in Banking System |
| #17.1 |  Data Breach in ATM Machine |


## Additional Instruction of Compiling Other Tools

## People
Authors:
- [Tzu-Han Hsu](https://tzuhancs.github.io/), Michigan State University.
- [Borzoo Bonakdarpour](http://www.cse.msu.edu/~borzoo/), Michigan State University.
- [César Sánchez](https://software.imdea.org/~cesar/), IMDEA Software Institute.

## Publication
- [Bounded Model Checking for Hyperproperties (TACAS'21)](https://link.springer.com/content/pdf/10.1007/978-3-030-72016-2_6.pdf)


