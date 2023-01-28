/* 
 *  
 * Quasi
 */

#include <Wire.h>

int numBlink = 0;
boolean isBlinking = false;
boolean IS_SENDER = true;

long tmrBlink;
long tmrSend;

void setup() {
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("start");

  tmrBlink = millis();
  tmrSend = millis();
}

void blink() {

    isBlinking = true;
    tmrBlink = millis();
    digitalWrite(13, HIGH);
}

void loop() {

  if (isBlinking && millis() > tmrBlink + 500) {
    digitalWrite(13, LOW);
    isBlinking = false;
  }

  if (IS_SENDER) {

    if (millis() > tmrSend + 2000) {

      //Serial.println("send val");

      tmrSend = millis();
      Serial.write(5);
      blink();
    }
  }

}


void serialEvent() {

  //Serial.println("in");
  while (Serial.available()) {

    // Get the blink number
    numBlink = (int)Serial.read();
    blink();

  }
}



  
