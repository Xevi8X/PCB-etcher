enum Heating {ON, OFF, ERROR};

typedef struct state_t
{
    float targetTemp = 40.0f;
    float actualTemp = 0.0f;
    uint8_t power = 0;
    Heating heating = Heating::OFF;
} State;