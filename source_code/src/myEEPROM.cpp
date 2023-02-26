#include <Arduino.h>
#include <EEPROM.h>

#include "myEEPROM.hpp"
#include "program_state.h"

extern State state;

void EEPROM_Init()
{
    const int eeprom_size = sizeof(Controller_param_t);
    EEPROM.begin(eeprom_size);
}

void getControllerParams()
{
    EEPROM_Init();
    EEPROM.get(CONTROLLER_PARAMS_ADDRESS, state.controller_param);
    EEPROM.end();
}

void saveControllerParams()
{
    EEPROM_Init();
    EEPROM.put(CONTROLLER_PARAMS_ADDRESS, state.controller_param);
    EEPROM.end();
}