#!/bin/sh
############################
#  Simple demos of HyperQB #
############################
HYPERQB="./hyperqb.sh"
OLDHYPERQB="./hyperqb_old.sh"
# AUTOHYPER="/Users/tzuhan/Install/autohyper/app/AutoHyper"

### [demo 0: run simple_krip with simple_krip_formula]
${HYPERQB} demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt -NN
# ${HYPERQB} demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -bughunt -NY

### [demo 1: run info-flow example]
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 3 -pes -NY

# ${OLDHYPERQB} demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -find
# ${HYPERQB} demo/mini.smv demo/mini.smv demo/mini.hq 300 -pes -find
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 10 -pes -find

### [demo 1.1: depth]

# ${OLDHYPERQB} test/depth.smv test/depth.smv test/depth.hq 3 -pes -find
# ${OLDHYPERQB} demo/depth.smv demo/depth.smv demo/depth.hq 1000 -pes -find
# ${OLDHYPERQB} demo/depth.smv demo/depth.smv demo/depth.hq 10000 -pes -find
# ${OLDHYPERQB} demo/depth.smv demo/depth.smv demo/depth.hq 20000 -pes -find
# ${OLDHYPERQB} demo/depth.smv demo/depth.smv demo/depth.hq 40000 -pes -find
# ${OLDHYPERQB} demo/depth.smv demo/depth.smv demo/depth.hq 60000 -pes -find
# ${OLDHYPERQB} demo/depth.smv demo/depth.smv demo/depth.hq 5000 -pes -find


### [demo 1.2: width]
# ${OLDHYPERQB} demo/width7.smv demo/width7.smv demo/width.hq 3 -pes -NN -find
# ${OLDHYPERQB} demo/width10.smv demo/width10.smv demo/width.hq 2 -pes -NN -find
# time ${OLDHYPERQB} demo/width7.smv demo/width7.smv demo/width.hq 500 -pes -NN -find
# ${HYPERQB} test/width10.smv test/width10.smv test/width.hq 2 -pes -NN -find
# ${OLDHYPERQB} demo/width20.smv demo/width20.smv demo/width.hq 500 -pes -NN -find
# ${OLDHYPERQB} demo/width30.smv demo/width30.smv demo/width.hq 500 -pes -NN -find
# ${OLDHYPERQB} demo/width40.smv demo/width40.smv demo/width.hq 500 -pes -NN -find
# ${OLDHYPERQB} demo/width50.smv demo/width50.smv demo/width.hq 500 -pes -NN -find
# ${HYPERQB} test/width100.smv test/width100.smv test/width.hq 3 -pes -NN -find


### [demo 2: run bakery algorithm with symmetry property]

# ${HYPERQB} test/mini.smv test/mini.smv test/mini.hq 10 -pes -debug
# # rm _develop/HyperRust/cases/bakery/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/mini/

# ${HYPERQB} test/mutation.smv test/mutation.smv test/mutation.hq 10 -pes -find
# # rm _develop/HyperRust/cases/bakery/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/mutation/

# ${HYPERQB2} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 30 -pes -debug

### [demo 3: run SNARK algorithm with linearizability propoerty, bug1]
# ${OLDHYPERQB} test/array.smv test/array.smv test/array.hq 2 -pes -debug
# ${HYPERQB} demo/snark1_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -debug



# ${OLDHYPERQB} benchmarks/15_csrf/csrf.smv benchmarks/15_csrf/csrf.smv benchmarks/15_csrf/csrf.hq 2 -pes -debug

# ${HYPERQB} benchmarks/3_ni/NI_incorrect.smv benchmarks/3_ni/NI_incorrect.smv benchmarks/3_ni/NI.hq 3 -hpes -find
# ${HYPERQB} benchmarks/3_ni/NI_correct.smv benchmarks/3_ni/NI_correct.smv benchmarks/3_ni/NI.hq 15 -hpes -find

# ${HYPERQB} test/extra/cmu1.smv test/extra/cmu1.smv test/extra/cmu1.smv test/extra/cmu1.hq 6 -pes -debug


# ${HYPERQB} test/width300.smv test/width300.smv test/width.hq 3 -pes -NN -find
# rm _develop/HyperRust/cases/width/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/width/



# ${HYPERQB} test/snark_conc.smv test/snark_seq.smv test/snark.hq 18 -pes -find
# rm _develop/HyperRust/cases/snark/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/snark/

# time ${AUTOHYPER} --witness --log --nusmv test/snark_conc.smv test/snark_seq.smv test/snark_AH.hq 

# ${HYPERQB} test/counter.smv test/counter.smv test/counter.hq 3 -pes -find
# rm _develop/HyperRust/cases/counter/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/counter/

# ${HYPERQB} test/slides.smv test/slides.smv test/slides.hq 3 -pes -find
# rm _develop/HyperRust/cases/slides/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/slides/

# time ${AUTOHYPER} --witness --log --nusmv test/snark1_conc_NEW.smv test/snark1_seq_NEW.smv test/snark_AH.hq 

# ${HYPERQB} test/snark1_conc_NEW.smv test/snark1_seq_NEW.smv test/snark.hq 18 -pes -find
# rm _develop/HyperRust/cases/snark/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/snark/

# ${HYPERQB} demo/bakery.smv demo/bakery.smv demo/bakery.hq 10 -pes -debug
# rm _develop/HyperRust/cases/bakery/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/bakery/

# ${HYPERQB} test/det512.smv test/det512.smv test/width.hq 3 -pes -find
# rm _develop/HyperRust/cases/width/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/width/

# ${HYPERQB} test/ndI_detR_300.smv test/ndI_detR_300.smv test/width.hq 3 -pes -find
# rm _develop/HyperRust/cases/width/*.bool
# cp _output:today/*.bool _develop/HyperRust/cases/ndI_detR/