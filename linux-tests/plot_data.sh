#!/bin/sh
# Script that gathers data erased vs free data from /proc/yaffs_stats and simultaneously \
# plots it using gnuplot.


#Gather settings
log_file=data
gather_delay=1

done_file=plot_done

# Plot settings
trunc_file=trunc_data
plot_samples=1000
plot_delay=2





# Gathering task

gather_data() {
i=0;
rm -f $log_file

while [ ! -e $done_file ] ; do
str=$(cat /proc/yaffs_stats)
echo "$i, $str" 
echo "$i, $str"  >> $log_file
i=$(($i+1))
sleep $gather_delay
done
}


# Plotting task
# Periodically creates a truncated version of the log file and
# outputs commands into gnuplot, thus driving gnuplot

drive_gnuplot(){
sleep 5
tail -$plot_samples $log_file > $trunc_file

plot_str=" plot '$trunc_file' using 1:3 with linespoints title 'free', '' using 1:4 with linespoints title 'erased'"

echo "set title 'yaffs free space vs erased space'"
echo "set xlabel 'seconds'"
echo "set ylabel 'chunks'"


echo $plot_str
 
while [ ! -e $done_file ]; do
sleep $plot_delay
tail -$plot_samples $log_file > $trunc_file
echo replot
done
}


rm -f $done_file
trap "touch $done_file" INT

echo "Start gathering task in background"
gather_data &
echo "Run plotting task"
drive_gnuplot | gnuplot

wait

echo "All done"

