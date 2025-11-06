#!/bin/bash

for j in {10135,10503,10505,10507,10509,10555,10696,9540,9541,9679}
do
	for i in {1..20}
	do 
		./ped_program $j $i
	done
done
