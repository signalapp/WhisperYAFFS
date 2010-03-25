#! /bin/sh

dir_id=-none
[ -z $1 ] || dir_id=$1

RUNDIR=`pwd`/tmpnand$dir_id
mkdir $RUNDIR
sudo mount -t tmpfs none $RUNDIR
sudo chmod a+wr $RUNDIR
cd $RUNDIR
cp ../*sh .
ln -s ../yaffs_test yaffs_test

./init_fw_update_test_nand.sh
./run_fw_update_test_nand.sh

