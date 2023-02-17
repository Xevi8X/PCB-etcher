#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "program_state.h"
#include "IO.hpp"


OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
extern State state;


void IRAM_ATTR zeroCrossingISR()
{
  digitalWrite(TRIAC_PIN,HIGH);
  if(state.power == 0) return;
  uint32_t ticks = 12 * (255-state.power) + TRIAC_DELAY; // (10ms/255)/3.2us ~= 12 
  timer1_write(ticks);
}

void IRAM_ATTR timerISR()
{
  digitalWrite(TRIAC_PIN,LOW);
}

void IO_init()
{
  //===IO===
  pinMode(TRIAC_PIN, OUTPUT);
  digitalWrite(TRIAC_PIN,HIGH);
  pinMode(ZERO_DETECTION_PIN, FUNCTION_3);
  attachInterrupt(digitalPinToInterrupt(ZERO_DETECTION_PIN),zeroCrossingISR,FALLING);
  //===DS18B20
  sensors.begin();
  sensors.setResolution(11); //0.125°C, 375ms
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  //===TIMER===
  timer1_isr_init();
  timer1_attachInterrupt(timerISR);        
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
}


void measure()
{
  state.actualTemp = sensors.getTempCByIndex(0);
  sensors.requestTemperatures();
  if(state.actualTemp == DEVICE_DISCONNECTED_C) 
  {
    state.heating = Heating::ERROR;
    state.actualTemp = 0.0f;
  }
  else
  {
    if(state.heating == Heating::ERROR && state.actualTemp < 100.0f && state.actualTemp > 0.0f) state.heating = Heating::OFF;
  }

}