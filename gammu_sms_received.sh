#!/bin/sh
PROGRAM=/home/pi/controller/controller
for i in `seq $SMS_MESSAGES` ; do
    eval "$PROGRAM --\"\${SMS_${i}_TEXT}\""
done
