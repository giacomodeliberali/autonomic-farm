#!/bin/bash

# Build the source
make prod 

# Empty all CSV files
rm -f csv/*

# The number of CSV generated for each input
runs_number=2
expected_troughput=20
initial_nw=1

echo "Running with $initial_nw nw and $expected_troughput throughput"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-default-$i-$initial_nw-$expected_troughput.csv..."
		./farm.out $initial_nw $expected_troughput > csv/srv-default-$i-$initial_nw-$expected_troughput.csv
	done

for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-constant-$i-$initial_nw-$expected_troughput.csv..."
		./farm.out $initial_nw $expected_troughput 2 > csv/srv-constant-$i-$initial_nw-$expected_troughput.csv
	done