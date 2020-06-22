#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"

void TIMER_initAdc(int samplePeriod);  //in ms
void TIMER_initInterrupt(int callbackPeriod, void (*callbackFn)());  //in sec
