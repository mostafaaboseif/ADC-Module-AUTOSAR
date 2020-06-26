#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//volatile uint32_t adcResult1=0,adcResult2=0,adcResult3=0;

extern AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS];


int main()
{
	TIMER_initAdc(100);
	
	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTD,PIN2,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTE,PIN1,ANALOG,PERIPHERAL);
	
	Adc_init(ArrayOfAdcChannelGroups[0]);
	Adc_init(ArrayOfAdcChannelGroups[1]);
	
	UART_init(UART1,UART_BAUD_9600);

	volatile uint32_t* buffer_ptr1 = NULL;
	volatile uint32_t* buffer_ptr2 = NULL;
	
	Adc_SetupResultBuffer (0 , &buffer_ptr1 );
	Adc_SetupResultBuffer (1 , &buffer_ptr2 );
	
	
	volatile uint32_t* buffer_ptr3 = buffer_ptr1++;
	
	while(1)
	{
		Adc_StartGroupConversion(0);
		Adc_StartGroupConversion(1);

		UART_sendString(UART1,"1st Result: ");
		UART_sendInt(UART1,*buffer_ptr1);		
		UART_sendString(UART1,"2nd Result: ");
		UART_sendInt(UART1,*buffer_ptr2);		
		UART_sendString(UART1,"3rd Result: ");
		UART_sendInt(UART1,*buffer_ptr3);	
		
	}
}

