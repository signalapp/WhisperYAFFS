#!/bin/bash


for ((i=0; i < 100000; i++))  
do

   seed=$RANDOM   
   j=$(( $i % 10 ))
   rm seed*$j
   echo $seed>seed$i
   rm data*$j
   cp ynorsimdata data$i
   echo "######### Run $i with seed $seed"
   ./yaffs_test -f -u -p -s$seed M18-1
done
