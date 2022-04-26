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

# echo "---Parse All Binary Numbers---"
echo "\n============ Get Nice-formatted Output if Output is avaialbe ============"

if [ ! -f "$QCIR_OUT" ]; then
    echo "$QCIR_OUT not exists"
    exit 1
fi

echo "parsing into readable format..."
# # echo "---Counterexample Mapping---"
# javac ${MAP}.java
# java ${MAP}.java ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}

# javac ${PARSE_BOOL}.java
# java ${PARSE_BOOL}.java ${MAP_OUT2} ${PARSE_OUT}
${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}
