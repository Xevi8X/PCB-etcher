enum ProgramStatus {WORKING, ERROR_TEMP_SENSOR,ERROR_OVERHEAT};

enum Controllers {BANG_BANG, PID};

typedef struct state_t
{
    float targetTemp = 40.0f;
    float actualTemp = 0.0f;
    uint8_t power = 0;
    ProgramStatus status = ProgramStatus::WORKING;
    Controllers controller = Controllers::PID;
} State;