#!/bin/bash

iterations=100000

[ -z $1 ]  || iterations=$1


rm iteration-max-*
touch iteration-max-$iterations

echo " Running $iterations iterations"
sleep 2

for ((i=0; i < $iterations; i++))  
do

   seed=$RANDOM   
   j=$(( $i % 10 ))
   rm seed-nand-*$j
   echo $seed>seed-nand-for-run-$i
   rm emfile-2k-0-*$j
   cp emfile-2k-0 emfile-2k-0-$i
   echo "#########"
   echo "#########"
   echo "#########"
   echo "######### Run $i of $iterations with seed $seed"
   echo "#########"
   echo "#########"
   echo "#########"
   ./yaffs_test -u -f -p -s$seed yaffs2
done
