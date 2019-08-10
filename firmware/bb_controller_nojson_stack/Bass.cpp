#include <Arduino.h>
#include <Adafruit_MotorShield.h>

#define MOUTH_MOTOR 0
#define BODY_MOTOR 1

const int CMD_OPEN = 0;
const int CMD_CLOSE = 1;
const int CMD_TAIL_ON = 2;
const int CMD_HEAD_ON = 3;
const int CMD_BODY_OFF = 4;
const int CMD_TAIL_OFF = 5;

class Bass
{

  int id;

  Adafruit_MotorShield *AFMS;
  Adafruit_DCMotor *_motor[2];

  int mouthMotorOrientation;
  int bodyMotorOrientation;

  public:

    int MOUTH_OPEN = FORWARD;
    int MOUTH_CLOSE = BACKWARD;

    int BODY_TAIL = FORWARD;
    int BODY_HEAD = BACKWARD;

    int lastCommand;
    
    Bass(
      Adafruit_MotorShield *_AFMS,
      int _mouthMotorIndex,
      int _bodyMotorIndex,
      bool _mouthDirection,
      bool _bodyDirection
    )
    {
        AFMS = _AFMS;

        _motor[MOUTH_MOTOR] = AFMS->getMotor(_mouthMotorIndex);
        _motor[BODY_MOTOR] = AFMS->getMotor(_bodyMotorIndex);

        if (_mouthDirection) {
          MOUTH_OPEN = BACKWARD;
          MOUTH_CLOSE = FORWARD;
        }

        if (_bodyDirection) {
          BODY_TAIL = BACKWARD;
          BODY_HEAD = FORWARD;
        }

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

      if (lastCommand != CMD_HEAD_ON) {
        lastCommand = CMD_TAIL_ON;
        runMotor(BODY_MOTOR, BODY_TAIL, 255);
      }
    }

    void bodyHead() {

      lastCommand = CMD_HEAD_ON;
      runMotor(BODY_MOTOR, BODY_HEAD, 255);
    }
    
    void runMotor(int _index, int _dir, int _speed)
    { 
      _motor[_index]->run(_dir);
      _motor[_index]->setSpeed(_speed);  
    }

};
