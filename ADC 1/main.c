#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"

volatile int adcResult = 0;

void ADC0SS3_Handler()
{
		adcResult = ADC0_SSFIFO3_R;
		ADC0_ISC_R = (1<<3);
}

void test()
{
	GPIO_writePin(PORTF,PIN1,HIGH);
}

int main()
{
	TIMER_initAdc(100);
	TIMER_initInterrupt(5,test);

	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);

	SYSCTL_RCGCADC_R |= 0x1;//0x2;
	while(!(SYSCTL_PRADC_R |= 0x1));
	
	ADC0_ACTSS_R &= ~0x8; /* disable SS3 during configuration */
	ADC0_EMUX_R = (0x5<<12);//(0xF<<12);//0x0
	ADC0_SSMUX3_R = 0x0; /* get input from channel 0 */
	ADC0_SSCTL3_R |= 0x6; /* take one sample at a time, set flag at 1st sample */
	ADC0_IM_R = (1<<3);
	ADC0_ACTSS_R |= 0x8; /* enable ADC0 sequencer 3 */
	ADC0_ISC_R = (1<<3);
	NVIC_EN0_R |= (1<<17);
	
	GPIO_initPin(PORTF,PIN1,DIGITAL,OUTPUT);
	GPIO_initPin(PORTF,PIN2,DIGITAL,OUTPUT);
	GPIO_initPin(PORTF,PIN0,DIGITAL,INPUT);
	GPIO_initInterrupt(PORTF,PIN0,FALLING,test);

	while(1)
	{
	//ADC0_PSSI_R |= 0x8; /* start a conversion sequence 3 */

	
	
		if(adcResult>2048)
			GPIO_writePin(PORTF,PIN2,HIGH);
		else
			GPIO_writePin(PORTF,PIN2,LOW);
		
		
	
	}
}

