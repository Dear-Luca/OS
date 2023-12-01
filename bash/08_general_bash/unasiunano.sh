#/bin/bash
FLAG=1
while read RIGA; do
      if ((${FLAG}==1)); then
            echo ${RIGA}
            FLAG=0
      else
            FLAG=1
      fi;
done

