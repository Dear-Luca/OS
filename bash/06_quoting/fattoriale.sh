COUNT=1
for((i=$1; $i>0; i=$i-1)); do
    ((  COUNT=${COUNT}*${i} ));
done
echo $COUNT
