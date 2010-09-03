#! /bin/bash

TESTDIR=/mnt

echo "start"
echo " "
cat /proc/yaffs

for(( i=0; i < 32; i++ )); do
dd of=$TESTDIR/$i if=/dev/urandom bs=1024 count=1024
done

echo "32 x 1MB written"
cat /proc/yaffs
for(( i=0; i < 32; i++ )); do
dd of=$TESTDIR/xx if=/dev/urandom bs=1024 count=1024
done

echo "32 x 1MB overwites"
cat /proc/yaffs


