#include <Arduino.h>
#include <SparkFun_TB6612.h>

#define MOUTH_MOTOR 0
#define BODY_MOTOR 1

//DEFINE DIRECTIONS
#define FORWARD 1
#define BACKWARD -1
#define RELEASE 0

#define TIMEOUT_MOUTH 30000
#define TIMEOUT_BODY 30000

const int offsetA = 1;
const int offsetB = 1;

const int CMD_OPEN = 0;
const int CMD_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 5;

const int CMD_LEARN_GROUP = 6;

const int STATE_MOUTH_OPEN = 0;
const int STATE_MOUTH_CLOSING = 1;
const int STATE_MOUTH_CLOSED = 2;

const int STATE_BODY_TAIL = 3;
const int STATE_BODY_HEAD = 4;
const int STATE_BODY_OFF = 5;
const int STATE_BODY_CLOSING = 6;

const int MOUTH_CLOSING_TIME = 1000;
const int BODY_CLOSING_TIME = 1000;

const int MAX_MOTOR = 255;

class Bass
{

  int id;

  int stateMouth = 0;
  int stateBody = 0;

  int mouthClosingIndex = 0;
  int bodyClosingIndex = 0;

  //Adafruit_MotorShield *AFMS;
  //Adafruit_DCMotor *_motor[2];
  Motor *_motor[2];

  int mouthMotorOrientation;
  int bodyMotorOrientation;

  long timerMouth = 0;
  long timerBody = 0;

  public:

    int MOUTH_OPEN = FORWARD;
    int MOUTH_CLOSE = BACKWARD;

    int BODY_TAIL = FORWARD;
    int BODY_HEAD = BACKWARD;

    int lastCommand;

    byte GROUP_ID;
  
    Bass(
      int AIN1, int AIN2, int PWMA, 
      int BIN1, int BIN2, int PWMB,
      bool _mouthDirection,
      bool _bodyDirection
    )
    {
        
        _motor[BODY_MOTOR] = new Motor(AIN1, AIN2, PWMA, offsetA);
        _motor[MOUTH_MOTOR] = new Motor(BIN1, BIN2, PWMB, offsetA);

        _motor[MOUTH_MOTOR]->brake();
        _motor[BODY_MOTOR]->brake();

        //If we need to flip the motor direction
        MOUTH_OPEN = FORWARD;
        MOUTH_CLOSE = BACKWARD;
        
        if (!_mouthDirection) {
          MOUTH_OPEN = BACKWARD;
          MOUTH_CLOSE = FORWARD;
        }

        BODY_TAIL = FORWARD;
        BODY_HEAD = BACKWARD;
          
        if (!_bodyDirection) {
          BODY_TAIL = BACKWARD;
          BODY_HEAD = FORWARD;
        }
 
    }


    void update() {
  
      if (stateMouth == STATE_MOUTH_CLOSING) {
        
        mouthClosingIndex = mouthClosingIndex + 1;
        if (mouthClosingIndex > MOUTH_CLOSING_TIME) {
          endMouthClose();
        }
      } else if (stateMouth == STATE_MOUTH_OPEN) {

        if (millis() > timerMouth + TIMEOUT_MOUTH) {
          mouthClose();
        }
      }

      if (stateBody == STATE_BODY_CLOSING) {

        bodyClosingIndex = bodyClosingIndex + 1;
 
        if (bodyClosingIndex > BODY_CLOSING_TIME) {
          endBodyClose(); 
        }

      } else if (stateBody == STATE_BODY_HEAD || stateBody == STATE_BODY_TAIL) {

        if (millis() > timerBody + TIMEOUT_BODY) {

          bodyClose();
          //runBody(RELEASE,MAX_MOTOR);
        }
      }

    }

    void addToGroup(int _i)
    {
        GROUP_ID = _i;
    }

    void runMouth(int _dir, int _speed)
    {
      runMotor(MOUTH_MOTOR, _dir, _speed);
    }

    void mouthOpen() {

      stateMouth = STATE_MOUTH_OPEN;
      
      //lastCommand = CMD_OPEN;
      runMotor(MOUTH_MOTOR, MOUTH_OPEN, MAX_MOTOR);

      timerMouth = millis();
    }

    void mouthClose() {

      //lastCommand = CMD_CLOSE;
      runMotor(MOUTH_MOTOR, MOUTH_CLOSE, MAX_MOTOR);
      //runMotor(MOUTH_MOTOR, RELEASE, 0);

      Serial.println("C");
      
      stateMouth = STATE_MOUTH_CLOSING;
      mouthClosingIndex = 0;
      
    }

    void bodyClose() {

      if (stateBody == STATE_BODY_TAIL) {
        runMotor(BODY_MOTOR, BODY_HEAD, MAX_MOTOR);
      } else {
        runMotor(BODY_MOTOR, BODY_TAIL, MAX_MOTOR);
      }
           
      stateBody = STATE_BODY_CLOSING;
      bodyClosingIndex = 0;

    }

    void endMouthClose() {

      stateMouth = STATE_MOUTH_CLOSED;
      mouthClosingIndex = 0;

      runBody(RELEASE,MAX_MOTOR);
    }

    void endBodyClose() {

      stateBody = STATE_BODY_OFF;
      bodyClosingIndex = 0;

      //
    }
    
    void runBody(int _dir, int _speed)
    {

      lastCommand = CMD_BODY_OFF;
      runMotor(BODY_MOTOR, _dir, _speed);

      stateBody = STATE_BODY_OFF;
    }

    void bodyTail() {

      //if (lastCommand != CMD_HEAD_ON) {
        lastCommand = CMD_TAIL_ON;
        runMotor(BODY_MOTOR, BODY_TAIL, MAX_MOTOR);
      //}

      stateBody = STATE_BODY_TAIL;
      
      timerBody = millis();
    }

    void bodyHead() {

      lastCommand = CMD_HEAD_ON;
      runMotor(BODY_MOTOR, BODY_HEAD, MAX_MOTOR);

      stateBody = STATE_BODY_HEAD;

      timerBody = millis();
    }
    
    void runMotor(int _index, int _dir, int _speed)
    { 

      stateBody = STATE_BODY_OFF;
      int _s = _dir * _speed;

      if (_dir == RELEASE) {

        //    Serial.println("RELEASE");
        _motor[_index]->brake();
        
      } else {

        //    Serial.println("Speed : ");
        //    Serial.println(_s);
        _motor[_index]->drive(_s);
      }
      
    }

};
