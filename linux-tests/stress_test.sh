#!/bin/bash
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

low_water=25
high_water=90
df_percent=50


random_number(){
 local val
 val=$RANDOM
 echo $val
}


update_df_percent(){
	df_percent=$(df | grep $MTD | sed "s_[ %] *_ _g" | cut -d" " -f 5)
}

update_df_percent

calc_low_water() {
   low_water=0
   while [ $low_water -lt $MIN_LOW_WATER ] ;
   do
   	low_water=$(($(random_number) % 100))
   	while [[ $low_water -ge $df_percent  || $low_water -gt $MAX_LOW_WATER ]] ;
   	do
   		low_water=$(($(random_number) % 100))
   	done
   done
}

calc_high_water() {
   high_water=100
   while [ $high_water -gt $MAX_HIGH_WATER ] ;
   do
   	high_water=$(($(random_number) % 100))
   	while [[ $high_water -le $df_percent || $high_water -lt $MIN_HIGH_WATER ]] ;
   	do
   		high_water=$(($(random_number) % 100))
   	done
   done
}



maybe_sleep(){
  local val
  val=$(($(random_number) % 10000))
  if [ $val -le 1 ] ; then
	val=$(($(random_number) % 100))
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

remount_count=0
maybe_remount_count=0

maybe_remount(){
  local val
  val=$(($(random_number) % 10000))
  maybe_remount_count=$(($maybe_remount_count+1))
  
  if [ $val -le 1 ] ; then
	remount_count=$(($remount_count+1))
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!!                                                              !!!!!!!!!!!!!!!!!!!"
  	echo "!!!!!!!!! R E M O U N T $remount_count from $maybe_remount_count"
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

      file_prefix=$(($(random_number) % 100))
      rm -rf $TEST_DIR/dummy $TEST_DIR/$file_prefix*

      update_df_percent
      maybe_sleep
      maybe_remount
   done
}

make_space(){
   local xx
   xx=$(($(random_number) % 100))
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



drop_caches(){
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	echo "!!!!!!!!!!! D R O P   C A C H E S"
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	sync
	echo 3 > /proc/sys/vm/drop_caches
}

maybe_drop_caches(){
	local x
	x=$(($(random_number) % 10000))
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
	   chunks=$(($(random_number) % 100))

	
	   update_df_percent
	   
	   skip_file_create=0
	   skip_dir_create=0

	   while [ $df_percent -le $high_water ] ; do
	   
	        if [ $skip_file_create -lt 1 ] ; then
			fsize=$(($(random_number)*$chunks+1))
			echo "!!!!!! File size is $fsize"
	     		dd if=/dev/urandom of=$REF_FILE bs=$fsize count=1
	   		REF_SUM=$(md5sum $REF_FILE | cut -f1 -d" " )
	   		skip_file_create=$(($(random_number) % 50))
	   	else
	   		skip_file_create=$(($skip_file_create-1))
	        fi

	        if [ $skip_dir_create -lt 1 ] ; then
	        	dir_name=$TEST_DIR/$(random_number)-dir
	        	mkdir -p $dir_name
	        	echo "Creating in directory $dir_name"
	   		skip_dir_create=$(($(random_number) % 100))
	   	else
	   		skip_dir_create=$(($skip_dir_create-1))
	        fi
	   
		FNAME=$dir_name/$(random_number)-$(random_number)-$(random_number)
		cp $REF_FILE $FNAME
		FILE_SUM=$(md5sum $FNAME | cut -f1 -d" ")
		md5sum $FNAME >> $SUM_FILE

		if [ "$FILE_SUM" != "$REF_SUM" ] ; then
			echo "File comparison between $REF_FILE and $FNAME failed $REF_SUM - $FILE_SUM"
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
	full=0
	while [ $full -eq 0 ] ; do
		dd if=/dev/urandom of=$TEST_DIR/$(random_number)-$(random_number)-fill bs=1000 count=50 2> /dev/null || full=1 || true
	done
		
}

rm -rf $TEST_DIR/*
mkdir -p $TEST_DIR
touch $TEST_DIR/dont-want-empty-dir
create_files

while true ; do

	x=$(($(random_number) % 100))
	if [ $x -lt 50 ] ; then
		create_files
	fi

	x=$(($(random_number) % 100))
	if [ $x -lt 50 ] ; then
		make_space
	fi

	x=$(($(random_number) % 100))
	if [ $x -lt 5 ] ; then 
		fill_disk
	fi

	x=$(($(random_number) % 100))
	if [ $x -lt 1 ] ; then
		drop_caches
	fi
done
