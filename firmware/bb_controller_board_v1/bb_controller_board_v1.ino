/* 
 *  
 * Quasi
 */

#include <Wire.h>
//#include <Adafruit_MotorShield.h>

//#include <ArduinoJson.h>
#include "Bass.cpp"

//Fix below to be array of billys
struct cmd {
  byte _cmd;
  int _time;
  bool _b1;
  bool _b2;
};

//DEFINE THE MOTOR DRIVER PINS
#define BIN2_0 12
#define BIN1_0 13
#define PWMB_0 11

#define AIN1_0 8
#define AIN2_0 7
#define PWMA_0 10

#define AIN1_1 A2
#define AIN2_1 A3
#define PWMA_1 9

#define BIN1_1 4
#define BIN2_1 2
#define PWMB_1 6

#define AIN1_2 A1
#define AIN2_2 A0
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
const int BASS_3_MOUTH = 4;
const int BASS_3_TAIL = 5;

const int NUM_BASS = 3;
const byte BOARD_ID = 'q';


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
Bass *arrBass[3];

void blinkT(int _num=1, int _delay=50) {

  for (int i=0; i<_num; i++)
  {

    digitalWrite(LED_BUILTIN, HIGH);
    delay(_delay);
    digitalWrite(LED_BUILTIN, LOW);
    delay(_delay);
  }

}

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

  Serial.println("BASS 0");
  arrBass[0] = new Bass(AIN1_0, AIN2_0, PWMA_0, BIN1_0, BIN2_0, PWMB_0, false, true);
  delay(500);

  Serial.println("BASS 1");
  arrBass[1] = new Bass(AIN1_1, AIN2_1, PWMA_1, BIN1_1, BIN2_1, PWMB_1, false, true);
  delay(500);

  Serial.println("BASS 2");
  arrBass[2] = new Bass(AIN1_2, AIN2_2, PWMA_2, BIN1_2, BIN2_2, PWMB_2, false, true);
  delay(500);

  /*
  arrBass[1] = new Bass(AFMS[2],1,2,true,false);
  arrBass[0] = new Bass(AFMS[2],4,3,false,true);
  arrBass[2] = new Bass(AFMS[0],2,1,false,false);
  arrBass[3] = new Bass(AFMS[0],4,3,false,true);
  arrBass[4] = new Bass(AFMS[1],4,3,true,false);
  */

  //Test all Bass
  testAnimation();

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

    case BASS_3_MOUTH:
      Serial.print("BASS_3_MOUTH ");
      break;

    case BASS_3_TAIL:
      Serial.print("BASS_3_TAIL ");
      break;
  }
}

void testAnimation() {

  int _delay = 500;
  
  for (int i=0; i<NUM_BASS; i++)
  {

  Serial.println(i);
    
  Serial.println("MOUTH OPEN");
  arrBass[i]->mouthOpen();
  delay(_delay);

  Serial.println("MOUTH CLOSE");
  arrBass[i]->mouthClose();
  delay(_delay);

  Serial.println("Tail");
  arrBass[i]->bodyTail();
  delay(_delay);

  Serial.println("Flat");
  arrBass[i]->runBody(RELEASE,MAX_MOTOR);
  delay(_delay);

  Serial.println("Head");
  arrBass[i]->bodyHead();
  delay(_delay);

  Serial.println("Flat");
  arrBass[i]->runBody(RELEASE,MAX_MOTOR);
  
  }

  delay(_delay);

}

void loop() {

  for (int i=0; i<3; i++) {
    arrBass[i]->update();
  }
}


void serialEvent() {

  //Serial.println("in");
 
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();

    if (inChar == '\n') {

      byte _groupID;
      bool _isGroup = false;

      int _cmd = inputString[0] - '0';
      int _len = inputString.length();

      if (_len == 2) {
        _groupID = inputString[1];
        _isGroup = true;
      }

      //JUST A COMMAND
      if (_len == 1 || _len == 2) {

          //Parse String
          for (int i=0; i<NUM_BASS; i++)
          {

            //If this is a group and we are in group, or all
            if (_isGroup == false || _groupID == arrBass[i]->GROUP_ID)
            {
              switch (_cmd) {
              
                case CMD_OPEN:
                  Serial.println("MOUTH OPEN");
                  arrBass[i]->mouthOpen();
                  break;
                
                case CMD_CLOSE:
                  Serial.println("MOUTH CLOSE");
                  arrBass[i]->mouthClose();
                  break;
                
                case CMD_TAIL_ON:
                
                  Serial.println("TAIL ON");
                  if (arrBass[i]->lastCommand != CMD_HEAD_ON) {
                    arrBass[i]->bodyTail();
                  }
                  break;
                
                case CMD_HEAD_ON:
                  Serial.println("HEAD ON");
                  arrBass[i]->bodyHead();
                  break;
                
                case CMD_TAIL_OFF:

                  Serial.println("TAIL OFF");
                  if (arrBass[i]->lastCommand != CMD_HEAD_ON) {
                    arrBass[i]->runBody(RELEASE,MAX_MOTOR);
                  }
                  break;
           
                case CMD_BODY_OFF:

                  Serial.println("BODY OFF");
                  arrBass[i]->runBody(RELEASE,MAX_MOTOR);
                  break;

              }

            }
          }
    
      }
      else if (_len > 2)
      {

          if (_cmd == CMD_LEARN_GROUP && _len == 4) {

            byte _boardNum =  inputString[1];
            int  _bassIndex = inputString[2] - '0';
            byte _groupID = inputString[3];

            if (_boardNum == BOARD_ID) {
              arrBass[_bassIndex]->addToGroup(_groupID);

              Serial.println("Add group");
              Serial.println(_bassIndex);
              Serial.println(_groupID);

            }

            blinkT(5);
    
          }
          else
          {

            //Num pairs
            int _pairs = (_len-1) / 2;

            for (int i=0; i<_pairs; i++)
            {

              //Iterate over pairs
              byte _boardNum = inputString[i*2+1];
              int  _bassIndex = inputString[i*2+2] - '0';

              if (_boardNum == BOARD_ID) {
            
                switch (_cmd) {
    
                case CMD_OPEN:
                  arrBass[_bassIndex]->mouthOpen();
                  break;
                case CMD_CLOSE:
                  arrBass[_bassIndex]->mouthClose();
                  break;
                case CMD_TAIL_ON:
                  arrBass[_bassIndex]->bodyTail();
                  break;
                case CMD_HEAD_ON:
                  arrBass[_bassIndex]->bodyHead();
                  break;
                case CMD_BODY_OFF:
                  arrBass[_bassIndex]->runBody(RELEASE,MAX_MOTOR);
                  break;  
                }
              }
            }
          }
          //Parse String
      }
      
      inputString = "";

    } else {
      
      inputString += inChar;
    }

    /*
    if (inChar == 'a') {
      
       digitalWrite(LED_BUILTIN, HIGH);
       
    }

    if (inChar == 'b') {
      
       digitalWrite(LED_BUILTIN, LOW);
       
    }    
    */
    
  }
}



  
