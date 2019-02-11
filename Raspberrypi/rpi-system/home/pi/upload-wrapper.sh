#!/bin/bash

echo "heartbeat" > /sys/class/leds/led0/trigger
/usr/bin/python3 /home/pi/upload.py
echo "none" > /sys/class/leds/led0/trigger
