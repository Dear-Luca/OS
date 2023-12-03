#!/bin/bash
while read IDPROCESSO DESCRIZIONEVERDETTO ; do
	while read IDDENUNCIA idprocesso ; do
		if [[ ${IDPROCESSO} == ${idprocesso} ]] ; then
			while read NOME COGNOME iddenuncia DESCRIZIONEREATO ; do
				if [[ ${IDDENUNCIA} == ${iddenuncia} ]] ; then
					echo ${NOME} ${COGNOME} ${DESCRIZIONEREATO} ${DESCRIZIONEVERDETTO}
					break
				fi
			done < denunce.txt
			break
		fi
		done < processi.txt
done < verdetti.txt

