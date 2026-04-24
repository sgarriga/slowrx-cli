#!/bin/bash
for zipfile in TESTDATA/${1}*.gz; do
    ./test1.sh ${zipfile}
done
