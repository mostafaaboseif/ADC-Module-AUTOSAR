#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile uint32_t adcResult1=0,adcResult2=0;


AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS] = {
{ADC0, SS0, ADC_TRIGGER_PROCESSOR, 1, {PE3} },
{ADC0, SS1, ADC_TRIGGER_TIMER, 1, {PD2} }
};


void ADC0SS0_Handler()
{
		adcResult1 = ADC0_SSFIFO0_R;
		ADC0_ISC_R = (1<<0);
}

void ADC0SS1_Handler()
{
		adcResult2 = ADC0_SSFIFO1_R;
		ADC0_ISC_R = (1<<1);
}

int main()
{
	TIMER_initAdc(100);
	
	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTD,PIN2,ANALOG,PERIPHERAL);
	
	Adc_init(ArrayOfAdcChannelGroups[0]);
	Adc_init(ArrayOfAdcChannelGroups[1]);
	
	UART_init(UART1,UART_BAUD_9600);
	
	while(1)
	{
		ADC0_PSSI_R = (1<<0);
		
		UART_sendString(UART1,"1st Result: ");
		UART_sendInt(UART1,adcResult1);		
		UART_sendString(UART1,"2nd Result: ");
		UART_sendInt(UART1,adcResult2);		
		
	}
}

