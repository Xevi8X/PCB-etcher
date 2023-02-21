enum ProgramStatus {WORKING, ERROR_TEMP_SENSOR,ERROR_OVERHEAT};

enum Controllers {NONE ,BANG_BANG, PID};

typedef struct Controller_param_t
{
    float Kp;
    float Ki;
    float Kd;
    float hysteresis;
    bool antyWindUp;
} Controller_param_t;

typedef struct state_t
{
    float targetTemp = 40.0f;
    float actualTemp = 0.0f;
    uint8_t power = 0;
    ProgramStatus status = ProgramStatus::WORKING;
    Controllers controller = Controllers::PID;
    Controller_param_t controller_param;
} State;