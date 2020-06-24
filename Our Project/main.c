#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


volatile uint32_t adcResult1=0,adcResult2=0,adcResult3=0;
extern AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS];





int main()
{
	volatile uint32_t  buffer;
	
	TIMER_initAdc(100);
	
	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTD,PIN2,ANALOG,PERIPHERAL);
	
	Adc_init(ArrayOfAdcChannelGroups[0]);
	
	UART_init(UART1,UART_BAUD_9600);
		
		
	while(1)
	{
		 
		UART_sendString(UART1,"=Result: ");
		Adc_SetupResultBuffer (ADC0 ,SS0 , &buffer);	
		UART_sendInt(UART1,buffer);	
		
		
//		UART_sendString(UART1,"1st Result: ");
//		UART_sendInt(UART1,adcResult1);		
//		UART_sendString(UART1,"2nd Result: ");
//		UART_sendInt(UART1,adcResult2);		
		
	}
}

