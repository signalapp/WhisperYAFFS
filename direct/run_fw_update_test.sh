#!/bin/bash


for ((i=0; i < 100000; i++))  
do
   echo $i > cycle.count
   j=$(( $i % 10 ))
   
   rm data*$j
   cp ynorsimdata data$i
   ./yaffs_test /M18-1 fw_update
done
