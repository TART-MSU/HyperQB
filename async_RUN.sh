#!/bin/sh
#coding:utf-8

HYPERQUBE=./hyperqube.sh
GENMODEL=./async_genmodel.sh
SOLVEQBF=./async_solveqbf.sh
DUMMYPROP=cases_async/dummy_prop.hq
PARSETRAJ=async_traj_parser.py


### generate model, build trajectories, then solve QBF

#####################
#    ACBD example   #
#####################

### uncomment to run ACDB original examle
# M=6
# TRAJ=12
# MODEL1=cases_async/acdb/original/acdb_composed.smv
# MODEL2=cases_async/acdb/original/acdb_composed.smv
# BUILDTRAJ=cases_async/acdb/original/buildtraj_acdb_original.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}


### uncomment to run ACDB examle with non-determinsm
M=8
TRAJ=16
MODEL1=cases_async/acdb/with_ndet/acdb_composed.smv
MODEL2=cases_async/acdb/with_ndet/acdb_composed.smv
BUILDTRAJ=cases_async/acdb/with_ndet/buildtraj_acdb_ndet.py
time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
time ${SOLVEQBF}
python3 ${PARSETRAJ}


# # Example 2
# # For checking and debugging
# # python3 traj_parser.py
#
# ### generate model, build trajectories, then solve QBF
# ### uncomment to run example 1
# M=6
# TRAJ=12
# MODEL1=cases_async/concleaks_original/conc_leaks_2procs.smv
# MODEL2=cases_async/concleaks_original/conc_leaks_2procs.smv
# BUILDTRAJ=cases_async/concleaks_original/build_conc_leaks_1traj.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}


# Example 3
# For checking and debugging
# python3 traj_parser.py

### generate model, build trajectories, then solve QBF
### uncomment to run example 1
# M=8
# TRAJ=16
# MODEL1=cases_async/concleaks_new/conc_leaks_3procs.smv
# MODEL2=cases_async/concleaks_new/conc_leaks_3procs.smv
# BUILDTRAJ=cases_async/concleaks_new/build_conc_leaks_2trajs.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}
# ## parse the trajectory counterexample
# python3 traj_parser.py



# Optimization
# For checking and debugging
# python3 traj_parser.py

# ### generate model, build trajectories, then solve QBF
# ### uncomment to run example 1
# M=8
# TRAJ=16
# MODEL1=cases_async/optimization/opt_LP_source.smv
# MODEL2=cases_async/optimization/opt_LP_target.smv
# BUILDTRAJ=cases_async/optimization/build_opt_1traj.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# # time ${SOLVEQBF}

# ### generate model, build trajectories, then solve QBF
# ### uncomment to run DBE Original
# M=3
# TRAJ=6
# MODEL1=cases_async/optimization/cav21/opt_DBE_source.smv
# MODEL2=cases_async/optimization/cav21/opt_DBE_target.smv
# BUILDTRAJ=cases_async/optimization/cav21/buildtraj_opt.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}
# # python3 ${PARSETRAJ}

# M=7
# TRAJ=14
# MODEL1=cases_async/optimization/cav21/opt_DBE_source_new.smv
# MODEL2=cases_async/optimization/cav21/opt_DBE_target_new.smv
# BUILDTRAJ=cases_async/optimization/cav21/buildtraj_opt.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}

# M=7
# TRAJ=14
# MODEL1=cases_async/optimization/cav21/opt_DBE_source_new.smv
# MODEL2=cases_async/optimization/cav21/opt_DBE_target_wrong_new.smv
# BUILDTRAJ=cases_async/optimization/cav21/buildtraj_opt.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}



### OPTIMIZATION NEW
# ### generate model, build trajectories, then solve QBF
# ### uncomment to run DBE new correct
# M=8
# D1=8
# D2=8
# TRAJ=16
# MODEL1=cases_async/optimization/with_ndet/opt_DBE_source_ndet.smv
# MODEL2=cases_async/optimization/with_ndet/opt_DBE_target_ndet.smv
# BUILDTRAJ=cases_async/optimization/with_ndet/buildtraj_opt_ndet.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${D1} ${D2} ${TRAJ}
# time ${SOLVEQBF}
# python3 ${PARSETRAJ}
# # ### uncomment to run DBE new wrong
# M=8
# D1=8
# D2=8
# TRAJ=16
# MODEL1=cases_async/optimization/with_ndet/opt_DBE_source_ndet.smv
# MODEL2=cases_async/optimization/with_ndet/opt_DBE_target_wrong_ndet.smv
# BUILDTRAJ=cases_async/optimization/with_ndet/buildtraj_opt_ndet.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${D1} ${D2} ${TRAJ}
# time ${SOLVEQBF}
# python3 ${PARSETRAJ}


### OPTIMIZATION NEW with DELAY
# # ### generate model, build trajectories, then solve QBF
# # ### uncomment to run TEST DBE new correct
# M=10
# D1=8
# D2=5
# TRAJ=13
# MODEL1=cases_async/optimization/with_delay/opt_smallDBE_source_delay.smv
# MODEL2=cases_async/optimization/with_delay/opt_smallDBE_target_delay.smv
# BUILDTRAJ=cases_async/optimization/with_delay/buildtraj_opt_delay.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${D1} ${D2} ${TRAJ}
# time ${SOLVEQBF}
# python3 ${PARSETRAJ}

# ### generate model, build trajectories, then solve QBF
# ### uncomment to run TEST DBE new incorrect
# M=10
# D1=8
# D2=5
# TRAJ=13
# MODEL1=cases_async/optimization/with_delay/opt_smallDBE_source_delay.smv
# MODEL2=cases_async/optimization/with_delay/opt_smallDBE_target_wrong_delay.smv
# BUILDTRAJ=cases_async/optimization/with_delay/buildtraj_opt_delay.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${D1} ${D2} ${TRAJ}
# time ${SOLVEQBF}
# python3 ${PARSETRAJ}
