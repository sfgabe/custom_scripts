#!/usr/bin/env python

import RPi.GPIO as GPIO
import time

servoPIN = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)

p = GPIO.PWM(servoPIN, 50) # GPIO 18 for PWM with 50Hz
p.start(0.5) # Initialization

p.ChangeDutyCycle(12.5) #home
time.sleep(0.5)
p.ChangeDutyCycle(0.5) #180
time.sleep(0.5)
p.ChangeDutyCycle(12.5) #home
time.sleep(0.5)
p.stop()

GPIO.cleanup()
exit()
