## This is code for using a modified HS-311 servo for 
## continuous rotation for an auger style fish feeder
## https://www.thingiverse.com/thing:301532

import RPi.GPIO as GPIO
import time

servoPIN = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)

p = GPIO.PWM(servoPIN, 50) # GPIO 17 for PWM with 50Hz
p.start(0.5) # Initialization

def dispensefood():
    print("Feeding the Fish")
    p.ChangeDutyCycle(15) #faking continuous servo-ing
    time.sleep(0.5)
    p.ChangeDutyCycle(13) #faking continuous servo-ing
    time.sleep(0.5)
    p.ChangeDutyCycle(15)
    time.sleep(0.5)
    p.ChangeDutyCycle(13)
    time.sleep(0.5)
    p.ChangeDutyCycle(15)
    time.sleep(0.5)
    p.ChangeDutyCycle(13)
    time.sleep(0.5)
    p.ChangeDutyCycle(15)
    time.sleep(0.5)
    p.ChangeDutyCycle(13)
    time.sleep(0.5)
    p.ChangeDutyCycle(15)
    time.sleep(0.5)
    p.ChangeDutyCycle(.1) #cleaning up
    print("Backing up...")
    time.sleep(0.5)
    p.stop()

dispensefood()
