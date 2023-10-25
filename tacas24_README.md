# README for TACAS24 Tool Papaer Artifact Evaluation

easychair paper#
easychair artifact#

## A hyperlink to the artifact


## Additional requirements for the artifact, such as installation of proprietary software or particular hardware resources

Following the state from this year's instruction "...allow reasonable network access from the VM...",  Our submission is partially using Docker as to strength the part tat replies on some 


## Detailed instruction for an early light review

We provide two scripts for light-weight review: 

1. several small models and simple formulas: 
    ```./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt```
    ```./hyperqb.sh demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find```
    ```./hyperqb.sh demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug```

2. ```run_demo.sh``` contains more examples that the reviewer can simply uncommand any case and execute this shell script to run HyperQB

3. ```run_lightway.sh``` contains all relatively smaller cases (i.e., < 2mins), the entire script should run xxx cases and the expected time spent is xxx second.



## detailed instructions for use of the artifact and replication of results in the paper, including estimated resource use if non-trivial

We provide a one click script ```run_benchmarks.sh``` for replicating all results resented in the paper. For the reviewers convenienet, we provide several useful flags:
- ```run_benchmarks.sh <case#>``` runs a specific case from the paper,
- ```run_benchmarks.sh -all``` runs all cases from the paper
- addition flags ```-AH``` and ```-QAH``` runs one/all cases together with AutoHyper and AutoHyperQ, respectively, for the comparison presented in the paper. 

For example, ```run_benchmarks.sh 1.3 -AH``` runs case #1.3 from the table (i.e., Bakery), with HyperQB, accompany by the outcome of the exact same case run on AutoHyper. Similarly, ```run_benchmarks.sh 9.2 -QAH``` runs case #9.2 with HyperQB abd AutoHyperQ. 

To replicate the entire table, run ```run_benchmarks.sh -all -AH -QAH``` 

We marked here those cases that would take longer time to execute for reviewers reference:







