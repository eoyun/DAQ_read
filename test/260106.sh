#!/bin/bash

./global_calib_add_program 11538 9 4 100000
./global_calib_add_program 11537 9 6 100000
./global_calib_add_program 11544 9 12 100000
./global_calib_add_program 11543 9 14 100000
./pt_test_glocal_program 11538 9 4 100000
./pt_test_glocal_program 11537 9 6 100000
./pt_test_glocal_program 11544 9 12 100000
./pt_test_glocal_program 11543 9 14 100000
./time_calib_program 11546 9 4 6
./time_calib_drs_program 11545 9 4 12 6 14
