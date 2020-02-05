#!/bin/bash

# Build the source
make prodff 

# Empty all CSV files
rm -f csv/*

# The number of CSV generated for each input
runs_number=2
expected_troughput=20
initial_nw=128

echo "Running FastFlow with $initial_nw nw and $expected_troughput throughput"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-ff-default-$i-$initial_nw-$expected_troughput.csv..."
		./farm-ff.out $initial_nw $expected_troughput > csv/srv-ff-default-$i-$initial_nw-$expected_troughput.csv
	done

for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-ff-constant-$i-$initial_nw-$expected_troughput.csv..."
		./farm-ff.out $initial_nw $expected_troughput 2 > csv/srv-ff-constant-$i-$initial_nw-$expected_troughput.csv
	done