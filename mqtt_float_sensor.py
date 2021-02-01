#!/usr/bin/env python

import paho.mqtt.client as paho
import time
import glob
import platform

node = platform.node()

mq = paho.Client()
mq.loop_start()

# Connect to homeassistant
mq.username_pw_set(username="mqtt_username",password="mqtt_password")
mq.connect("mqtt-server-name-or-ip")

file = "/home/pi/data/current_water_level.txt"

while True:
   with open(file, 'r') as myfile:
      data = myfile.read()
      print data

      mq.publish("water-level-octo/reading",data)
         time.sleep(30)

