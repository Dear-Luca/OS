echo "caller"
# setto la var locale PIPPO
# la var d’ambiente PATH esiste già
PIPPO=ALFA
echo "PATH = ${PATH}"
echo "PIPPO = ${PIPPO}"
echo "calling subshell"
source ./var_called2.sh
echo "ancora dentro caller"
echo "variabili sono state modificate ?"
echo "PATH = ${PATH}"
echo "PIPPO = ${PIPPO}"
echo "NUOVA= ${NUOVA}"
