## This is code for using a DS04-NFC
## continuous rotation servo for an auger style fish feeder
## https://www.thingiverse.com/thing:301532

import RPi.GPIO as GPIO
import time

servoPIN = 12
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)

p = GPIO.PWM(servoPIN, 50) # GPIO 12 for PWM with 50Hz
p.start(1.5) # Initialization

def dispensefood():
    print("Feeding the Fish")
    p.ChangeDutyCycle(2)
    time.sleep(0.2)
    p.stop()

dispensefood()
