#!/bin/sh

MNTPT=/mnt
N=5000
the_file=`pwd`\/data
rm $the_file

let i=0
for j in $(seq 1 5000)
do
	echo "##"
	echo "##"
	echo "## Mount $j of $N"
	echo "##"
	echo "##"

	df
	echo ###"
	mount -t yaffs2 /dev/mtdblock0 $MNTPT
	df
	echo ###"
	cd $MNTPT

	str=$(cat /proc/yaffs_stats)
	echo "$i, $str" 
	echo "$i, $str"  >> $the_file
	let i=$i+1

	sleep 1

	str=$(cat /proc/yaffs_stats)
	echo "$i, $str" 
	echo "$i, $str"  >> $the_file
	let i=$i+1
	
	sleep 1

	~charles/fs-tests/fsx/fsx -N 10000 aa  &

	rm zze

	str=$(cat /proc/yaffs_stats)
	echo "$i, $str" 
	echo "$i, $str"  >> $the_file
	let i=$i+1

	dd if=/dev/urandom of=zza count=4000 &
	dd if=/dev/urandom of=zzb count=4000 &
	dd if=/dev/urandom of=zzc count=100000 &
	dd if=/dev/urandom of=zzd count=4000 &
	dd if=/dev/urandom of=zze count=4000 &
	dd if=/dev/urandom of=zzf count=40000 &

	for k in $(seq 1 10)
	do
		sleep 1

		str=$(cat /proc/yaffs_stats)
		echo "$i, $str" 
		echo "$i, $str"  >> $the_file
		let i=$i+1
	done
	
	echo "waiting..."
	wait
	echo "wait completed"
	
	rm zza

	str=$(cat /proc/yaffs_stats)
	echo "$i, $str" 
	echo "$i, $str"  >> $the_file
	let i=$i+1

	rm zzb
	str=$(cat /proc/yaffs_stats)
	echo "$i, $str" 
	echo "$i, $str"  >> $the_file
	let i=$i+1
	rm zzc
	str=$(cat /proc/yaffs_stats)
	echo "$i, $str" 
	echo "$i, $str"  >> $the_file
	let i=$i+1

	cd -
	umount $MNTPT
	
done
