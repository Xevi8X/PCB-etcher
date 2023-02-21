#include "Arduino.h"

#include "controller.hpp"
#include "config.h"
#include "program_state.h"

typedef struct PID_data_t
{
    float integral = 0.0f;
    float last_val = FLT_MAX;
} PID_data_t;

extern State state;
PID_data_t pid_data;


void bangBangController()
{
  if(state.status != ProgramStatus::WORKING)
  {
    state.power = 0;
    return;
  }

  float error = state.targetTemp - state.actualTemp;

  if(state.power != 255  && error > state.controller_param.hysteresis)
  {
     state.power = 255;
     return;
  }
  if(state.power != 0 && error < -state.controller_param.hysteresis)
  {
     state.power = 0;
     return;
  }
}

void PID_Controller()
{
  if(state.status != ProgramStatus::WORKING)
  {
    state.power = 0;
    return;
  }

  float error = state.targetTemp - state.actualTemp; 
  //P
  float tmp = state.controller_param.Kp * error; 
  //I
  pid_data.integral += error * MEASURE_PERIOD_IN_MILLIS/1000.0f;
  tmp += state.controller_param.Ki*pid_data.integral;
  //D
  if(pid_data.last_val != FLT_MAX)
   tmp += state.controller_param.Kd* (error-pid_data.last_val)*1000.0f/MEASURE_PERIOD_IN_MILLIS;
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

  if(state.controller_param.antyWindUp)
  {
    //TODO: ANTY WINDUP
  }

  state.power = newPower;
}

void initControllers()
{
  state.controller_param.Kp = Kp_init;
  state.controller_param.Ki = Ki_init;
  state.controller_param.Kd = Kd_init;
  state.controller_param.hysteresis = Hysteresis_init;
  state.controller_param.antyWindUp = false;
}

void controllerWork()
{
  switch (state.controller)
  {
    case Controllers::NONE:
      break;

    case Controllers::BANG_BANG:
      bangBangController();
      break;

    case Controllers::PID:
      PID_Controller();
      break;
  }
}

void clearControllers()
{
  //Bang-bang
  //Nothing to clean
  //PID
  pid_data.integral = 0.0f;
  pid_data.last_val = FLT_MAX;
}
