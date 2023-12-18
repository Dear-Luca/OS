#!/bin/bash

FILES=`find ~/Documents/UNI/OS/C/thread/exam_exercises -name  "*.c" -mindepth 2`

for RECENTE in ${FILES}; do
    break
done

for NAME in ${FILES}; do
    if [[ $NAME -nt $RECENTE ]]; then
        RECENTE=$NAME
    fi
done

echo $RECENTE
