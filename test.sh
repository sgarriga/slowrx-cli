#!/bin/bash
for zipfile in TESTDATA/*.gz; do
    testfile=$(basename ${zipfile} .wav.gz)
    echo "Testing ${zipfile} -> ${testfile}.wav -> ${testfile}.bmp"
    gunzip ${zipfile}
    ./slowrx-cli  -o TESTDATA/${testfile}.bmp TESTDATA/${testfile}.wav
    gzip TESTDATA/${testfile}.wav
done