#!/bin/bash

ARG=$#
STRING=""

for((i=2; $i<=$ARG; i=$i+2)); do
      STRING=${STRING}${!i};
done

for((i=1; $i<=$ARG; i=$i+2)); do
      STRING=${STRING}${!i};
done

echo ${STRING}

