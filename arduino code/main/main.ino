#include"x_khop.h"
#include "SerialCommand.h"
#include "AccelStepper.h"
#include "pins.h"
#include "Encoder.h"
#include "endstop.h"
#include <TimerOne.h>

#define STEPPER_PULSE_PER_ROUND 3200L*18
#define ENCODER_PULSE_PER_ROUND 400L*2*61/14

#define KHOP_B   

SerialCommand SCmd;
AccelStepper A_stepper(1, A_STEP_PIN, A_DIR_PIN);
Encoder A_encoder(A_ENCODER_A, A_ENCODER_B);
endstop A_endstop_max(A_MAX_PIN);
endstop A_endstop_min(A_MIN_PIN);

#ifdef KHOP_B
AccelStepper B_stepper(1, B_STEP_PIN, B_DIR_PIN);
Encoder B_encoder(B_ENCODER_A, B_ENCODER_B);
endstop B_endstop_max(B_MAX_PIN);
endstop B_endstop_min(B_MIN_PIN);
#endif

long now = millis();

//x_khop khop_dau_goi(A_encoder, A_stepper, A_endstop_max, A_endstop_min, STEPPER_PULSE_PER_ROUND, ENCODER_PULSE_PER_ROUND);
x_khop khop_dau_goi(A_ENCODER_A, A_ENCODER_B, A_STEP_PIN, A_DIR_PIN, A_MAX_PIN, A_MIN_PIN, STEPPER_PULSE_PER_ROUND, ENCODER_PULSE_PER_ROUND);

#ifdef KHOP_B
#define B_STEPPER_PULSE_PER_ROUND 3200L*18
#define B_ENCODER_PULSE_PER_ROUND 400L*2*61/14
x_khop khop_dui(B_ENCODER_A, B_ENCODER_B, B_STEP_PIN, B_DIR_PIN, B_MAX_PIN, B_MIN_PIN, B_STEPPER_PULSE_PER_ROUND, B_ENCODER_PULSE_PER_ROUND);
#endif


void timer_interrupt() {
  if (millis() - now >= 50)
  {
    khop_dau_goi.updateEndstop();
//        if (khop_dau_goi.stp_currentPosition() != khop_dau_goi.stp_targetPosition())
//        {
          //Serial.print("A" + String(khop_dau_goi.get_position()) + " " + "B" + String(khop_dui.get_position()) + "\n");  //Serial.print(khop_dau_goi.get_position());
          //Serial.print(" "); Serial.print("B");  Serial.print(khop_dui.get_position());
//        }
    
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
  double A_des_Position = 0;
#ifdef KHOP_B
  double B_des_Position = 0;
#endif
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
#ifdef KHOP_B
      B_des_Position = tmp.toDouble();
#endif

    }

  }
  digitalWrite(13, HIGH);
  khop_dau_goi.moveTo(A_des_Position);

#ifdef KHOP_B
  khop_dui.moveTo(B_des_Position);
#endif
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
      khop_dau_goi.home();
    }

  }
  arg = SCmd.next();

  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("B"))
    {
#ifdef KHOP_B
      khop_dui.home();
#endif
    }

  }
}

void SPEED()
{
  char *arg;
  arg = SCmd.next();
  float A_speed = 0;
#ifdef KHOP_B
  float B_speed = 0;
#endif
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("A"))
    {
      String tmp = Str.substring(1);
      A_speed = tmp.toFloat();
    }

  }
  arg = SCmd.next();
  if (arg != NULL)
  {
    String Str = String(arg);
    if (Str.startsWith("B"))
    {
      String tmp = Str.substring(1);
#ifdef KHOP_B
      B_speed = tmp.toFloat();
#endif
    }
  }

  khop_dau_goi.setMaxSpeed(A_speed);
#ifdef KHOP_B
  khop_dui.setMaxSpeed(B_speed);
#endif
}

void ACCEL()
{
  char *arg;
  arg = SCmd.next();
  float A_ACCEL = 0;
#ifdef KHOP_B
  float B_ACCEL = 0;
#endif
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
#ifdef KHOP_B
      B_ACCEL = tmp.toFloat();
#endif
    }

  }

  khop_dau_goi.setAcceleration(A_ACCEL);
#ifdef KHOP_B
  khop_dui.setAcceleration(A_ACCEL);
#endif

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {}
  SCmd.addCommand("MOVE", MOVE);
  SCmd.addCommand("HOME", HOME);
  SCmd.addCommand("SPEED", SPEED);
  SCmd.addCommand("ACCEL", ACCEL);
  Timer1.initialize(50000);
  Timer1.attachInterrupt(timer_interrupt);
  //pinMode(A_MAX_PIN, INPUT_PULLUP);
  //pinMode(A_MIN_PIN, INPUT_PULLUP);
  khop_dau_goi.setAcceleration(10);
  khop_dau_goi.setMaxSpeed(20);
#ifdef KHOP_B
  khop_dui.setAcceleration(10);
  khop_dui.setMaxSpeed(20);
#endif
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  SCmd.readSerial();
  khop_dau_goi.run();
#ifdef KHOP_B
  khop_dui.run();
#endif
}
