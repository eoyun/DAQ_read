#!/bin/bash

mid=8
ch=20

for runnum in {5785..5794}
do 
	root -l -b -q peakADC.cc"($runnum,$mid,$ch)"
done

for runnum in {5825..5839}
do 
	root -l -b -q peakADC.cc"($runnum,$mid,$ch)"
done
