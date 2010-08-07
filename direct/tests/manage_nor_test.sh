#! /bin/sh

dir_id=-none
[ -z $1 ] || dir_id=$1


iterations=100000

[ -z $2 ]  || iterations=$2

STARTDIR=`pwd`
RUNDIR=`pwd`/tmp/nor-$dir_id
mkdir $RUNDIR
cd $RUNDIR
cp $STARTDIR/*sh .
ln -s $STARTDIR/yaffs_test yaffs_test

./init_fw_update_test_nor.sh
./run_fw_update_test_nor.sh $iterations

echo "!!!!!!!!!!!"
echo "Tests done"
while true
do
sleep 10
done
