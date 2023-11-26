echo "called"
echo "le variabili sono state passate ? "
echo "PATH = ${PATH}"
echo "PIPPO = ${PIPPO}"
echo "modifico variabili "
PIPPO="${PIPPO}:MODIFICATO"
PATH="${PATH}:MODIFICATO"
echo “creo nuova variabilie"
NUOVA=NUOVOCONTENUTO
echo "PATH = ${PATH}"
echo "PIPPO = ${PIPPO}"
echo "NUOVA= ${NUOVA}"
echo "termina called“
