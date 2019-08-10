/* 
 *  
 * Quasi
 */

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <ArduinoJson.h>
#include "Bass.cpp"

//Fix below to be array of billys
struct cmd {
  byte _cmd;
  int _time;
  bool _b1;
  bool _b2;
};

const int STATE_LOAD_SONG = 0;
const int STATE_RUN = 1;
const int STATE_WAIT = 0;

const int CMD_OPEN = 0;
const int CMD_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;

const int BASS_1_MOUTH = 0;
const int BASS_1_TAIL = 1;
const int BASS_2_MOUTH = 2;
const int BASS_2_TAIL = 3;

const int NUM_BASS = 5;

//long mouthNext = 0;
//long bodyNext = 0;

int _low = 100;
int _high = 0;

int count = 0;
int bodyCount = 0;
int bodyPosition = 0;

int buttonState = 0;
boolean headState = false;
boolean lastHeadButton = false;

//State
int runState = 0;

//Array of commands
cmd * arrCmd;

unsigned long  _runningTime;
int   nextCmdIndex;
int   numCmd;

String inputString = "";

// Create the motor shield object with the default I2C address
Adafruit_MotorShield *AFMS[3];
Bass *arrBass[5];

void setup() {
  
  Serial.begin(57600);           // set up Serial library at 9600 bps
  Serial.println("Billy Bass Wall. Now playing: \"Sade\"");

  //Register Shields
  AFMS[0] = new Adafruit_MotorShield(0x60);
  AFMS[1] = new Adafruit_MotorShield(0x68);
  AFMS[2] = new Adafruit_MotorShield(0x71);

  //Create Billys 
  // Shield Point, Mouth Index, Body Index, Mouth Dir, Body Dir
  arrBass[0] = new Bass(AFMS[2],2,1,true,true);
  arrBass[1] = new Bass(AFMS[2],4,3,false,true);
  arrBass[2] = new Bass(AFMS[0],2,1,false,false);
  arrBass[3] = new Bass(AFMS[0],4,3,false,true);
  arrBass[4] = new Bass(AFMS[1],2,1,true,false);

  for (int i=0; i<3; i++) {
    AFMS[i]->begin();
  }

  //Test all Bass
  testLoop2();

  //Set the pin modes
  //pinMode(2, OUTPUT);
  //pinMode(3, OUTPUT);

  runState = STATE_RUN;
}

void printDirection(int _dir)
{
  switch(_dir) {

    case FORWARD:
      Serial.print("FORWARD ");
      break;

    case BACKWARD:
      Serial.print("BACKWARD ");
      break;

    case RELEASE:
      Serial.print("RELEASE ");
      break;
  }
}

void printMotorIndex(int _index)
{
    switch(_index) {

    case BASS_1_MOUTH:
      Serial.print("BASS_1_MOUTH ");
      break;

    case BASS_1_TAIL:
      Serial.print("BASS_1_TAIL ");
      break;

    case BASS_2_MOUTH:
      Serial.print("BASS_2_MOUTH ");
      break;

    case BASS_2_TAIL:
      Serial.print("BASS_2_TAIL ");
      break;
  }
}

void testLoop2() {

  for (int i=0; i<NUM_BASS; i++)
  {

    Serial.println(i);
    
  Serial.println("MOUTH OPEN");
  arrBass[i]->mouthOpen();
  delay(500);

  Serial.println("MOUTH CLOSE");
  arrBass[i]->mouthClose();
  delay(500);

  Serial.println("BODY TAIL");
  arrBass[i]->bodyTail();
  delay(500);

  Serial.println("BODY ");
  arrBass[i]->runBody(RELEASE,255);
  delay(500);

  Serial.println("BODY HEAD");
  arrBass[i]->bodyHead();
  delay(500);

  Serial.println("BODY ");
  arrBass[i]->runBody(RELEASE,255);
  delay(500);
  
  }

}


void loop() {
 
  if (runState == STATE_RUN)
  {}
  else if (runState == STATE_WAIT) 
  {}
  else
  {}

}


void serialEvent() {

  //Serial.println("in");
 
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();

    if (inChar == '\n') {

      int _cmd = inputString[0];

      //Parse String
      for (int i=1; i<inputString.length(); i++)
      {
        int _index = inputString[i];
        switch (_cmd) {

          case CMD_OPEN:
            arrBass[_index]->mouthOpen();
            break;
          case CMD_CLOSE:
            arrBass[_index]->mouthClose();
            break;
          case CMD_TAIL_ON:
            arrBass[_index]->bodyTail();
            break;
          case CMD_HEAD_ON:
            arrBass[_index]->bodyHead();
            break;
          case CMD_BODY_OFF:
            arrBass[_index]->runBody(RELEASE,255);
            break;
        }
      }

      inputString = "";

    } else {
      
      inputString += inChar;
    }
/*
    switch (inChar) {

      case '1':

        Serial.println("mouth");
        arrBass(0
        runMotor(BASS_1_MOUTH, FORWARD, 255);
        break;

      case '2':

        runMotor(BASS_2_MOUTH, FORWARD, 255);
        break;

      case '3':

        runMotor(BASS_1_MOUTH, RELEASE, 0);
        break;

      case '4':

        runMotor(BASS_2_MOUTH, RELEASE, 0);
        break;

      case '5':

        runMotor(BASS_1_TAIL, BACKWARD, 255);
        break;

      case '6':

        runMotor(BASS_2_TAIL, BACKWARD, 255);
        break;
        
      case '7':

        runMotor(BASS_1_TAIL, FORWARD, 255);
        break;
        
      case '8':

        runMotor(BASS_2_TAIL, FORWARD, 255);
        break;
             
      case '9':

        runMotor(BASS_1_TAIL, RELEASE, 0); 
        break;

      case '0':

        runMotor(BASS_2_TAIL, RELEASE, 0);
        break;
    }
*/

    if (inChar == 'a') {
      
       digitalWrite(LED_BUILTIN, HIGH);
       
    }

    if (inChar == 'b') {
      
       digitalWrite(LED_BUILTIN, LOW);
       
    }    
  }
}



  
