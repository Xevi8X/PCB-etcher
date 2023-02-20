#include <float.h>

//Configuration
//==================================================

//BANG-BANG
#define HYSTERESIS 1.0f

//PID
#define Kp 1.0f
#define Ki 0.05f
#define Kd 0.1f

//==================================================

void bangBangController();
void PID_Controller();

typedef struct PID_t
{
    float integral = 0.0f;
    float last_val = FLT_MAX;
} PID_t;