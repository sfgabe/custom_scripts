#!/usr/bin/python
# Gets the temp and humity from a DHT11 and sends it out via MQTT

from __future__ import print_function
import Adafruit_DHT
import RPi.GPIO as GPIO
import datetime
import sys
import paho.mqtt.client as paho
import time
import glob
import platform

# Out of + pins!
# Give GPIO pin 3.3v power
GPIO.setmode(GPIO.BCM)
GPIO.setup(24, GPIO.OUT)
GPIO.output(24, GPIO.HIGH)

sensor = Adafruit_DHT.DHT11
pin = 23
humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)
node = platform.node()
mq = paho.Client()
file = "/home/pi/data/current_dst11.txt"

def my_callback():
    if humidity is not None and temperature is not None:
        print('Temp={0:0.1f}*C  Humidity={1:0.1f}% at '.format(temperature, humidity) + str(datetime.datetime.now()))
        log = open(file, "w")
        print ('Temp={0:0.1f}*C  Humidity={1:0.1f}% at '.format(temperature, humidity) + str(datetime.datetime.now()), file = log)
    else:
        print('Failed to get reading. Try again! Checked at ' + str(datetime.datetime.now()))
        log = open(file, "w")
        print('Message= Failed to get reading. Try again! CHecked at ' + str(datetime.datetime.now()))

def mqtt_publish():
    mq.loop_start()
    # Connect to homeassistant
    mq.username_pw_set(username="xxxx",password="xxxx")
    mq.connect("raspberrypi.local")
    with open(file, 'r') as myfile:
        data = myfile.read()
    mq.publish("dht11/reading",data)

try:
    while True:
        callback=my_callback()
        time.sleep(10)
        publish=mqtt_publish()
        GPIO.setwarnings(False)
        time.sleep(120)

except KeyboardInterrupt:
    GPIO.cleanup()       # clean up GPIO on CTRL+C exit
GPIO.cleanup()           # clean up GPIO on normal exit
