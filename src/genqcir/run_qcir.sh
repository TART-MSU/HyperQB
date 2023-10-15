#!/bin/sh

K=5

cd ..

cp build_today/HQ_old.qcir gen_qcir/check/AB.qcir

cd gen_qcir

PROJ='check'
python3 extract.py ${PROJ} T F; 
python3 genqcir.py ${PROJ} ${K} T F;

time /Users/tzuhan/install/quabs/quabs --statistics ${PROJ}/EAencoding.qcir; 
echo "File size: "
stat -f%z ${PROJ}/EAencoding.qcir
