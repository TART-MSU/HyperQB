#!/bin/sh
#coding:utf-8

HYPERQUBE=./hyperqube.sh
GENMODEL=./async_genmodel.sh
SOLVEQBF=./async_solveqbf.sh
DUMMYPROP=cases_async/dummy_prop.hq

# Example 1
# For checking and debugging
# python3 traj_parser.py

### generate model, build trajectories, then solve QBF
### uncomment to run example 1
# M=6
# TRAJ=12
# MODEL1=cases_async/acdb/acdb_composed.smv
# MODEL2=cases_async/acdb/acdb_composed.smv
# BUILDTRAJ=cases_async/acdb/build_acdb_1traj.py
# time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
# time python3 ${BUILDTRAJ} ${M} ${M} ${TRAJ}
# time ${SOLVEQBF}


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
M=8
TRAJ=16
MODEL1=cases_async/concleaks_new/conc_leaks_3procs.smv
MODEL2=cases_async/concleaks_new/conc_leaks_3procs.smv
BUILDTRAJ=cases_async/concleaks_new/build_conc_leaks_2trajs.py
time ${GENMODEL} ${MODEL1} ${MODEL2} ${DUMMYPROP} ${M} hpes -find -multi
time python3 ${BUILDTRAJ} ${M} ${M} ${M} ${M} ${TRAJ}
time ${SOLVEQBF}
## parse the trajectory counterexample
python3 traj_parser.py



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
