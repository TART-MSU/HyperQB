# HyperQube subTools
GENQBF=exec/genqbf
QUABS=exec/quabs
MAP=exec/util_mapvars
PARSE_BOOL=exec/util_parsebools

# output files
QCIR_OUT=HQ_async.qcir
QUABS_OUT=HQ.quabs

# simple mapping of variable names and values
# TODO: do it in python
# MAP=util/util_mapvars.java
# MAP=util_mapvars
MAP_OUT1=OUTPUT_byName.cex
MAP_OUT2=OUTPUT_byTime.cex
# simple parser for convering binaries to digits
# PARSE_BOOL=util/util_parsebools.java
# PARSE_BOOL=util_parsebools
PARSE_OUT=OUTPUT_formatted.cex
# echo "---QUABS solving---"
echo "solving QBF..."
  ${QUABS}  --statistics --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
#  ${QUABS} --statistics --preprocessing 0 --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
