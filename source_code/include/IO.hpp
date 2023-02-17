//Configuration
//==================================================
#define TRIAC_DELAY 3 //in ms, selected experimentally
//==================================================

//PINOUT
//==================================================
#define ONE_WIRE_PIN 2
#define TRIAC_PIN 0
#define ZERO_DETECTION_PIN 3
//==================================================

void IO_init();
void measure();
