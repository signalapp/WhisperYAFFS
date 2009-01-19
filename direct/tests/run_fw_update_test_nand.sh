#!/bin/bash


for ((i=0; i < 100000; i++))  
do

   seed=$RANDOM   
   j=$(( $i % 10 ))
   rm seed*$j
   echo $seed>seed-for-run-$i
   rm emfile-2k-0-*$j
   cp emfile-2k-0 emfile-2k-0-$i
   echo "######### Run $i with seed $seed"
   ./yaffs_test -u -f -p -s$seed yaffs2
done
