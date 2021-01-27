#!/usr/bin/python
# Read the buttons on a Pimoroni PhatBeat hat and send a MQTT message when pressed
import paho.mqtt.client as paho
import time
import urlparse
import datetime
import signal
import phatbeat

# Mqtt
mqttc = paho.Client()
url_str = 'mqtt://ipaddress:port'
url = urlparse.urlparse(url_str)
mqttc.username_pw_set("mqtt-login", "mqtt-password")

def printtime(): # Used for debug
        # Current time
        global hour, minute, wholetime
        now = datetime.datetime.now()
        hour = str(now.hour)
        minute = int(now.minute)
        minute = '%02d' % minute
        wholetime = hour + ":" + minute

def sendmqtt(mess):
    try:
        mqttc.connect(url.hostname, url.port)
        mqttc.publish("phatbeat/state", mess)
        sleep(5)
    except:
            pass

@phatbeat.on(phatbeat.BTN_FASTFWD)
def fast_forward(pin):
   # print("FF Pressed")
       sendmqtt("FFWD")

@phatbeat.hold(phatbeat.BTN_FASTFWD, hold_time=2)
def hold_fast_forward(pin):
    #print("FF Held")
    sendmqtt("FFWD_HELD")

@phatbeat.on(phatbeat.BTN_PLAYPAUSE)
def play_pause(pin):
    #print("PP Pressed")
    sendmqtt("PLAY_PAUSE")

@phatbeat.hold(phatbeat.BTN_PLAYPAUSE, hold_time=2)
def hold_play_pause(pin):
    #print("PP Held")
    sendmqtt("PLAY_PAUSE_HELD")

@phatbeat.on(phatbeat.BTN_VOLUP)
def volume_up(pin):
    #print("VU Pressed")
    sendmqtt("VOL_UP")

@phatbeat.hold(phatbeat.BTN_VOLUP)
def hold_volume_up(pin):
    #print("VU Held")
    sendmqtt("VOL_UP_HELD")

@phatbeat.on(phatbeat.BTN_VOLDN)
def volume_down(pin):
    #print("VD Pressed")
    sendmqtt("VOL_DOWN")

@phatbeat.hold(phatbeat.BTN_VOLDN)
def hold_volume_down(pin):
    #print("VD Held")
    sendmqtt("VOL_DOWN_HELD")
    
@phatbeat.on(phatbeat.BTN_REWIND)
def rewind(pin):
    #print("RR Pressed")
    sendmqtt("RWD")

@phatbeat.hold(phatbeat.BTN_REWIND)
def hold_rewind(btn):
    #print("RR Held")
    sendmqtt("RWD_HELD")

@phatbeat.on(phatbeat.BTN_ONOFF)
def onoff(pin):
    #print("OO Pressed")
    sendmqtt("00")

@phatbeat.hold(phatbeat.BTN_ONOFF)
def hold_onoff(pin):
    #print("OO Held")
    sendmqtt("00_HELD")

signal.pause()
