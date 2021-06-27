#!/bin/bash

while [[ 1 ]]; do
    ls -l /dev/mqueue | grep ^-
    sleep 0.1
    clear
done