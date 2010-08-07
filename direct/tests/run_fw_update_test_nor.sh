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

   rm seed-nor-*$j
   echo $seed>seed-nor-for-run-$i


   rm emfile-nor-*$j
   cp emfile-nor emfile-nor-$i

   rm log-nor-*$j

   echo "#########"
   echo "#########"
   echo "#########"
   echo "######### Run $i of $iterations with seed $seed"
   echo "#########"
   echo "#########"
   echo "#########"
   ./yaffs_test -u -f -p -s$seed -t0 M18-1 
   #>log-nor-$i
done

