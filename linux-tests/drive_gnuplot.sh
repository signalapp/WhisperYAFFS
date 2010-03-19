#!/bin/sh
# drive_gnuplot syntehsises the commands for driving gnuplot

the_log_file=data

plot_str=" plot 'trunc_data' using 1:3 with linespoints title 'free', '' using 1:4 with linespoints title 'erased'"

echo "set title 'yaffs free space and erased space'"

echo $plot_str
 
while true; do
sleep 1
tail -100 $the_log_file > trunc_data
echo replot
done

