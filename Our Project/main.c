#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test()
{
	GPIO_writePin(PORTF,PIN1,HIGH);
}

int main()
{
	TIMER_initAdc(100);
	//TIMER_initInterrupt(5,test);

	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
	
	AdcChannel AdcChannel = {ADC0,SS3,PE3};
	
	Adc_init(AdcChannel ,ADC_TRIGGER_TIMER);
	
	GPIO_initPin(PORTF,PIN1,DIGITAL,OUTPUT);
	GPIO_initPin(PORTF,PIN2,DIGITAL,OUTPUT);
	GPIO_initPin(PORTF,PIN0,DIGITAL,INPUT);
	//GPIO_initInterruptAdc(PORTF,PIN0,FALLING);
	
	//UART_init(UART1,9600);
	
	volatile uint32_t x;
	Adc_SetupResultBuffer(ADC0,&x);
	
	volatile int y ;
	while(1)
	{
		y =x ;
		//ADC0_PSSI_R = 0x08;
	//	UART_sendInt(UART1,12);		
	//UART_sendString(UART1,"yaraab\n");
	}
}

