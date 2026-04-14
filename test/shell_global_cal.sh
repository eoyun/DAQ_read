#!/bin/bash

for i in $(seq 1000 1000 100000); do
    ./global_calib_program 11537 9 6 $i
    ./global_calib_program 11538 9 4 $i
    ./pt_test_glocal_program 11537 9 6 $i
    ./pt_test_glocal_program 11538 9 4 $i
done

