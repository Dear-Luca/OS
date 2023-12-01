COUNT=0
while read -N 1 A; do
      ((COUNT=$COUNT+1))
done < prova.txt
echo ${COUNT}
