/* 
 *  
 * Quasi
 */

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <ArduinoJson.h>

struct cmd {
  byte _cmd;
  long _time;
  byte _set; 
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

//State
int runState = 0;

//Array of commands
cmd * arrCmd;

long  _runningTime;
int   nextCmdIndex;
int   numCmd;

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

  //Set the pin modes
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  /*
   * Set static JSON doc of commands
   * 
   *  CMD_OPEN = 0;
   *  CMD_CLOSE = 1;  
   *  CMD_TAIL_ON = 2;
   *  CMD_HEAD_ON = 3;
   *  CMD_BODY_OFF = 4;
   */
  StaticJsonDocument<500> objCmd;

  char json[] = "["\
    "{\"cmd\":0,\"time\":0,\"set\":[0,1]},"\
    "{\"cmd\":1,\"time\":1000,\"set\":[0,1]},"\
    "{\"cmd\":0,\"time\":2000,\"set\":[0]},"\
    "{\"cmd\":2,\"time\":3000,\"set\":[1]},"\
    "{\"cmd\":4,\"time\":4000,\"set\":[1]},"\
    "{\"cmd\":3,\"time\":5000,\"set\":[1]},"\
    "{\"cmd\":4,\"time\":6000,\"set\":[1]}"\
  "]";

  /*
   * Deserialize the JSON document
   * 
   * Create array of objects
   * 
   */
  DeserializationError error = deserializeJson(objCmd, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  //Get the size of the JSON
  numCmd = objCmd.size();

  //Create array of commands
  arrCmd = (cmd*)malloc(sizeof(cmd) * numCmd);

  //Create array of cmd objects
  for (int i=0; i<numCmd; i++) {
    
    arrCmd[i] = cmd {
      (byte)objCmd[i]["cmd"], 
      (long)objCmd[i]["time"], 
      (byte)objCmd[i]["set"][0]
    };
  }

  //Init running time and command index
  _runningTime = millis();
  nextCmdIndex = 0;

  runState = STATE_RUN;
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

  if (runState == STATE_RUN)
  {

    if ((millis() > _runningTime + arrCmd[nextCmdIndex]._time) ) {

      Serial.println((_runningTime + arrCmd[nextCmdIndex]._time));
      Serial.println(millis());
      Serial.println(nextCmdIndex);
      Serial.println();

      nextCmdIndex++;

      if (nextCmdIndex >= numCmd) {
        runState = STATE_WAIT;
        nextCmdIndex = 0;
        _runningTime = millis();
      }
    }
  
  }
  else if (runState == STATE_WAIT) 
  {
    
  }
  else
  {
    
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

  }

}
