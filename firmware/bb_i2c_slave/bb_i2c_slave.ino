// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

const int I2C_ADDR = 2;
const int ledPin =  LED_BUILTIN;

void setup() {
 
  Wire.begin(I2C_ADDR);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  //Serial.begin(9600);           // start serial for output

  pinMode(ledPin, OUTPUT);
}

void loop() {
  delay(50);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

  //while (1 < Wire.available()) { // loop through all but the last
  //  char c = Wire.read(); // receive byte as a character
  //  Serial.print(c);         // print the character
  //}

  int x = Wire.read();    // receive byte as an integer

  digitalWrite(ledPin, x);
  //Serial.println(x);         // print the integer
}
