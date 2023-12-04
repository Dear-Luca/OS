#!/bin/bash

VAR=`./lanciaeprendipid.sh`

for process in ${VAR}; do
      kill ${process}
done

