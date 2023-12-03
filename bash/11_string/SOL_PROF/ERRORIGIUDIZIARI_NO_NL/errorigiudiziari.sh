#!/bin/bash

while
	read IDPROCESSO DESCRIZIONEVERDETTO;
	#condizione vera se IDPROCESSO e DESCRIZIONEVERDETTO sono non nulle
      [[ -n ${IDPROCESSO} && -n ${DESCRIZIONEVERDETTO} ]] ;
do
	while 
		read IDDENUNCIA idprocesso ;
		[[ -n ${IDDENUNCIA} && -n ${idprocesso} ]] ;
	do
		if [[ ${IDPROCESSO} == ${idprocesso} ]] ; then
			while
				read NOME COGNOME iddenuncia DESCRIZIONEREATO ;
				[[ -n ${NOME} && -n ${COGNOME} &&
				   -n ${iddenuncia} && -n ${DESCRIZIONEREATO}
				]] ;
			do
				if [[ ${IDDENUNCIA} == ${iddenuncia} ]] ; then
					echo ${NOME} ${COGNOME} ${DESCRIZIONEREATO} ${DESCRIZIONEVERDETTO}
					break
				fi
			done < denunce.txt
			break
		fi
		done < processi.txt
done < verdetti.txt

