#!/bin/bash
for letter in {c..g}; do
      for name in /usr/include/?${letter}*; do
            if [[ -e ${name} && (${#name} -lt 18 || ${#name} -gt 23) ]]; then
                echo "${name}"
            fi
      done
done 

