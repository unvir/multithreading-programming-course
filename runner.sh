#!/bin/bash

LOOP_COUNT=10
scripts=( main_input omp_main_input main_input2 omp_main_input2 main_input3 omp_main_input3 )
omp_scripts=( omp_main_input omp_main_input2 omp_main_input3 )
threads=(2 4 6 10)

export OMP_NUM_THREADS=1

for src in "${scripts[@]}"
do
 echo "1 threads $src:"
 for (( i = 0; i < $LOOP_COUNT; i++ ))
  do
   ./$src | awk '{print $2}'
  done
done

for threads_count in "${threads[@]}"
do
 export OMP_NUM_THREADS=$threads_count
 for src in "${omp_scripts[@]}"
 do
  echo "$threads_count threads $src:"
  for (( i = 0; i < $LOOP_COUNT; i++ ))
   do
    ./$src | awk '{print $2}'
   done
 done
done


