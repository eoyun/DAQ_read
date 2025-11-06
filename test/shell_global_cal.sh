#!/bin/bash

for i in $(seq 1 1 100); do
    ./global_calib_add_program 11537 9 6 $i
    ./global_calib_add_program 11538 9 4 $i
    ./pt_test_glocal_add_program 11537 9 6 $i
    ./pt_test_glocal_add_program 11538 9 4 $i
done

