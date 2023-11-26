#!/bin/bash
NUM=$1
if (( ${NUM}>1 )); then 
      (( RIS=${RIS}*${NUM} ))
      source ./fattoriale1.sh $((${NUM}-1))
fi


