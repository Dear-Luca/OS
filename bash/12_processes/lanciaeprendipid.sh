#!/bin/bash

for (( i=0; $i<10; i=$i+1 )); do
      ./puntini.sh 10 1>&2 &
      echo -n " $!";
done  
