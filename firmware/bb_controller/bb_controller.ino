/* 
 *  
 * Quasi
 */

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <ArduinoJson.h>

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
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *_m[4];

//Adafruit_DCMotor *_m1 = AFMS.getMotor(1);
//Adafruit_DCMotor *_m2 = AFMS.getMotor(2);
//Adafruit_DCMotor *_m3 = AFMS.getMotor(3);
//Adafruit_DCMotor *_m4 = AFMS.getMotor(4);

void setup() {
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Billy Bass Wall. Now playing: \"Sade\"");

  for (int i=0; i<4; i++) {

    Serial.println("Motor");
    Serial.println(i);

    _m[i] = AFMS.getMotor(i+1);
  }

  AFMS.begin();  // create with the default frequency 1.6KHz

  //test loop
  
  testLoop();

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
  StaticJsonDocument<1000> objCmd;

  char json[] = "["\
    "{\"cmd\":0,\"time\":0,\"set\":[true,true]},"\
    "{\"cmd\":1,\"time\":1000,\"set\":[true,true]},"\
    "{\"cmd\":0,\"time\":2000,\"set\":[true,false]},"\
    "{\"cmd\":2,\"time\":3000,\"set\":[false,true]},"\
    "{\"cmd\":4,\"time\":4000,\"set\":[false,true]},"\
    "{\"cmd\":3,\"time\":5000,\"set\":[false,true]},"\
    "{\"cmd\":4,\"time\":6000,\"set\":[false,true]}"\
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
      (int)objCmd[i]["time"], 
      (bool)objCmd[i]["set"][0],
      (bool)objCmd[i]["set"][1]
    };

    Serial.println("check time");
    Serial.println((int)objCmd[i]["time"]);
    
  }

  //Init running time and command index
  _runningTime = millis();
  nextCmdIndex = 0;

  runState = STATE_RUN;

  //Set Up LED 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
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

void runMotor(int _index, int _dir, int _speed)
{

  printMotorIndex(_index);
  printDirection(_dir);
  Serial.println(_speed);
  
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

void testLoop() {

  runMotor(BASS_1_MOUTH, FORWARD, 255);
  runMotor(BASS_2_MOUTH, FORWARD, 255);
  delay(1000);
  
  runMotor(BASS_1_MOUTH, RELEASE, 0);
  runMotor(BASS_2_MOUTH, RELEASE, 0);
  delay(1000);

  runMotor(BASS_1_TAIL, BACKWARD, 255);
  runMotor(BASS_2_TAIL, BACKWARD, 255);
  delay(1000);

  runMotor(BASS_1_TAIL, FORWARD, 255);
  runMotor(BASS_2_TAIL, FORWARD, 255);
  delay(1000);

  runMotor(BASS_1_TAIL, RELEASE, 0);
  runMotor(BASS_2_TAIL, RELEASE, 0);
  delay(1000);
  
}


void loop() {

  if (runState == STATE_RUN)
  {

    unsigned long _n = 1*_runningTime + 1*arrCmd[nextCmdIndex]._time;

    Serial.print(millis());
    Serial.print(" run> ");
    Serial.println(_runningTime);
    Serial.print(" time> ");
    Serial.println(arrCmd[nextCmdIndex]._time);
    Serial.print(" comb> ");
    Serial.println(_n);

    if ((millis() > _n) ) {

      //Handle commands
      bool b1 = false;
      bool b2 = false;

      
      Serial.println(arrCmd[nextCmdIndex]._cmd);
      Serial.println(arrCmd[nextCmdIndex]._b1);
      Serial.println(arrCmd[nextCmdIndex]._b2);

      switch (arrCmd[nextCmdIndex]._cmd) {

        case CMD_OPEN:

          if (arrCmd[nextCmdIndex]._b1)
            runMotor(BASS_1_MOUTH, FORWARD, 255);
          if (arrCmd[nextCmdIndex]._b2)
            runMotor(BASS_2_MOUTH, FORWARD, 255);
          break;

        case CMD_CLOSE:

          if (arrCmd[nextCmdIndex]._b1)
            runMotor(BASS_1_MOUTH, RELEASE, 0);
          if (arrCmd[nextCmdIndex]._b2)
            runMotor(BASS_2_MOUTH, RELEASE, 0);
          break;

        case CMD_TAIL_ON:

          if (arrCmd[nextCmdIndex]._b1)
            runMotor(BASS_1_TAIL, BACKWARD, 255);
          if (arrCmd[nextCmdIndex]._b2)
            runMotor(BASS_2_TAIL, BACKWARD, 255);
          break;

        case CMD_HEAD_ON:

          if (arrCmd[nextCmdIndex]._b1)
            runMotor(BASS_1_TAIL, FORWARD, 255);
          if (arrCmd[nextCmdIndex]._b2)
            runMotor(BASS_2_TAIL, FORWARD, 255);
          break;
 
        case CMD_BODY_OFF:

          if (arrCmd[nextCmdIndex]._b1)
            runMotor(BASS_1_TAIL, RELEASE, 0);
          if (arrCmd[nextCmdIndex]._b2)
            runMotor(BASS_2_TAIL, RELEASE, 0);
          break;
      }

      /*
      Serial.println((_runningTime + arrCmd[nextCmdIndex]._time));
      Serial.println(millis());
      Serial.println(nextCmdIndex);
      Serial.println();
      */
      
      nextCmdIndex++;

      if (nextCmdIndex >= numCmd) {
        runState = STATE_WAIT;
        nextCmdIndex = 0;
        _runningTime = millis();
        
        delay(1000);
        runState = STATE_RUN;
      }
    }
  
  }
  else if (runState == STATE_WAIT) 
  {
    
  }
  else
  {

    /*
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
*/

  }

}

void serialEvent() {
 
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();

    //blink = !blink;
    //Serial.println(blink);
 
    if (inChar == 'a') {
      
       digitalWrite(LED_BUILTIN, HIGH);
       
    }

    if (inChar == 'b') {
      
       digitalWrite(LED_BUILTIN, LOW);
       
    }    
  }

}



  
