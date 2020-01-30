#!/bin/bash

# Build the source
make prod 

# The number of CSV generated for each input
runs_number=5

# The initial number of workers
initial_nw=1

# The troughput
expected_troughput=30

echo "Input: Default"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-default-$i.csv..."
   		./farm.out $initial_nw $expected_troughput > csv/srv-default-$i.csv
	done

echo "Input: Constant"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-constant-$i.csv..."
   		./farm.out $initial_nw $expected_troughput 2 > csv/srv-constant-$i.csv
	done

echo "Input: ReverseDefault"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-reverse-default-$i.csv..."
   		./farm.out $initial_nw $expected_troughput 3 > csv/srv-reverse-default-$i.csv
	done

echo "Input: LowHigh"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-lowhigh-$i.csv..."
   		./farm.out $initial_nw $expected_troughput 4 > csv/srv-lowhigh-$i.csv
	done

echo "Input: HighLow"
for (( i=1; i<=$runs_number; i++ ))
	do  
		echo "srv-highlow-$i.csv..."
   		./farm.out $initial_nw $expected_troughput 5 > csv/srv-highlow-$i.csv
	done