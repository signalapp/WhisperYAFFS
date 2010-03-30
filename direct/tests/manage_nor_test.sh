#! /bin/sh

dir_id=-none
[ -z $1 ] || dir_id=$1

STARTDIR=`pwd`
RUNDIR=`pwd`/tmp/nor-$dir_id
mkdir $RUNDIR
cd $RUNDIR
cp $STARTDIR/*sh .
ln -s $STARTDIR/yaffs_test yaffs_test

./init_fw_update_test_nor.sh
./run_fw_update_test_nor.sh

