import RPi.GPIO as GPIO
import time

servoPIN = 14
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)

p = GPIO.PWM(servoPIN, 50) # GPIO 17 for PWM with 50Hz
p.start(0.5) # Initialization
try:
  while True:
    p.ChangeDutyCycle(0.5) #home
    time.sleep(0.5)
    p.ChangeDutyCycle(12.5) #180
    time.sleep(0.5)
    p.ChangeDutyCycle(0.5) #home
    time.sleep(0.5)
    p.stop()
except KeyboardInterrupt:
  p.stop()
  GPIO.cleanup()

