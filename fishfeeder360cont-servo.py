## This is code for using a DS04-NFC
## continuous rotation servo for an auger style fish feeder
## https://www.thingiverse.com/thing:301532

import time
import wiringpi

servoPIN = 12

wiringpi.wiringPiSetupGpio()
wiringpi.pinMode(servoPIN, wiringpi.GPIO.PWM_OUTPUT)
wiringpi.pwmSetMode(wiringpi.GPIO.PWM_MODE_MS)

wiringpi.pwmSetClock(192)
wiringpi.pwmSetRange(2000)

delay_period = 0.02
backup_period = 0.001

def dispensefood():
    wiringpi.pwmWrite(servoPIN,0)
    print("Feeding the Fish")
    for pulse in range(50, 250, 1):
           wiringpi.pwmWrite(servoPIN, pulse)
           time.sleep(delay_period)
    print("Backing up...")
    for pulse in range(250, 50, -1):
            wiringpi.pwmWrite(servoPIN, pulse)
            time.sleep(backup_period)
    wiringpi.pwmWrite(servoPIN,0)

dispensefood()
