#!/bin/bash

while read ANNO LUOGO MOTIVO DANNI; do
    echo "${MOTIVO} " >> ${MOTIVO}.txt
done < cadutevic.txt

for name in *.txt; do
    if [[ "${name}" != "cadutevic.txt" ]]; then
        COUNT=0
        while read PAROLA; do
            (( COUNT=$COUNT+1 ))
        done < ${name}
        NUOVA=`echo ${name} | sed 's/.\{4\}$//'`
        echo ${NUOVA} ${COUNT};
        rm ${name}
    fi;
done