#include"x_khop.h"
#include "SerialCommand.h"
#include "AccelStepper.h"
#include "pins.h"
#include "Encoder.h"
#include "endstop.h"
#include "x_chan.h"
#include "TimerOne.h"

#define STEPPER_PULSE_PER_ROUND 3200L*18
#define ENCODER_PULSE_PER_ROUND 400L*2*61/14

#define KHOP_B
#define ECHO_POSITION Serial.print("A" + String(khop_dui.get_position()) + " " + "B" + String(khop_dau_goi.get_position()) + "\n");

SerialCommand SCmd;
AccelStepper A_stepper(1, A_STEP_PIN, A_DIR_PIN);
Encoder A_encoder(A_ENCODER_A, A_ENCODER_B);
endstop A_endstop_max(A_MAX_PIN);
endstop A_endstop_min(A_MIN_PIN);
//
//#ifdef KHOP_B
AccelStepper B_stepper(1, B_STEP_PIN, B_DIR_PIN);
Encoder B_encoder(B_ENCODER_A, B_ENCODER_B);
endstop B_endstop_max(B_MAX_PIN);
endstop B_endstop_min(B_MIN_PIN);
//#endif

long now = millis();
long update_time = 50;
double A_des_Position = 0;
double B_des_Position = 0;
float A_speed = 15;
float B_speed = 20;
float A_ACCEL = 15;
float B_ACCEL = 10;

bool stop_auto = false;

x_khop khop_dau_goi(A_encoder, A_stepper, A_endstop_max, A_endstop_min, STEPPER_PULSE_PER_ROUND, ENCODER_PULSE_PER_ROUND);
//x_khop khop_dau_goi(A_ENCODER_A, A_ENCODER_B, A_STEP_PIN, A_DIR_PIN, A_MAX_PIN, A_MIN_PIN, STEPPER_PULSE_PER_ROUND, ENCODER_PULSE_PER_ROUND);

#ifdef KHOP_B
#define B_STEPPER_PULSE_PER_ROUND 3200L*19
#define B_ENCODER_PULSE_PER_ROUND 400L*2*61/14
//x_khop khop_dui(B_ENCODER_A, B_ENCODER_B, B_STEP_PIN, B_DIR_PIN, B_MAX_PIN, B_MIN_PIN, B_STEPPER_PULSE_PER_ROUND, B_ENCODER_PULSE_PER_ROUND);
//x_khop khop_dui(B_ENCODER_A, B_ENCODER_B, B_STEP_PIN, B_DIR_PIN, B_MAX_PIN, B_MIN_PIN, B_STEPPER_PULSE_PER_ROUND, B_ENCODER_PULSE_PER_ROUND);
x_khop khop_dui(B_encoder, B_stepper, B_endstop_max, B_endstop_min, B_STEPPER_PULSE_PER_ROUND, B_ENCODER_PULSE_PER_ROUND);
#endif

x_chan chan_trai(khop_dui, khop_dau_goi);

void timer_interrupt() {
  if (millis() - now >= update_time)
  {
    khop_dau_goi.updateEndstop();
    khop_dui.updateEndstop();
    if (chan_trai.is_running() == true)
    {
      Serial.print("A" + String(khop_dui.get_position()) + " " + "B" + String(khop_dau_goi.get_position()) + "\n");
      //Serial.print(" "); Serial.print("B");  Serial.print(khop_dui.get_position());
    }

#ifdef KHOP_B
    //        if (khop_dui.stp_currentPosition() != khop_dui.stp_targetPosition())
    //        {
    //          Serial.print(" "); Serial.print("B");  Serial.print(khop_dui.get_position());
    //        }

#endif
    if (khop_dau_goi.stp_currentPosition() == khop_dau_goi.stp_targetPosition()
#ifdef KHOP_B
        && khop_dui.stp_currentPosition() == khop_dui.stp_targetPosition()
#endif
       ) {
      //Serial.print("done");
    }
    //Serial.println();

    now = millis();
  }
}

void MOVE()
{
  char *arg;

  arg = SCmd.next();
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("A"))
    {
      String tmp = Str.substring(1);
      A_des_Position = tmp.toDouble();
    }

  }

  arg = SCmd.next();
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("B"))
    {
      String tmp = Str.substring(1);
      B_des_Position = tmp.toDouble();
    }
  }
  chan_trai.moveTo(A_des_Position, B_des_Position);
  chan_trai.runToPosition();
  ECHO_POSITION
  Serial.print("MOVE done\n");
}

void HOME()
{
  char *arg;
  arg = SCmd.next();

  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("A"))
    {
      chan_trai.home(true, false);
    }

  }
  arg = SCmd.next();

  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("B"))
    {
      chan_trai.home(false, true);
    }

  }
  ECHO_POSITION
  Serial.print("HOME done\n");
}
void SPEED()
{
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("A"))
    {
      String tmp = Str.substring(1);
      A_speed = tmp.toFloat();
    }
  }

  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("B"))
    {
      String tmp = Str.substring(1);
      B_speed = tmp.toFloat();
    }
  }

  //  khop_dau_goi.setMaxSpeed(A_speed);
  //#ifdef KHOP_B
  //  khop_dui.setMaxSpeed(B_speed);
  //#endif

  chan_trai.setMaxSpeed(A_speed, B_speed);
  Serial.print("SPEED done\n");
}

void ACCEL()
{
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("A"))
    {
      String tmp = Str.substring(1);
      A_ACCEL = tmp.toFloat();
    }

  }
  arg = SCmd.next();
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("B"))
    {
      String tmp = Str.substring(1);
      B_ACCEL = tmp.toFloat();
    }

  }

  //  khop_dau_goi.setAcceleration(A_ACCEL);
  //#ifdef KHOP_B
  //  khop_dui.setAcceleration(A_ACCEL);
  //#endif
  chan_trai.setAcceleration(A_ACCEL, B_ACCEL);
  Serial.print("ACCEL done\n");
}

void AUTO()
{
  char *arg;
  arg = SCmd.next();
  while (1)
  {
    if (stop_auto){
      stop_auto = false;
      break;
    }
    chan_trai.moveTo(0, 0);
    chan_trai.runToPosition();
    chan_trai.moveTo(90, 60);
    chan_trai.runToPosition();
  }
}

void STOPAUTO()
{
  char *arg;
  arg = SCmd.next();
  stop_auto = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {}
  SCmd.addCommand("MOVE", MOVE);
  SCmd.addCommand("HOME", HOME);
  SCmd.addCommand("SPEED", SPEED);
  SCmd.addCommand("ACCEL", ACCEL);
  SCmd.addCommand("AUTO", AUTO);
  SCmd.addCommand("STOPAUTO", STOPAUTO);
  Timer1.initialize(50000);
  Timer1.attachInterrupt(timer_interrupt);
  khop_dau_goi.setAcceleration(B_ACCEL);
  khop_dau_goi.setMaxSpeed(B_speed);
  khop_dui.setAcceleration(A_ACCEL);
  khop_dui.setMaxSpeed(A_speed);

}

String str = "";

void loop() {
  // put your main code here, to run repeatedly:
  SCmd.readSerial();
  khop_dau_goi.run();
  khop_dui.run();
}
