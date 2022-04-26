#!/bin/sh
#coding:utf-8
HYPERQUBE=./hyperqube.sh
GENMODEL=./async_genmodel.sh
SOLVEQBF=./async_solveqbf.sh
DUMMYPROP=cases_async/dummy_prop.hq
PARSETRAJ=async_traj_parser.py
BUILD_V1=cases_async/buildtraj_v1.py # AAE
BUILD_V2=cases_async/buildtraj_v2.py # AAAE
BUILD_V3=cases_async/buildtraj_v3.py # AAE

TIMEFORMAT='>>>>>>>>>>>>>>> Total: %R sec <<<<<<<<<<<<<<<'

# rm time.txt

### generate model, build trajectories, then solve QBF

#####################
#    ACBD example   #
#####################

### uncomment to run ACDB original examle
# M=6
# TRAJ=12
# MODEL1=cases_async/acdb/original/acdb_composed.smv
# MODEL2=cases_async/acdb/original/acdb_composed.smv
# BUILDTRAJ=${BUILD_V1}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}

### uncomment to run ACDB examle with non-determinsm
# M=8
# TRAJ=16
# MODEL1=cases_async/acdb/with_ndet/acdb_composed.smv
# MODEL2=cases_async/acdb/with_ndet/acdb_composed.smv
# BUILDTRAJ=${BUILD_V2}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}
# python3 ${PARSETRAJ}


########################
#   concurrent leaks   #
########################

### uncomment to run conc_leak original
# M=6
# TRAJ=12
# MODEL1=cases_async/concleaks/original/conc_leaks_2procs.smv
# MODEL2=cases_async/concleaks/original/conc_leaks_2procs.smv
# BUILDTRAJ=${BUILD_V1}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}

### uncomment to run conc_leak new
# M=6
# TRAJ=12
# MODEL1=cases_async/concleaks/with_ndet/conc_leaks_3procs.smv
# MODEL2=cases_async/concleaks/with_ndet/conc_leaks_3procs.smv
# BUILDTRAJ=${BUILD_V2}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}


########################
#   optimiazation      #
########################



########################
#   Original: DBE      #
########################
### uncomment to run example original optimization _ DBE
# M=4
# TRAJ=8
# MODEL1=cases_async/optimization/original/dbe/DBE_source.smv
# MODEL2=cases_async/optimization/original/dbe/DBE_target.smv
# BUILDTRAJ=${BUILD_V3}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi;
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ};
# time ${SOLVEQBF}

# Check for termination
# ${HYPERQUBE} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi


########################
#   Original: LP      #
########################
### uncomment to run example original optimization _ LP
# M=17
# TRAJ=34
# MODEL1=cases_async/optimization/original/lp/LP_source.smv
# MODEL2=cases_async/optimization/original/lp/LP_target.smv
# BUILDTRAJ=${BUILD_V3}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}

# Check for termination
# ${HYPERQUBE} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi

########################
#   Original: EFLP      #
########################
### uncomment to run example original optimization _ LP
# M=25
# TRAJ=30
# MODEL1=cases_async/optimization/original/eflp/EFLP_source.smv
# MODEL2=cases_async/optimization/original/eflp/EFLP_target.smv
# BUILDTRAJ=${BUILD_V3}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}

## Check for termination
# ${HYPERQUBE} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi


######################
#   with_ndet: DBE   #
######################
### OPTIMIZATION NEW
# ### generate model, build trajectories, then solve QBF
# ### uncomment to run DBE new correct
M=13
D1=13
D2=13
TRAJ=26
MODEL1=cases_async/optimization/with_ndet/dbe/DBE_source_ndet.smv
MODEL2=cases_async/optimization/with_ndet/dbe/DBE_target_ndet.smv
BUILDTRAJ=${BUILD_V2}
time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
time python3 ${BUILDTRAJ} ${D1} ${D2} ${TRAJ}
time ${SOLVEQBF}
# ## Check program termination
# # ${HYPERQUBE} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi

# # ### uncomment to run DBE new wrong
# M=13
# D1=13
# D2=13
# TRAJ=26
# MODEL1=cases_async/optimization/with_ndet/dbe/DBE_source_ndet.smv
# MODEL2=cases_async/optimization/with_ndet/dbe/DBE_target_wrong_ndet.smv
# BUILDTRAJ=${BUILD_V2}
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${D1} ${D2} ${TRAJ}
# time ${SOLVEQBF}
# python3 ${PARSETRAJ}
## Check program termination
# ${HYPERQUBE} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
