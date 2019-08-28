/* 
 *  
 * Quasi
 */

#include <Wire.h>
#include <SparkFun_TB6612.h>
#include <SoftwareSerial.h>

#define SSerialRX        A1  //Serial Receive pin
#define SSerialTX        A0  //Serial Transmit pin
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
//#define STBY_0 5

#define AIN1_1 A2
#define AIN2_1 A3
#define PWMA_1 9

#define BIN1_1 4
#define BIN2_1 2
#define PWMB_1 6
//#define STBY_1 5

#define AIN1_2 A6
#define AIN2_2 A7
#define PWMA_2 5

#define BIN1_2 A5
#define BIN2_2 A4
#define PWMB_2 3
//#define STBY_2 5


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


Motor _m0 =  Motor(AIN1_0, AIN2_0, PWMA_0, offsetA);
Motor _m1 =  Motor(BIN1_0, BIN2_0, PWMB_0, offsetA);

Motor _m2 =  Motor(AIN1_1, AIN2_1, PWMA_1, offsetA);
Motor _m3 =  Motor(BIN1_1, BIN2_1, PWMB_1, offsetA);

Motor _m4 =  Motor(A0, A1, PWMA_2, offsetA);
Motor _m5 =  Motor(BIN1_2, BIN2_2, PWMB_2, offsetA);

void setup() {
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  //Serial.println("Billy Bass Wall. Now playing: \"Sade\"");

  _m0.brake();
  _m1.brake();
  _m2.brake();
  _m3.brake();
  _m4.brake();
  _m5.brake();
  
  //pinMode(3, OUTPUT);
  //testLoop();

  driveAll(255, false);
  delay(1000);
  driveAll(0, false);
  
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

void runMotor(int _index, int _dir, int _speed)
{

  if (_dir == RELEASE)
    _speed = 0;
  else if (_dir == BACKWARD)
    _speed = -_speed;
}

void driveAll(int _val, bool _mouth) {

  if (_mouth)
  {
    if (_val == 0) {
    
    _m0.brake();
    //_m1.brake();
    _m2.brake();
    //_m3.brake();
    _m4.brake();
    //_m5.brake();
    }
    else
    {
    _m0.drive(_val);
    //_m1.drive(_val);
    _m2.drive(_val);
    //_m3.drive(_val);
    _m4.drive(_val);
    //_m5.drive(_val);
    }
  }
  else
  {
    if (_val == 0) {
    
    //_m0.brake();
    _m1.brake();
    //_m2.brake();
    _m3.brake();
    //_m4.brake();
    _m5.brake();
    }
    else
    {
    //_m0.drive(_val);
    _m1.drive(_val);
    //_m2.drive(_val);
    _m3.drive(_val);
    //_m4.drive(_val);
    _m5.drive(_val);
    }
  }



}

void testLoop() {

  _m0.drive(255);
  _m1.drive(255);
  delay(500);
  
  _m0.brake();
  _m1.brake();
  delay(500);
  
  _m0.drive(-255);
  _m1.drive(-255);
  delay(500);
  
  _m0.brake();
  _m1.brake();
  delay(500);

  _m2.drive(255);
  _m3.drive(255);
  delay(500);
  
  _m2.drive(0);
  _m3.drive(0);
  delay(500);
    
  _m2.drive(-255);
  _m3.drive(-255);
  delay(500);
  
  _m2.drive(0);
  _m3.drive(0);
  delay(500);

  Serial.println("Drive m4 and m5 to 255");
  _m4.drive(255);
  _m5.drive(255);
  delay(500);

  Serial.println("Drive m4 and m5 to 0");
  _m4.brake();
  _m5.brake();
  delay(500);

  Serial.println("Drive m4 and m5 to -255");
  _m4.drive(-255);
  _m5.drive(-255);
  delay(500);

  Serial.println("Drive m4 and m5 to 0");
  _m4.brake();
  _m5.brake();
  delay(500);

  
/*
  _m0.drive(255);
  Serial.println("Write M0");
  delay(2000);
  
  _m0.drive(-255);
  _m1.drive(255);
  Serial.println("Write M1");
  delay(2000);
  
  _m0.drive(0);
  _m1.drive(-255);
  _m2.drive(255);
  Serial.println("Write M2");
  delay(2000);
  
  _m1.drive(0);
  _m2.drive(-255);
  _m3.drive(255);
  Serial.println("Write M3");
  delay(2000);
  
  _m2.drive(0);
  _m3.drive(-255);
  _m4.drive(255);
  Serial.println("Write M4");
  delay(2000);
  
  _m3.drive(0);
  _m4.drive(-255);
  _m5.drive(255);
  Serial.println("Write M5");
  delay(2000);
  
  _m4.drive(0);
  _m5.drive(-255);
  delay(2000);
  _m5.drive(0);
 */

 //_m5.drive(255);

}


void loop() {

}

void serialEvent() {
 
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();

    //blink = !blink;
    //Serial.println(blink);

    switch (inChar) {

      case '1':

        Serial.println("mouth");
        driveAll(255, false);
        break;

      case '2':

        driveAll(-255, false);
        break;

      case '3':

        driveAll(0, false);
        break;

      case '4':

        Serial.println("mouth");
        driveAll(255, true);
        break;

      case '5':

        Serial.println("mouth");
        driveAll(-255, true);
        break;

      case '6':

        driveAll(0, true);
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




  
