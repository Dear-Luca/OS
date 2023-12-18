#!/bin/bash

INDEX=1
COUNT_PARI=0
COUNT_DISPARI=0

for name in $@; do
    if [[ -e ${name} ]]; then
        LINES=`cat $name | wc -l`
        if (( ${INDEX} % 2 == 0 )); then
            (( COUNT_PARI=${COUNT_PARI}+${LINES} ))
        else
            (( COUNT_DISPARI=${COUNT_DISPARI}+${LINES} ))
        fi
    fi
    (( INDEX=$INDEX+1 ))
done

echo $COUNT_PARI
echo $COUNT_DISPARI 1>&2