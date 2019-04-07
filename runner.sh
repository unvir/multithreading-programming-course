#!/bin/bash

LOOP_COUNT=10
scripts=( main_input omp_main_input main_input2 omp_main_input2 main_input3 omp_main_input3 )

for src in "${scripts[@]}"
do
 echo "$src:"
 for (( i = 0; i < $LOOP_COUNT; i++ ))
  do
   ./$src | awk '{print $2}'
  done
done
