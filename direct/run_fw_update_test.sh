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
   ./yaffs_test /M18-1 fw_update $seed
done
