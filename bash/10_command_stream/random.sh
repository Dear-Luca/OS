#!/bin/bash

COUNT=0
while (( ${RANDOM}%10!=2 )); do
      ((COUNT=${COUNT}+1))
done
echo ${COUNT}



