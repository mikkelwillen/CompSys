#!/bin/bash

### USAGE
# All test must x86prime files (*.prime), located in tests
# All test must start with a label "test:" from which the test start
#   You can insert a "jmp myprogram" as the first instruction, if your program starts elsewhere

# Exit immediately if any command below fails.
set -e

# Ensure latest version
make

# The command with which you run PRUN: You should likely change this variable
PRUN=prun

# The command with which you run PRASM: You should likely change this variable
PRASM=prasm

TESTLOC=tests
TESTDIR=test_runs

if [[ ! -d "${TESTLOC}" ]] ; then
  echo "Cannot find the test location ${TESTLOC}"
  exit
fi

echo ">> I will now test all *.prime files in ${TESTLOC}/"

echo "Generating a test_runs directory.."
mkdir -p $TESTDIR
rm -f $TESTDIR/*

echo "Running the tests.."
exitcode=0

for f in tests/*.prime; do
  filename=${f#"$TESTLOC/"}
  fname=${filename%.prime}

  echo ">>> Testing ${filename}.."

  # Generate hex file
  hexfile=$TESTDIR/$fname.hex
  $PRASM $f
  mv $TESTLOC/$fname.hex $TESTDIR/
  mv $TESTLOC/$fname.sym $TESTDIR/

  # Generate trace file
  tracefile=$TESTDIR/$fname.trc
  $PRUN $hexfile test -tracefile $tracefile

  set +e # Continue after failed trace
  ./sim $hexfile 0x0 $tracefile > $TESTDIR/$fname.out
  set -e

  if [ ! $(grep Done $TESTDIR/$fname.out) ]
  then
    echo "Failed :-("
    echo "For details, see $TESTDIR/$fname.out"
    exitcode=1
  else
    echo "Success :-)"
  fi
done

exit $exitcode
