#!/usr/bin/python
from __future__ import print_function
import RPi.GPIO as GPIO
import datetime
import time
import sys

def my_callback(channel):
    if GPIO.input(7) == GPIO.HIGH:
        print('\n FLOAT at ' + str(datetime.datetime.now()))
        log = open("/home/pi/data/current_water_level.txt", "w")
        print (('OK | Last flipped at ' + str(datetime.datetime.now())), file = log)
    else:
        print('\n SINK at ' + str(datetime.datetime.now()))
        log = open("/home/pi/data/current_water_level.txt", "w")
        print (('Low | Last flipped at ' + str(datetime.datetime.now())), file = log)

GPIO.setmode(GPIO.BCM)
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.add_event_detect(7, GPIO.BOTH, callback=my_callback)

print ("Listening for float changes")

try:
        while True:
         time.sleep(0.5)

except KeyboardInterrupt:
    GPIO.cleanup()       # clean up GPIO on CTRL+C exit
GPIO.cleanup()           # clean up GPIO on normal exit
