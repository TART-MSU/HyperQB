# README for TACAS24 Tool Papaer Artifact Evaluation

easychair paper#
easychair artifact#

## A hyperlink to the artifact



## Additional requirements for the artifact

To provide the best evaluating experiemnce for the reviewers, 
we have tested this artifact on the provided virtual machine
tacas23-AEC.ova (reused in 2024). 
Each bin executable are pre-compiled by us, so they are garanteed
to work as expected on the same VM. 

Following the state from this year's instruction 
"...allow reasonable network access from the VM...", 
our submission is partially using Docker image
to reduce the reviewer's burden on the dependencies installation.

By running the all-in-one ```setup.sh```, all required setup will be done. 

Note:   The core technical parts of HyperQB BMC algorithm
        (i.e., encoding, unrolling, QBF-solving, etc.),
        are still self-contained in this artifact. 
        This docker image only resolve the initial parsing, 
        before the Boolean formula to QBF query transformation. 

      
## Detailed instruction for an early light review

The light-weight review can be done in following ways
to quickly show our tool is functional and runnable.

1. Complete setup, run: 
    ```sudo ./setup.sh```

2. Test small models and simple formulas, run any line below: 
    ```sudo ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt```
    ```sudo ./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find```
    ```sudo ./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug```

    The script ```run_demo.sh```contains more examples such that 
    the reviewer can simply uncommand any case and execute it.

3. Run a subset of lighter cases from our benchmark, run:
    ```sudo run_lightway.sh``` 
    The script contains all relatively smaller cases (i.e., < 2mins), 
    the entire script runs xxx cases and the expected time spent is xxx second.

## detailed instructions for use of the artifact and replication of results in the paper, including estimated resource use if non-trivial

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

We marked here those cases that would take longer time to execute for reviewers reference:



Thank you for using HyperQB!




