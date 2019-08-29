/* 
 *  
 * Quasi
 */

#include <Wire.h>
//#include <Adafruit_MotorShield.h>

#include <ArduinoJson.h>
#include "Bass.cpp"

//Fix below to be array of billys
struct cmd {
  byte _cmd;
  int _time;
  bool _b1;
  bool _b2;
};

//DEFINE THE MOTOR DRIVER PINS
#define AIN2_0 13
#define AIN1_0 12
#define PWMA_0 11

#define BIN1_0 8
#define BIN2_0 7
#define PWMB_0 10

#define AIN1_1 A2
#define AIN2_1 A3
#define PWMA_1 9

#define BIN1_1 4
#define BIN2_1 2
#define PWMB_1 6

#define AIN1_2 A6
#define AIN2_2 A7
#define PWMA_2 5

#define BIN1_2 A5
#define BIN2_2 A4
#define PWMB_2 3


const int STATE_LOAD_SONG = 0;
const int STATE_RUN = 1;
const int STATE_WAIT = 0;

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
//Adafruit_MotorShield *AFMS[3];
Bass *arrBass[5];

void setup() {
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Billy Bass Wall");

  //Register Shields
  /*
  AFMS[0] = new Adafruit_MotorShield(0x60);
  AFMS[1] = new Adafruit_MotorShield(0x68);
  AFMS[2] = new Adafruit_MotorShield(0x71);
  */

  //Create Billys 
  // Shield Point, Mouth Index, Body Index, Mouth Dir, Body Dir

  arrBass[0] = new Bass(AIN1_0, AIN2_0, PWMA_0, BIN1_0, BIN2_0, PWMB_0, true, true);
  arrBass[1] = new Bass(AIN1_1, AIN2_1, PWMA_1, BIN1_1, BIN2_1, PWMB_1, true, true);
  arrBass[2] = new Bass(AIN1_2, AIN2_2, PWMA_2, BIN1_2, BIN2_2, PWMB_2, true, true);

  /*
  arrBass[1] = new Bass(AFMS[2],1,2,true,false);
  arrBass[0] = new Bass(AFMS[2],4,3,false,true);
  arrBass[2] = new Bass(AFMS[0],2,1,false,false);
  arrBass[3] = new Bass(AFMS[0],4,3,false,true);
  arrBass[4] = new Bass(AFMS[1],4,3,true,false);
  */

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

  int _delay = 200;
  
  for (int i=0; i<NUM_BASS; i++)
  {

    Serial.println(i);
    
  Serial.println("MOUTH OPEN");
  arrBass[i]->mouthOpen();
  delay(_delay);

  Serial.println("MOUTH CLOSE");
  arrBass[i]->mouthClose();

  Serial.println("BODY TAIL");
  arrBass[i]->bodyTail();
  delay(_delay);

  Serial.println("BODY HEAD");
  arrBass[i]->bodyHead();
  delay(_delay);

  Serial.println("BODY ");
  arrBass[i]->runBody(RELEASE,255);
  delay(_delay);
  
  }

}


void loop() {

}


void serialEvent() {

  //Serial.println("in");
 
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();

    if (inChar == '\n') {

      int _cmd = inputString[0] - '0';

      Serial.println("done");
      Serial.println(_cmd);

      if (inputString.length() == 1) {

          //Parse String
          for (int i=0; i<NUM_BASS; i++)
          {
            switch (_cmd) {
              case CMD_OPEN:
                arrBass[i]->mouthOpen();
                break;
              case CMD_CLOSE:
                arrBass[i]->mouthClose();
                break;
              case CMD_TAIL_ON:
                arrBass[i]->bodyTail();
                break;
              case CMD_HEAD_ON:
                arrBass[i]->bodyHead();
                break;
              case CMD_TAIL_OFF:
              
                if (arrBass[i]->lastCommand != CMD_HEAD_ON) {
                  arrBass[i]->runBody(RELEASE,255);
                }
                break;
  
              case CMD_BODY_OFF:
                arrBass[i]->runBody(RELEASE,255);
                break;
            }
          }
      }
      else
      {

          //Parse String
          for (int i=1; i<inputString.length(); i++)
          {
            int _index = inputString[i] - '0';
    
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



  