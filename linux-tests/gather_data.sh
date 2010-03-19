#! /bin/sh
# gather_data.sh  agthers the data to be plotted
#
the_file=data
i=0;
rm -f $the_file

while true; do
str=$(cat /proc/yaffs_debug)
echo "$i, $str" 
echo "$i, $str"  >> $the_file
let i=$i+1
sleep 1
done
