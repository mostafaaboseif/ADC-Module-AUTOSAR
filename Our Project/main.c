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

void ADC0SS0_Handler()
{
		adcResult1 = ADC0_SSFIFO0_R;
		ADC0_ISC_R = (1<<0);
}

void ADC1SS0_Handler()
{
		adcResult2 = ADC1_SSFIFO0_R;
		ADC1_ISC_R = (1<<0);
}

int main()
{
	TIMER_initAdc(100);
	
	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTD,PIN2,ANALOG,PERIPHERAL);
	
	AdcChannelGroup AdcChannelGroup1 = {ADC0, SS0, ADC_TRIGGER_PROCESSOR, 1, {PE3} };
	AdcChannelGroup AdcChannelGroup2 = {ADC1, SS0, ADC_TRIGGER_TIMER, 1, {PD2} };	

	Adc_init(AdcChannelGroup1);
	Adc_init(AdcChannelGroup2);
	
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

