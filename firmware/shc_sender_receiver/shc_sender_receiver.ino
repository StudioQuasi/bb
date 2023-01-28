/* 
 *  
 * Quasi
 */

#include <Wire.h>

int numBlink = 250;
boolean isBlinking = false;
boolean IS_SENDER = false;

long tmrBlink;
long tmrSend;

void setup() {

  Serial.begin(9600);
  Serial.println(IS_SENDER ? "start sender" : "start receiver");

  tmrBlink = millis();
  tmrSend = millis();

  blink();

}

void blink() {

    isBlinking = true;
    tmrBlink = millis();
    digitalWrite(13, HIGH);
}

void loop() {

  //Passing byte value ... multiplying by 4 and using as blink delay
  if (isBlinking && millis() > tmrBlink + numBlink*4) {
    digitalWrite(13, LOW);
    isBlinking = false;
  }

  if (IS_SENDER) {

    if (millis() > tmrSend + 2000) {

      //Serial.println("send val");
      tmrSend = millis();

      byte _n = random(256);
      numBlink = _n;
      Serial.write(_n);
      blink();
    }
  }

}


void serialEvent() {

  //Serial.println("in");
  if (!IS_SENDER) {

    while (Serial.available()) {

      //Serial.println("In");
 
      // Get the blink number
      numBlink = (int)Serial.read();
      blink();
    }
  }
  
}



  
