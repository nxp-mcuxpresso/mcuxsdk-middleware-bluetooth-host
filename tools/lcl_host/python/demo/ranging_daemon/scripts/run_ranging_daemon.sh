#!/bin/sh
#PASSPHRASE=`cat /proc/cpuinfo |grep Serial|cut -d' ' -f2`
#/home/pi/encmount.sh -m -u pi -p $PASSPHRASE /home/pi/ranging/python 

## Set the CPU scaling governor to performance
echo -n performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

python /home/pi/ranging/python/demo/ranging_daemon/ranging_daemon.py
