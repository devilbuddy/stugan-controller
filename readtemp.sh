#!/bin/bash
temp=$(cat /sys/bus/w1/devices/28-03146596e8ff/w1_slave | grep  -E -o ".{0,0}t=.{0,5}" | cut -c 3-)
echo "scale=2;$temp/1000"|bc
