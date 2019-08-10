#include <Arduino.h>
#include <Adafruit_MotorShield.h>

#define MOUTH_MOTOR 0
#define BODY_MOTOR 1



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

    Bass(
      Adafruit_MotorShield *_AFMS,
      int _mouthMotorIndex,
      int _bodyMotorIndex,
      int _mouthDirection,
      int _bodyDirection
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
      runMotor(MOUTH_MOTOR, MOUTH_OPEN, 255);
    }

    void mouthClose() {
      runMotor(MOUTH_MOTOR, RELEASE, 0);
    }
 
    void runBody(int _dir, int _speed)
    {
      runMotor(BODY_MOTOR, _dir, _speed);
    }

    void bodyTail() {
      runMotor(BODY_MOTOR, BODY_TAIL, 255);
    }

    void bodyHead() {
      runMotor(BODY_MOTOR, BODY_HEAD, 255);
    }
    
    void runMotor(int _index, int _dir, int _speed)
    { 
      _motor[_index]->run(_dir);
      _motor[_index]->setSpeed(_speed);  
    }

};
