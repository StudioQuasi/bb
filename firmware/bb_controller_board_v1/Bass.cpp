#include <Arduino.h>
#include <SparkFun_TB6612.h>

#define MOUTH_MOTOR 0
#define BODY_MOTOR 1

//DEFINE DIRECTIONS
#define FORWARD 1
#define BACKWARD -1
#define RELEASE 0

const int offsetA = 1;
const int offsetB = 1;

const int CMD_OPEN = 0;
const int CMD_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 5;

class Bass
{

  int id;

  //Adafruit_MotorShield *AFMS;
  //Adafruit_DCMotor *_motor[2];
  Motor *_motor[2];

  int mouthMotorOrientation;
  int bodyMotorOrientation;

  public:

    int MOUTH_OPEN = FORWARD;
    int MOUTH_CLOSE = BACKWARD;

    int BODY_TAIL = FORWARD;
    int BODY_HEAD = BACKWARD;

    int lastCommand;
    
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

/*
        _motor[MOUTH_MOTOR]->drive(255);
        _motor[BODY_MOTOR]->drive(255);
        delay(1000);
        
        _motor[MOUTH_MOTOR]->drive(-255);
        _motor[BODY_MOTOR]->drive(-255);
        delay(1000);
        
        _motor[MOUTH_MOTOR]->brake();
        _motor[BODY_MOTOR]->brake();
*/
    }

    void runMouth(int _dir, int _speed)
    {
      runMotor(MOUTH_MOTOR, _dir, _speed);
    }

    void mouthOpen() {

      lastCommand = CMD_OPEN;
      runMotor(MOUTH_MOTOR, MOUTH_OPEN, 255);
    }

    void mouthClose() {

      lastCommand = CMD_CLOSE;
      runMotor(MOUTH_MOTOR, RELEASE, 0);
    }
 
    void runBody(int _dir, int _speed)
    {

      lastCommand = CMD_BODY_OFF;
      runMotor(BODY_MOTOR, _dir, _speed);
    }

    void bodyTail() {

      //if (lastCommand != CMD_HEAD_ON) {
        lastCommand = CMD_TAIL_ON;
        runMotor(BODY_MOTOR, BODY_TAIL, 255);
      //}
    }

    void bodyHead() {

      lastCommand = CMD_HEAD_ON;
      runMotor(BODY_MOTOR, BODY_HEAD, 255);
    }
    
    void runMotor(int _index, int _dir, int _speed)
    { 

      int _s = _dir * _speed;

      //Serial.println("Drive Motor");
      //Serial.println(_index);

      
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
