#!/bin/bash

file="/home/pi/last_online"
ping -q -c1 bhive.informatik.uni-bremen.de
status=$?
if [ "$status" -ne 0 ] && [ "$2" = "down" ] && [ ! -f $file ]; then
	echo "none" > /sys/class/leds/led0/trigger
	echo "$(date +%s)" > $file
fi

if [ "$status" -eq 0 ] && [ "$2" = "up" ] && [ -f $file ]; then
	rm $file
	bash /home/pi/upload-wrapper.sh &
fi
