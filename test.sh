#!/bin/bash
for zipfile in TESTDATA/*.gz; do
    testfile=$(basename ${zipfile} .wav.gz)
    echo "Testing ${zipfile} -> ${testfile}.bmp"
    # create a copy to not have to rezip and cause a git diff
    cp ${zipfile} TESTDATA/_${testfile}.wav.gz
    gunzip TESTDATA/_${testfile}.wav.gz
    ./slowrx-cli  -o TESTDATA/${testfile}.bmp TESTDATA/_${testfile}.wav
    rm TESTDATA/_${testfile}.wav
done