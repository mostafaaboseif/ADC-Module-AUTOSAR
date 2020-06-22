#include "timer.h"

void (*callback)();

void TIMER_initAdc(int samplePeriod) {
/* initialize timer 0 to trigger ADC at 1 sample/sec */
SYSCTL_RCGCTIMER_R |= 1; /* enable clock to Timer Block 0 */
while(!(SYSCTL_PRTIMER_R |= 0x1));
TIMER0_CTL_R = 0; /* disable WTimer before initialization */
TIMER0_CFG_R = 0x04; /* 16-bit option */
TIMER0_TAMR_R = 0x02; /* periodic mode and down-counter */
TIMER0_TAILR_R = samplePeriod*CLK_SPEED/1000; /* Timer A interval load value reg (1 s) */
TIMER0_CTL_R |= 0x20; /* timer triggers ADC */
TIMER0_CTL_R |= 0x01; /* enable Timer A after initialization */
}

void TIMER_initInterrupt(int callbackPeriod, void (*callbackFn)()) {
SYSCTL_RCGCTIMER_R |= 2; 
while(!(SYSCTL_PRTIMER_R |= 0x1));
TIMER1_CTL_R = 0; 
TIMER1_CFG_R = 0x0; //16-bit
TIMER1_TAMR_R = 0x02; 
TIMER1_TAILR_R = callbackPeriod*CLK_SPEED; 
TIMER1_IMR_R = 0x01;
TIMER1_CTL_R |= 0x20; 
TIMER1_CTL_R |= 0x01; 
NVIC_EN0_R |= (1<<21);
callback=callbackFn;
}

void TIMER1A_Handler()
{
	callback();
	TIMER1_ICR_R = 0x01;
}
