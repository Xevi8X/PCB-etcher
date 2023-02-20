#include "Arduino.h"

#include "controller.hpp"
#include "config.h"
#include "program_state.h"

extern State state;
PID_t pid_data;


void bangBangController()
{
  if(state.status != programStatus::WORKING)
  {
    state.power = 0;
    return;
  }

  float error = state.targetTemp - state.actualTemp;

  if(state.power != 255  && error > HYSTERESIS)
  {
     state.power = 255;
     return;
  }
  if(state.power != 0 && error < -HYSTERESIS)
  {
     state.power = 0;
     return;
  }
}

void PID_Controller()
{
  if(state.status != programStatus::WORKING)
  {
    state.power = 0;
    return;
  }

  float error = state.targetTemp - state.actualTemp; 
  //P
  float tmp = Kp * error; 
  //I
  pid_data.integral += error * MEASURE_PERIOD_IN_MILLIS/1000.0f;
  tmp += Ki*pid_data.integral;
  //D
  if(pid_data.last_val != FLT_MAX)
   tmp += Kd* (error-pid_data.last_val)*1000.0f/MEASURE_PERIOD_IN_MILLIS;
  pid_data.last_val = error;

  //SATURATION
  int newPower = (int)tmp;
  if(newPower > 255)
  {
    newPower = 255;
  }
  if(newPower < 0)
  {
    newPower = 0;
  }

  state.power = newPower;

}