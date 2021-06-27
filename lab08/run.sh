#!/bin/bash

data=`ls data | grep .ascii.pgm$`

for each in $data; do
    convert data/$each data/${each:0:-10}.png
    for threads_no in `echo 1 2 4 8 16`; do
        for method in `echo numbers block`; do
            ./target/negate $threads_no $method data/$each out/$each
        done
    done
    convert out/$each out/${each:0:-10}.png
done
