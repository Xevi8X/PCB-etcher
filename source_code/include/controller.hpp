#include <float.h>

//Configuration
//==================================================

//BANG-BANG
#define Hysteresis_init 1.0f

//PID
#define Kp_init 58.5f
#define Ki_init 0.1f
#define Kd_init 0.0f

//==================================================


void initControllers();
void controllerWork();
void clearControllers();

