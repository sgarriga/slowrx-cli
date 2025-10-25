#!/bin/bash
for zipfile in TESTDATA/*.gz; do
    ./test1.sh ${zipfile}
done
