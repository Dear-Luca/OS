#!/bin/bash

for name in `find ~/Documents/UNI/OS/C/thread/exam_exercises/piattello/ -name "*p*.c" -maxdepth 1`;do
    VAR=`cat $name | wc -l`
    if (( $VAR>=1 && $VAR<=100 )); then
        tail -n 1 $name | wc -c
    fi
done
