echo "called"
echo "le variabili sono state passate?"
echo "PATH=${PATH}"
echo "PIPPO=${PIPPO}"

echo "modifico variabili "
PIPPO="${PIPPO}:MODIFICATO"
PATH="${PATH}:MODIFICATO"
echo "PATH = ${PATH}"
echo "PIPPO = ${PIPPO}"
echo "termina called"

