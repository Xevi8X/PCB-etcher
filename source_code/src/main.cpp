#include <Arduino.h>

#include "config.h"
#include "host.hpp"
#include "IO.hpp"
#include "controller.hpp"



State state;
unsigned long previousMillis = 0;

void setup() 
{
  startServer();
  IO_init();
  initControllers();
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