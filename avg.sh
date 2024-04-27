#!/bin/bash

mid=7


for ch in {1..5}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 19)),$mid,$ch)"
done

for ch in {6..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 20)),$mid,$ch)"
done

for ch in {1..1}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 28)),$mid,$ch)"
done

for ch in {2..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 31)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 41)),$mid,$ch)"
done

for ch in {1..3}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 49)),$mid,$ch)"
done

for ch in {4..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 51)),$mid,$ch)"
done

for ch in {1..2}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 61)),$mid,$ch)"
done

for ch in {3..3}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 62)),$mid,$ch)"
done

for ch in {4..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 67)),$mid,$ch)"
done

for ch in {1..2}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 75)),$mid,$ch)"
done

for ch in {3..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 77)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 85)),$mid,$ch)"
done

for ch in {1..6}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 93)),$mid,$ch)"
done


for ch in {7..8}
do 
	root -l -b -q peakADC_S.cc"($(($ch + 94)),$mid,$ch)"
done

