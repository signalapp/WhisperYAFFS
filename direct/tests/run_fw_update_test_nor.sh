#!/bin/bash


for ((i=0; i < 100000; i++))  
do

   seed=$RANDOM   
   j=$(( $i % 10 ))
   rm seed*$j
   echo $seed>seed-for-run-$i
   rm data*$j
   cp emfile-nor data-for-run-$i
   echo "######### Run $i with seed $seed"
   ./yaffs_test -f -u -p -s$seed M18-1
done
