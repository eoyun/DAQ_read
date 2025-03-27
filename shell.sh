#!/bin/bash

mid=11
ch=3
risetime=50
width=200

for runnum in {10993..10999}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
	#root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

#ch=12

#for runnum in {9453..9483}
#do 
	#root -l -b -q intADC_fast.cc"($runnum,$mid,$ch,$risetime,$width)"
#	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
#done
