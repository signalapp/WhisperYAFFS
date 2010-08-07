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

   cycles=$RANDOM
   cycles=$(( $cycles % 10000 ))

   rm -f log-nand-*$j
   rm -f seed-nand-*$j
   echo $seed>seed-nand-for-run-$i

   rm -f emfile-2k-0-*$j
   rm -f emfile-2k-1-*$j
   rm -f emfile-2k-2-*$j
   rm -f emfile-2k-3-*$j

   echo "fuzz"
 
   [ -e emfile-2k-0 ] && ./fuzzer -p 10000 emfile-2k-0 
   [ -e emfile-2k-1 ] && ./fuzzer -p 10000 emfile-2k-1 
   [ -e emfile-2k-2 ] && ./fuzzer -p 10000 emfile-2k-2 
   [ -e emfile-2k-3 ] && ./fuzzer -p 10000 emfile-2k-3 

   cp emfile-2k-0 emfile-2k-0-$i
   cp emfile-2k-1 emfile-2k-1-$i
   cp emfile-2k-2 emfile-2k-2-$i
   cp emfile-2k-3 emfile-2k-3-$i

   echo "#########"
   echo "#########"
   echo "#########"
   echo "######### Run $i of $iterations with seed $seed"
   echo "#########"
   echo "#########"
   echo "#########"
   ./yaffs_test -u  -p -s$seed -t 8 -n $cycles -z yaffs2 
done
