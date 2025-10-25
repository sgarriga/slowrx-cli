#!/bin/bash
testfile=$(basename ${1} .wav.gz)
echo "Testing ${1} -> ${testfile}.bmp"
# create a copy to not have to rezip and cause a git diff
cp ${1} TESTDATA/_${testfile}.wav.gz
gunzip TESTDATA/_${testfile}.wav.gz
./slowrx-cli  -o TESTDATA/${testfile}.bmp TESTDATA/_${testfile}.wav
rm TESTDATA/_${testfile}.wav
