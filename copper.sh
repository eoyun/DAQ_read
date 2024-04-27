#!/bin/bash

mid=8


for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5858)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5866)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5874)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5882)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5890)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5898)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5906)),$mid,$ch)"
done

for ch in {1..8}
do 
	root -l -b -q copper_mindiff.cc"($(($ch + 5914)),$mid,$ch)"
done

