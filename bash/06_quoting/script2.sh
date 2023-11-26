NUM=$#
for((i=$NUM; $i>0; i=$i-1));do
      echo ${!i};
done


