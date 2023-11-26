echo "caller"
PIPPO=ALFA
echo "PATH=${PATH}"
echo "PIPPO=${PIPPO}"
echo "calling subshell"

./var_called.sh
echo "dentro var_caller"
echo "le variabili sono state modificate?"
echo "PATH=${PATH}"
echo "PIPPO=${PIPPO}"

