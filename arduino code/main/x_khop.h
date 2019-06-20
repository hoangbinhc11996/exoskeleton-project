#ifndef x_khop_h
#define x_khop_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Encoder.h"
#include "AccelStepper.h"
#include "endstop.h"



//void ctht_min_handler_debug()
//{
//  Serial.println("vao day duoc la gioi");
//}

class x_khop
{
  private:
    Encoder *_encoder;
    AccelStepper *_stepper;
    endstop *_endstop_max;
    endstop *_endstop_min;

    double _max_position;
    double _current_position;
    double _set_positon;
    bool _is_max_position;
    bool _is_min_position;
    double _max_velocity;
    double _max_acceleration;
    double _velocity;
    double _max_peed;
    double _acceleration;
    long _stepper_pulse_per_round;
    long _encoder_pulse_per_round;
  public:


  public:
    x_khop();
    x_khop(Encoder encoder, AccelStepper stepper, endstop endstop_max, endstop endstop_min,
           long stepper_pulse_per_round, long encoder_pulse_per_round);
    x_khop(uint8_t encoder_A_pin, uint8_t encoder_B_pin,
           uint8_t stepper_step_pin, uint8_t stepper_dir_pin, uint8_t endstop_max_pin, uint8_t endstop_min_pin,
           long stepper_pulse_per_round, long encoder_pulse_per_round);
    ~x_khop();

    // get

    double get_position();
    bool is_max_position();
    bool is_min_position();
    bool is_running();

    // set
    void setEnablePin(uint8_t enable_pin);
    void setDirectionInverted();
    void setPosition(double position);
    void setAcceleration(float acceleration);
    void setSpeed(float speed);
    void setMaxSpeed(float speed);
    float maxSpeed();
    float speed();
    void moveTo(long absolute);
    void move(long relative);
    bool run();
    float distanceToGo();
    void stop();
    void home();
    void updateEndstop();
    long stp_targetPosition();
    long stp_currentPosition();  

};

x_khop::x_khop(Encoder encoder, AccelStepper stepper, endstop endstop_max, endstop endstop_min,
               long stepper_pulse_per_round, long encoder_pulse_per_round) {
  _encoder = &encoder;
  _stepper = &stepper;
  _endstop_min = &endstop_min;
  _endstop_max = &endstop_max;
  _stepper_pulse_per_round = stepper_pulse_per_round;
  _encoder_pulse_per_round = encoder_pulse_per_round;
}

x_khop::x_khop(uint8_t encoder_A_pin, uint8_t encoder_B_pin,
               uint8_t stepper_step_pin, uint8_t stepper_dir_pin, uint8_t endstop_max_pin, uint8_t endstop_min_pin,
               long stepper_pulse_per_round, long encoder_pulse_per_round) {
  _encoder = new Encoder(encoder_A_pin, encoder_B_pin);
  _stepper = new AccelStepper(1, stepper_step_pin, stepper_dir_pin);
  _endstop_min = new endstop(endstop_min_pin);
  _endstop_max = new endstop(endstop_max_pin);
  _stepper_pulse_per_round = stepper_pulse_per_round;
  _encoder_pulse_per_round = encoder_pulse_per_round;
  //_endstop_min->addHandler(ctht_min_handler_debug);
}

x_khop::~x_khop() {
  delete[] _encoder;
  delete[] _stepper;
  delete[] _endstop_min;
  delete[] _endstop_max;
}

double x_khop::get_position()
{
  return _stepper->currentPosition() * 360.0 / _stepper_pulse_per_round;
}

bool x_khop::is_max_position()
{
  return _is_max_position;
}

bool x_khop::is_min_position()
{
  return _is_min_position;
}

bool x_khop::is_running()
{
  _stepper->isRunning();
}


void x_khop::setEnablePin(uint8_t enable_pin)
{
  _stepper->setEnablePin(enable_pin);
}

void x_khop::setDirectionInverted()
{
  _stepper->setPinsInverted(true, false, false);
}

void x_khop::setAcceleration(float acceleration)
{
  float stepper_acceleration = acceleration * _stepper_pulse_per_round / 360.0;
  _stepper->setAcceleration(stepper_acceleration);
  _acceleration = stepper_acceleration;
}

void x_khop::setSpeed(float speed)
{
  float stepper_speed = speed * _stepper_pulse_per_round / 360.0;
  _stepper->setSpeed(stepper_speed);
}

void x_khop::setMaxSpeed(float speed)
{

  float stepper_speed = speed * _stepper_pulse_per_round / 360.0;
  _stepper->setMaxSpeed(stepper_speed);
  _max_peed = stepper_speed;
}

void x_khop::stop()
{
  _stepper->setAcceleration(100000);
  _stepper->setMaxSpeed(0);
  _stepper->stop();
  _stepper->setAcceleration(_acceleration);
  _stepper->setMaxSpeed(_max_peed);
}

void x_khop::setPosition(double position)
{
  long steper_pos = position * _stepper_pulse_per_round / 360;
  long encoder_pos = position * _encoder_pulse_per_round / 360;
  _encoder->write(encoder_pos);
  _stepper->setCurrentPosition(steper_pos);
}
void x_khop::moveTo(long absolute)
{
  if (absolute < 0) return;
  long steper_pos = absolute * _stepper_pulse_per_round / 360.0;
  _stepper->moveTo(steper_pos);
}

void x_khop::move(long relative)
{
  long steper_pos = relative * _stepper_pulse_per_round / 360.0;
  _stepper->move(steper_pos);
}

boolean x_khop::run()
{
  _stepper->run();
}

float x_khop::maxSpeed()
{
  return (_stepper->maxSpeed() / (float)_stepper_pulse_per_round) * 360.0;
}

float x_khop::speed()
{
  return _stepper->speed();
}

void x_khop::home()
{
  _stepper->setAcceleration(100000);
  _stepper->setMaxSpeed(20000);
  _stepper->moveTo(-1000000);
  while (!_endstop_min->m_hited)
  {
    this->run();
  }
  this->stop();
  this->setPosition(0);
  _stepper->setAcceleration(_acceleration);
  _stepper->setMaxSpeed(_max_peed);
}

void x_khop::updateEndstop()
{
  _endstop_min->update();
  _endstop_max->update();
  if (_endstop_min->m_hited)
  {
    //this->stop();
  }
  if (_endstop_max->m_hited)
  {
    this->stop();
  }
}

long x_khop::stp_targetPosition()
{
  return _stepper->targetPosition();
}

long x_khop::stp_currentPosition(){
  return _stepper->currentPosition();
}

#endif
