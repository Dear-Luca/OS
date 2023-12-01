#!/bin/bash
LENGTH=0
COUNT=0
for name in `ls`;do
      if [[ -d ${name} ]]; then
            LENGTH=$((${LENGTH} + ${#name} ))
      else
            ((COUNT=${COUNT}+1))
      fi;
done

echo "la somma delle lunghezze dei nomi delle directory e': ${LENGTH}"
echo "il numero dei file e': ${COUNT} "

