#include <Arduino.h>

#include "host.hpp"
#include "IO.hpp"

//Configuration
//==================================================
#define MEASURE_PERIOD_IN_MILLIS 500
float hysteresis = 1.0f;
//==================================================

State state;
unsigned long previousMillis = 0;

void execHeating()
{
  if(state.heating == Heating::ON) state.power = 255;
  else state.power = 0;
}

void controllerWork()
{
  //simple bang-bang controller
  if(state.heating == Heating::ERROR)
  {
     execHeating();
     return;
  }

  float error = state.targetTemp - state.actualTemp;

  if(state.heating == Heating::OFF  && error > hysteresis)
  {
     state.heating = Heating::ON;
     execHeating();
     return;
  }
  if(state.heating == Heating::ON && error < -hysteresis)
  {
     state.heating = Heating::OFF;
     execHeating();
     return;
  }
}

void setup() 
{
  startServer();
  IO_init();
}

void loop() 
{
  handleClientsServer();
  unsigned long now = millis();
  if(now - previousMillis > MEASURE_PERIOD_IN_MILLIS)
  {
    previousMillis = now;
    measure();
    controllerWork();
  }
}