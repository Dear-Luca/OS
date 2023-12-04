#!/bin/bash

NUM=$1

while (( $NUM > 0 )); do
      sleep 1
      echo -n ". $$"
      (( NUM=$NUM-1 ))
done

