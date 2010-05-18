#!/bin/sh
# Do some random operations on a yaffs2 file system

MTD="/dev/mtdblock0"
MOUNT_PT="/mnt"
TEST_DIR="$MOUNT_PT/stress_dir"
REF_FILE="$MOUNT_PT/ref.file"
SUM_FILE="$MOUNT_PT/sum.file"
MIN_LOW_WATER=5
MAX_LOW_WATER=85
MIN_HIGH_WATER=15
MAX_HIGH_WATER=95

let low_water=25
let high_water=90
let df_percent=50


update_df_percent(){
	df_percent=$(df | grep $MTD | sed "s_[ %] *_ _g" | cut -d" " -f 5)
}

update_df_percent

calc_low_water() {
   let low_water=0
   while [ $low_water -lt $MIN_LOW_WATER ] ;
   do
   	let low_water=$RANDOM%100
   	while [[ $low_water -ge $df_percent  || $low_water -gt $MAX_LOW_WATER ]] ;
   	do
   		let low_water=$RANDOM%100
   	done
   done
}

calc_high_water() {
   let high_water=100
   while [ $high_water -gt $MAX_HIGH_WATER ] ;
   do
   	let high_water=$RANDOM%100
   	while [[ $high_water -le $df_percent || $high_water -lt $MIN_HIGH_WATER ]] ;
   	do
   		let high_water=$RANDOM%100
   	done
   done
}



maybe_sleep(){
  local val
  let val=$RANDOM%10000
  if [ $val -le 1 ] ; then
	let val=$RANDOM%100
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!                                                              !!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!! S L E E P  $val"
  	echo "!!!!!!!!!                                                              !!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	sleep $val
  	echo " Sleep completed "
  fi
}

maybe_remount(){
  local val
  let val=$RANDOM%10000
  
  if [ $val -le 1 ] ; then
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!                                                              !!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!! R E M O U N T                                                !!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!                                                              !!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  
  	umount $MOUNT_PT
  	mount -t yaffs2 $MTD $MOUNT_PT
  fi

}



make_space_slow(){
   
   update_df_percent
   

   calc_low_water	

   echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
   echo "!!!!! Slowly reduce usage from $df_percent percent to $low_water percent"
   echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
   echo "!!!!!!"
   while [ $df_percent -ge $low_water ] ; do

      DELETE_FILE=`ls $TEST_DIR | head -n 1`
      rm -rf "$TEST_DIR/$DELETE_FILE"

      update_df_percent
      maybe_sleep
      maybe_remount
   done
}

make_space_fast(){
   
   calc_low_water
   update_df_percent

   echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
   echo "!!!!! Quickly reduce usage from $df_percent percent to $low_water percent"
   echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
   echo "!!!!!!"
   while [ $df_percent -ge $low_water ] ; do

      let file_prefix=$RANDOM%100
      rm -rf $TEST_DIR/dummy $TEST_DIR/$file_prefix*

      update_df_percent
      maybe_sleep
      maybe_remount
   done
}

make_space(){
   local xx
   let xx=$RANDOM%100
   update_df_percent

   if [ $df_percent -gt $MIN_LOW_WATER ] ; then

	   if [ $xx -lt 50 ] ; then
   		make_space_slow
	   else
	   	make_space_fast
	   fi
  fi
}

periodic_action(){

  maybe_sleep
  maybe_remount
}

random_number(){
 local val
 val=$RANDOM
 echo $val
}


drop_caches(){
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	echo "!!!!!!!!!!! D R O P   C A C H E S"
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	sync
	echo 3 > /proc/sys/vm/drop_caches
}

maybe_drop_caches(){
	local x
	let x=$RANDOM%10000
	if [ $x -eq 99 ] ; then
		drop_caches
	fi
}

create_files(){
   
   update_df_percent
   
   if [ $df_percent -lt $MAX_HIGH_WATER ] ; then
   
	   calc_high_water
	   
	   rm -f $SUM_FILE
	   
	   for i in $(find $TEST_DIR) ; do
	   	[ -f $i ] && md5sum $i >> $SUM_FILE
	   done

	   echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	   echo "!!!!! Fill from $df_percent percent to $high_water percent"
	   echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	   let chunks=$RANDOM%100

	
	   update_df_percent
	   
	   let skip_file_create=0
	   let skip_dir_create=0

	   while [ $df_percent -le $high_water ] ; do
	   
	        if [ $skip_file_create -lt 1 ] ; then
			let fsize=$RANDOM*$chunks+1
			echo "!!!!!! File size is $fsize"
	     		dd if=/dev/urandom of=$REF_FILE bs=$fsize count=1
	   		REF_SUM=$(md5sum $REF_FILE | cut -f1 -d" " )
	   		let skip_file_create=$RANDOM%50
	   	else
	   		let skip_file_create=$skip_file_create-1
	        fi

	        if [ $skip_dir_create -lt 1 ] ; then
	        	dir_name=$TEST_DIR/$RANDOM-dir
	        	mkdir -p $dir_name
	        	echo "Creating in directory $dir_name"
	   		let skip_dir_create=$RANDOM%100
	   	else
	   		let skip_dir_create=$skip_dir_create-1
	        fi
	   
		FNAME=$dir_name/$RANDOM-$RANDOM-$RANDOM
		cp $REF_FILE $FNAME
		FILE_SUM=$(md5sum $FNAME | cut -f1 -d" ")
		md5sum $FNAME >> $SUM_FILE

		if [ "$FILE_SUM" != "$REF_SUM" ] ; then
			echo "File comparison failed $REF_SUM - $FILE_SUM"
			exit 1
		fi

		update_df_percent
		maybe_sleep
		maybe_remount
		maybe_drop_caches
	   done

	   if ! md5sum -c $SUM_FILE > /dev/null ; then
	   	echo "File verification failed"
	   	exit 1
	   fi
   fi
	
}

fill_disk(){
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	echo "!!!!!!!!!!! F I L L   D I S K"
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	let full=0
	while [ $full -eq 0 ] ; do
		dd if=/dev/urandom of=$TEST_DIR/$RANDOM-$RANDOM-fill bs=1000 count=50 2> /dev/null || let full=1 || true
	done
		
}

rm -rf $TEST_DIR/*
mkdir -p $TEST_DIR
touch $TEST_DIR/dont-want-empty-dir
create_files

while true ; do

	let x=$RANDOM%100
	if [ $x -lt 50 ] ; then
		create_files
	fi

	let x=$RANDOM%100
	if [ $x -lt 50 ] ; then
		make_space
	fi

	let x=$RANDOM%100
	if [ $x -lt 5 ] ; then 
		fill_disk
	fi

	let x=$RANDOM%100
	if [ $x -lt 1 ] ; then
		drop_caches
	fi
done
