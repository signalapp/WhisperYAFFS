#! /bin/sh
set -x
LAUNCHDIR=`pwd`
RUNDIR=`pwd`/tmp
mkdir $RUNDIR
sudo mount -t tmpfs none $RUNDIR
sudo chmod a+wr $RUNDIR



xterm  -e "$LAUNCHDIR/manage_nor_test.sh  0"&
xterm  -e "$LAUNCHDIR/manage_nand_test.sh 0"&
xterm  -e "$LAUNCHDIR/manage_nor_test.sh  1"&
xterm  -e "$LAUNCHDIR/manage_nand_test.sh 1"&
xterm  -e "$LAUNCHDIR/manage_nor_test.sh  2"&
xterm  -e "$LAUNCHDIR/manage_nand_test.sh 2"&
