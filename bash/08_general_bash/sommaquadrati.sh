#!/bin/bash

ARG=$#
NUM=0
for((i=1; $i<=$ARG; i=$i+1));do
      ((NUM=$NUM+${!i}*${!i}));
done

for((i=1; $i<=$ARG; i=$i+1));do
      ((NUM=$NUM-${i}));
done

echo ${NUM}
