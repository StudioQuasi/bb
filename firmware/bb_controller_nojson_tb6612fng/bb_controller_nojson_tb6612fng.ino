/* 
 *  
 * Quasi
 */

#include <Wire.h>
#include <SparkFun_TB6612.h>
#include <SoftwareSerial.h>

#define SSerialRX        A3  //Serial Receive pin
#define SSerialTX        A2  //Serial Transmit pin
#define Pin13LED         13

#include <ArduinoJson.h>

//Fix below to be array of billys
struct cmd {
  byte _cmd;
  int _time;
  bool _b1;
  bool _b2;
};

#define FORWARD 1
#define BACKWARD -1
#define RELEASE 0

#define AIN2_0 13
#define AIN1_0 12
#define PWMA_0 11

#define BIN1_0 8
#define BIN2_0 7
#define PWMB_0 10
#define STBY_0 5

#define AIN1_1 A0
#define AIN2_1 4
#define PWMA_1 9

#define BIN1_1 A1
#define BIN2_1 2
#define PWMB_1 6
#define STBY_1 5

#define AIN1_2 A6
#define AIN2_2 A7
#define PWMA_2 3

#define BIN1_2 A5
#define BIN2_2 A4
#define PWMB_2 5
#define STBY_2 5

SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX


int byteReceived;
int _readIndex = 0;
byte _input[32];
byte _addr = 4;

/*

*/

const int offsetA = 1;
const int offsetB = 1;

const int STATE_LOAD_SONG = 0;
const int STATE_RUN = 1;
const int STATE_WAIT = 0;

const int CMD_OPEN = 0;
const int CMD_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;

const int BASS_1_MOUTH  = 0;
const int BASS_1_TAIL   = 1;
const int BASS_2_MOUTH  = 2;
const int BASS_2_TAIL   = 3;
const int BASS_3_MOUTH  = 4;
const int BASS_3_TAIL   = 5;

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

// Create the motor shield object with the default I2C address
//Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
//Motor  *_m[4];

Motor _m0 =  Motor(AIN1_0, AIN2_0, PWMA_0, offsetA, STBY_0);
Motor _m1 =  Motor(BIN1_0, BIN2_0, PWMB_0, offsetA, STBY_0);

Motor _m2 =  Motor(AIN1_1, AIN2_1, PWMA_1, offsetA, STBY_1);
Motor _m3 =  Motor(BIN1_1, BIN2_1, PWMB_1, offsetA, STBY_1);

//#define AIN1_2 A6
//#define AIN2_2 A7
//#define PWMA_2 3

Motor _m4 =  Motor(AIN1_2, AIN2_2, PWMA_2, offsetA, STBY_2);
Motor _m5 =  Motor(BIN1_2, BIN2_2, PWMB_2, offsetA, STBY_2);


//Adafruit_DCMotor *_m1 = AFMS.getMotor(1);
//Adafruit_DCMotor *_m2 = AFMS.getMotor(2);
//Adafruit_DCMotor *_m3 = AFMS.getMotor(3);
//Adafruit_DCMotor *_m4 = AFMS.getMotor(4);

void setup() {
  
  Serial.begin(57600);           // set up Serial library at 9600 bps
  Serial.println("Billy Bass Wall. Now playing: \"Sade\"");

  /*
  for (int i=0; i<4; i++) {

    Serial.println("Motor");
    Serial.println(i);

    _m[i] = AFMS.getMotor(i+1);
  }*/

  //AFMS.begin();  // create with the default frequency 1.6KHz

  //test loop
  
  testLoop();

  //Set the pin modes
  //pinMode(2, OUTPUT);
  //pinMode(3, OUTPUT);

  runState = STATE_RUN;

  RS485Serial.begin(9600);
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

void runMotor(int _index, int _dir, int _speed)
{

  if (_dir == RELEASE)
    _speed = 0;
  else if (_dir == BACKWARD)
    _speed = -_speed;

  switch(_index) {

    /*
    case BASS_1_MOUTH:
    
      _m0.drive(_speed);
      Serial.print("BASS_1_MOUTH ");
      break;

    case BASS_1_TAIL:

      _m1.drive(_speed);
      Serial.print("BASS_1_TAIL ");
      break;

    case BASS_2_MOUTH:

      _m2.drive(_speed);
      Serial.print("BASS_2_MOUTH ");
      break;

    case BASS_2_TAIL:

      _m3.drive(_speed);
      Serial.print("BASS_2_TAIL ");
      break;

    case BASS_3_MOUTH:

      _m4.drive(_speed);
      Serial.print("BASS_3_MOUTH ");
      break;

    case BASS_3_TAIL:

      _m5.drive(_speed);
      Serial.print("BASS_3_TAIL ");
      break;
*/

  }
  

/*
  switch () {
    case 0:
      
      break;
  }
*/

  /*
   * if (_dir == RELEASE)
    _m[_index]->drive(0, 1000);
  else if (_dir == FORWARD)
    _m[_index]->drive(_speed, 1000);
  else
    _m[_index]->drive(-_speed, 1000);
  */
  
  //  _m[_index]->run(_dir);
  //  _m[_index]->setSpeed(_speed);  
}

/*void runMotorAllMotors(int _dir, int _speed)
{
  
  for (int i=0; i<4; i++) {
 
    if (i == BASS_1_TAIL || i== BASS_2_TAIL || _dir != BACKWARD) {
 
      _m[i]->run(_dir);
      _m[i]->setSpeed(_speed);  
    }
  }
}*/

void testLoop() {


  _m0.drive(255);
  _m1.drive(255);

  
  _m2.drive(255);
  _m3.drive(255);
  
  _m5.drive(255);
 

/*
  Serial.println("Test Loop");
  runMotor(BASS_3_MOUTH, FORWARD, 255);
  delay(1000);

  runMotor(BASS_3_MOUTH, RELEASE, 0);
  delay(1000);

  
  runMotor(BASS_3_TAIL, BACKWARD, 255);
  delay(1000);

  runMotor(BASS_3_TAIL, FORWARD, 255);
  delay(1000);

  runMotor(BASS_3_TAIL, RELEASE, 0);
  delay(1000);
*/

/*
  Serial.println("Test Loop");
  
  runMotor(BASS_1_MOUTH, FORWARD, 255);
  runMotor(BASS_2_MOUTH, FORWARD, 255);
  runMotor(BASS_3_MOUTH, FORWARD, 255);
  delay(1000);
  
  runMotor(BASS_1_MOUTH, RELEASE, 0);
  runMotor(BASS_2_MOUTH, RELEASE, 0);
  runMotor(BASS_3_MOUTH, RELEASE, 0);
  delay(1000);

  runMotor(BASS_1_TAIL, BACKWARD, 255);
  runMotor(BASS_2_TAIL, BACKWARD, 255);
  runMotor(BASS_3_TAIL, BACKWARD, 255);
  delay(1000);

  runMotor(BASS_1_TAIL, FORWARD, 255);
  runMotor(BASS_2_TAIL, FORWARD, 255);
  runMotor(BASS_3_TAIL, FORWARD, 255);
  delay(1000);

  runMotor(BASS_1_TAIL, RELEASE, 0);
  runMotor(BASS_2_TAIL, RELEASE, 0);
  runMotor(BASS_3_TAIL, RELEASE, 0);
  delay(1000);
*/
  
}


void loop() {

  if (RS485Serial.available())
  {

    byteReceived = RS485Serial.read();   // Read the byte 

    //IF END OF SERIAL STRING
    if (byteReceived == '\n') {

      if (_input[0] == _addr) {

        for (int i=0; i<_input[1]; i++)
        {
          digitalWrite(Pin13LED, HIGH);  // Show activity
          delay(20);
          digitalWrite(Pin13LED, LOW);
          delay(50);
        }

      }

      _readIndex = 0;
    } else {

      int _num = byteReceived - '0';
      _input[_readIndex++] = _num;
    }
  }

  if (runState == STATE_RUN)
  {
 
  }
  else if (runState == STATE_WAIT) 
  {
    
  }
  else
  {

  }

}


void serialEvent() {

  Serial.println("in");
 
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();

    //blink = !blink;
    //Serial.println(blink);

    switch (inChar) {

      case '1':

        Serial.println("mouth");
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

    if (inChar == 'a') {
      
       digitalWrite(LED_BUILTIN, HIGH);
       
    }

    if (inChar == 'b') {
      
       digitalWrite(LED_BUILTIN, LOW);
       
    }    
  }
}



  
