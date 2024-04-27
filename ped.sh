#!/bin/bash

mid=7
ch=17

for runnum in {5391..5403}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

ch=18

for runnum in {5405..5417}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

ch=19

for runnum in {5419..5432}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

ch=20

for runnum in {5434..5446}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

ch=21

for runnum in {5448..5460}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done


ch=22

for runnum in {5462..5474}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

ch=23

for runnum in {5476..5488}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done

ch=24

for runnum in {5490..5502}
do 
	root -l -b -q intADC.cc"($runnum,$mid,$ch)"
done
