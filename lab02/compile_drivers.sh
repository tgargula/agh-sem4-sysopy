#!/bin/bash

command=()
files=()

ctr=0
isArg=1
for arg in $@ ; do
    if [[ $arg == ":" ]]; then
        isArg=0
        ctr=-1
    elif [[ isArg -eq 1 ]]; then
        command[$ctr]=$arg
    else
        files[$ctr]=$arg
    fi
    ctr=$(($ctr + 1))
done

for file in ${files[*]}; do
    ${command[*]} -c src/drivers/${file}.c -o target/out/${file}.o -Dbenchmark -lm
    ${command[*]} -static -o target/bin/${file} target/out/${file}.o -L./target/lib -l_library -lm
done