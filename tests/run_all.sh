#!/bin/bash

for ds in $(ls datasets); do
    echo "***** DATASET $ds *****"
    cd datasets/$ds
    for q in $(ls queries); do
        n=${q%%-*}
        echo -n "running $q: "
        diff=$(diff -B <(../../../testexe 100000000 1 < queries/$q) results/$n)
        if [[ $diff ]]; then
            echo FAILURE
            echo "$diff"
        else
            echo OK
        fi
    done
    cd ..
done
