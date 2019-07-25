
// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

int arrSlaves[] = {1,2};
int _numSlaves = 2;

long _next;
bool _state;

const int ledPin =  LED_BUILTIN;

void setup() {

  pinMode(ledPin, OUTPUT);
  Wire.begin(); // join i2c bus (address optional for master)

  Serial.begin(9600);

  _next = millis();
  _state = false;

}

byte x = 0;

void sendCmd(int _state) {

  for (int i=0; i<_numSlaves; i++) {

    Wire.beginTransmission(arrSlaves[i]); // transmit to device #8
    Wire.write(_state);              // sends one byte
    Wire.endTransmission();    // stop transmitting
  }

}

void loop() {

  if (millis() > _next) {

    _next = millis() + random(100,800);

    _state = !_state;
    digitalWrite(ledPin, _state);

    Serial.println(_state);

    if (_state)
      sendCmd(1);
    else
      sendCmd(0);
  Serial.println("Wire");}


  
  //Wire.beginTransmission(8); // transmit to device #8
  //Wire.write("x is ");        // sends five bytes
  //Wire.write(x);              // sends one byte
  //Wire.endTransmission();    // stop transmitting

  //x++;
  //delay(500);

}
