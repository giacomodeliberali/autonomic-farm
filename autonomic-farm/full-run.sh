#!/bin/bash

# Build the source
make prod 

# Empty all CSV files
rm -f csv/*

# The number of CSV generated for each input
runs_number=2


for expected_troughput in 10 20
	do
		for initial_nw in 1 32 64 128 256
			do
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

				for (( i=1; i<=$runs_number; i++ ))
					do  
						echo "srv-reverse-default-$i-$initial_nw-$expected_troughput.csv..."
						./farm.out $initial_nw $expected_troughput 3 > csv/srv-reverse-default-$i-$initial_nw-$expected_troughput.csv
					done

				for (( i=1; i<=$runs_number; i++ ))
					do  
						echo "srv-lowhigh-$i-$initial_nw-$expected_troughput.csv..."
						./farm.out $initial_nw $expected_troughput 4 > csv/srv-lowhigh-$i-$initial_nw-$expected_troughput.csv
					done

				for (( i=1; i<=$runs_number; i++ ))
					do  
						echo "srv-highlow-$i-$initial_nw-$expected_troughput.csv..."
						./farm.out $initial_nw $expected_troughput 5 > csv/srv-highlow-$i-$initial_nw-$expected_troughput.csv
					done
			done
		done
	