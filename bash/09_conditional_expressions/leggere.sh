#!/bin/bash
while read A B C D; do
      if(($?==0)); then
             echo ${C}
      fi
done < prova.txt

