#!/usr/bin/python
# modified from https://community.openhab.org/t/controlling-a-raspberrypi-gpio-pin-with-mqtt/40912/33
# controls GPIO connected LED lights via an MQTT channel

import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

MQTT_HOST = "MQTT_HOST"
MQTT_PORT = XXXX
MQTT_KEEPALIVE_INTERVAL = 90
MQTT_TOPIC = "octocam-gpio-lights/reading"
MQTT_USER = "USERNAME"
MQTT_PASS ="PASSWORD"

# Raspberry Pi pins connected to H-bridge
LIGHT_GPIO_IN1 = 6
LIGHT_GPIO_IN2 = 1
LIGHT_GPIO_ENA = 12

GPIO.setmode(GPIO.BCM)
GPIO.setup(LIGHT_GPIO_IN1, GPIO.OUT)
GPIO.setup(LIGHT_GPIO_IN2, GPIO.OUT)
GPIO.setup(LIGHT_GPIO_ENA, GPIO.OUT)
GPIO.output(LIGHT_GPIO_IN1,GPIO.HIGH)
GPIO.output(LIGHT_GPIO_IN2,GPIO.HIGH)
GPIO.PWM(LIGHT_GPIO_ENA,1500).start(100)

try:
  def on_connect(self,mosq, obj, rc):
     mqttc.subscribe(MQTT_TOPIC, 0)
     print("Connect on "+MQTT_HOST)
    def on_message(mosq, obj, msg):
     if (msg.payload=='ON'):
           GPIO.output(LIGHT_GPIO_IN1,GPIO.HIGH)
           GPIO.output(LIGHT_GPIO_IN2,GPIO.LOW)
           GPIO.output(LIGHT_GPIO_ENA,GPIO.HIGH)
           print 'Garden Light On'
           mqttc.publish(msg.payload)
     if (msg.payload=='OFF'):
           GPIO.output(LIGHT_GPIO_IN1,False)
           GPIO.output(LIGHT_GPIO_IN2,False)
           GPIO.output(LIGHT_GPIO_ENA,False)
           print 'Garden Light Off'
           mqttc.publish(msg.payload)

  def on_subscribe(mosq, obj, mid, granted_qos):
          print("Subscribed to Topic: " +
          MQTT_TOPIC + " with QoS: " + str(granted_qos))

    # Initiate MQTT Client
  mqttc = mqtt.Client()

    # Assign event callbacks
  mqttc.on_message = on_message
  mqttc.on_connect = on_connect
  mqttc.on_subscribe = on_subscribe

    # Connect with MQTT Broker
  mqttc.username_pw_set(username=(MQTT_USER),password=(MQTT_PASS))
  mqttc.connect(MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE_INTERVAL)

    # Continue monitoring the incoming messages for subscribed topic
  mqttc.loop_forever()

except KeyboardInterrupt:  
    GPIO.cleanup()  
