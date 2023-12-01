#!/bin/bash

OUT=""
while read A B C; do
      OUT=${OUT}${B}
done < $1
echo ${OUT}





