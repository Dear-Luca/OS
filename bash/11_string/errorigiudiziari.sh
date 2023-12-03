#!/bin/bash

while read ID_PROCESSO ASSOLTO; do
      while read ID_D ID_P; do
            while read NOME COGNOME NUM DENUNCIA; do
                  if [[ ${NUM} == ${ID_D} && ${ID_P} == ${ID_PROCESSO} ]]; then
                        echo ${NOME} ${COGNOME} ${DENUNCIA} ${ASSOLTO}
                  fi
            done < denunce.txt
      done < processi.txt      
done < verdetti.txt

