enum programStatus {WORKING, ERROR_TEMP_SENSOR};

typedef struct state_t
{
    float targetTemp = 40.0f;
    float actualTemp = 0.0f;
    uint8_t power = 0;
    programStatus status = programStatus::WORKING;
} State;