// A sketch for a DigiSpark USB device that detects when a door is opened
// The door should have a reed switch with NC (normally closed) connected to the Vin wire of the device
// When the door is opened, the reed switch is closed and the device is started
// A 32-nit message is then sent via a 433 MHz signal that can be picked up by an appropriate receiver
// (a Raspberry Pi in my case).
// When the door is closed, the reed switch is opened and the device is shut down
// 
// Depends on the RCSwitch library https://github.com/sui77/rc-switch
// and the eeprom-library 
// based on https://github.com/LarsBergqvist/door_open_detector, with additional powersaver for frequently left-open doors

#include "RCSwitch.h"
#include <EEPROM.h>
#include <avr/sleep.h>

// Unique ID:s (4 bits, 0-15) for each measurement type so that the receiver
// understands how to interpret the data on arrival
#define DOOR_MEASUREMENT_ID 4

#define TX_PIN 1                     // PWM output pin to use for transmission


// Utility macros
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

// constants 
const unsigned long Alarm = 300000; // alarm time - 5 minutes
unsigned long StartTime = 0;        // start time

// A rolling sequence number for each measurement so that the receiver can handle duplicate data
// Restarts at 0 after seqNum=15 has been used. Stored in EEPROM between restarts of the device.
byte seqNum=0;

RCSwitch transmitter = RCSwitch();
 
void setup() 
{
  if ( EEPROM.read(0) == 42 )
  {
    // We have stored the previously used sequence number in EEPROM
    seqNum = EEPROM.read(1);
  }
  seqNum++;
  if (seqNum > 15)
  {
    seqNum = 0;
  }
  EEPROM.write(0,42);
  EEPROM.write(1,seqNum);

  transmitter.enableTransmit(TX_PIN); 
  transmitter.setRepeatTransmit(25);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}


bool valueHasBeenSent = false;
void loop() 
{
  if (!valueHasBeenSent)
  {
    // Send the message several times to increase
    // detection by the receiver
    sendDoorOpenSignal(seqNum);
    delay(4000);
    sendDoorOpenSignal(seqNum);
    delay(4000);
    sendDoorOpenSignal(seqNum);
    delay(4000);
    valueHasBeenSent = true;
    enterSleep();
  }
}

void enterSleep(void)
{
  sleep_enable();
  sleep_cpu();
}

void sendDoorOpenSignal(int sequenceNumber)
{
  // use alternating bits for the value for better reliability
  unsigned long valueToSend = 0b0101010;
  unsigned long dataToSend = code32BitsToSend(DOOR_MEASUREMENT_ID,sequenceNumber,valueToSend);
  transmitter.send(dataToSend, 32);    
}

unsigned long code32BitsToSend(int measurementTypeID, unsigned long seq, unsigned long data)
{
    unsigned long checkSum = measurementTypeID + seq + data;
    unsigned long byte3 = ((0x0F & measurementTypeID) << 4) + (0x0F & seq);
    unsigned long byte2_and_byte_1 = 0xFFFF & data;
    unsigned long byte0 = 0xFF & checkSum;
    unsigned long dataToSend = (byte3 << 24) + (byte2_and_byte_1 << 8) + byte0;

    return dataToSend;
}
