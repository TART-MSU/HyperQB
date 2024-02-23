# CSE 814 Project Set B (Spring 2024)

This package contains the information and necessary information for 814 class project


## Big Picture

1. build the emperical set for the bounded model checker HyperQB (https://github.com/TART-MSU/HyperQB)
2. report model checking results (SAT/UNSAT), including stats (parsing/unrolling/QBFsolving time, qnatifiers, flags (-bughunt, -find), state space, semantics, and bound k).  
3. observe the result and analyze the cases based on the result (like where the bottle neck is, if run time is long)
4. based on the observation, scale the cases with different aspects (e.g., more non-determinism in input or transitions)


## What's in this package?
* background/
    + infoflow1/: 
    The mini examples of verifying information-flow security. Learn how basic syntax of NuSMV and how transition relation works.
    + infoflow2/:
    The minimal example that covers more syntax of NuSMV. Learn how integer Booleanization and exhausive conditional searching works. 


* case_medium/
    This folder contains some medium cases to start with. For each folder, you should have a .smv and .hq file, and is able to run them using HyperQB with correct parameter (please modify run.sh) 


* case_large/
    This is the largest case that will be using in the (main) benchmark set.

## Contact
Please email tzuhan@msu.edu if you have any questions.






