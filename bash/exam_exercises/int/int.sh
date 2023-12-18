#!/bin/bash
FILES=`find ~/Documents/UNI/OS/C/thread/exam_exercises -name "*.h"`
for name in ${FILES}; do
    if [[ -e tmp.txt ]]; then
		rm tmp.txt
	fi
    grep "int" $name > tmp.txt 
    cut -c 1-3 tmp.txt
done
  
