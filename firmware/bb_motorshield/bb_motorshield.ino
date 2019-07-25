/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

const int BASS_1_MOUTH = 0;
const int BASS_1_TAIL = 1;
const int BASS_2_MOUTH = 2;
const int BASS_2_TAIL = 3;

long mouthNext = 0;
long bodyNext = 0;

int _low = 100;
int _high = 0;

int count = 0;
int bodyCount = 0;
int bodyPosition = 0;

int buttonState = 0;
boolean headState = false;
boolean lastHeadButton = false;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *_m[4];

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  for (int i=0; i<4; i++) {
    _m[i] = AFMS.getMotor(i+1);
  }

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
}

void runMotor(int _index, int _dir, int _speed)
{
  _m[_index]->run(_dir);
  _m[_index]->setSpeed(_speed);  
}

void runMotorAllMotors(int _dir, int _speed)
{
  
  for (int i=0; i<4; i++) {
 
    if (i == BASS_1_TAIL || i== BASS_2_TAIL || _dir != BACKWARD) {
 
      _m[i]->run(_dir);
      _m[i]->setSpeed(_speed);  
    }
  }
}

void loop() {

   buttonState = digitalRead(2);
  
  if (digitalRead(3) == HIGH && lastHeadButton == false) {
    headState = !headState;
  }  
  lastHeadButton = digitalRead(3);
  
  Serial.println(buttonState);
  if(buttonState) {
    runMotor(BASS_1_MOUTH, FORWARD, 255);
    
    if (headState) {
       runMotor(BASS_2_MOUTH, FORWARD, 255);
    }
  }
  else {
    runMotor(BASS_1_MOUTH, RELEASE, 0);
    runMotor(BASS_2_MOUTH, RELEASE, 0);
  }

  if (headState) {
    
    runMotor(BASS_1_TAIL, FORWARD, 255);
    runMotor(BASS_2_TAIL, FORWARD, 255);
        
  } else {
    
    if (millis() > bodyNext) {
    
      bodyPosition = !bodyPosition;
    
      if (bodyPosition) {
      
        runMotor(BASS_1_TAIL, BACKWARD, 255);
        runMotor(BASS_2_TAIL, BACKWARD, 255);
        bodyNext = millis() +  150;
      
      } else {
      
        runMotor(BASS_1_TAIL, BACKWARD, 0);
        runMotor(BASS_2_TAIL, BACKWARD, 0);
        bodyNext = millis() + 820;
      }
    }

  }


  /*
  if (millis() > bodyNext) {

    Serial.println(">");
    Serial.println(bodyPart);
    Serial.println(bodyPosition);

    if (bodyCount > 12) {

      bodyCount = 0;
      bodyPart = !bodyPart;
      bodyNext = millis() + 10000;
    }

    if (bodyPart == 0)
    {
        if (bodyPosition) {
          
          runMotor(BASS_1_TAIL, , );
          
     
          //motorBody->setHighLow(true, bodyPart);
          bodyNext = millis() + 100;
        } else {
          motorBody->setHighLow(false, bodyPart);
          bodyNext = millis() + 1000;
        }
    }
    else
    {
      motorBody->setHighLow(true, bodyPart);
    }
    
    bodyPosition = !bodyPosition;
    bodyCount++;
  }
  */
  
/*
  uint8_t i;
  

   
  Serial.print(buttonState);

    runMotorAllMotors(FORWARD, 255);
    delay(1000);
  
    runMotorAllMotors(BACKWARD, 255);   
    delay(1000); 

    runMotorAllMotors(RELEASE, 0);
    delay(1000);    
*/
}
