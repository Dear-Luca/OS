
FILEAPPOGGIO=temp.txt

while read DATA LUOGO MOTIVO DANNI; do
    NUM=`grep ${MOTIVO} cadutevic.txt | wc -l`
    echo "${MOTIVO} ${NUM}" >> ${FILEAPPOGGIO}
done < cadutevic.txt

cat ${FILEAPPOGGIO}
rm ${FILEAPPOGGIO}