#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS];

extern volatile Adc_ValueGroupType adcResult[MAX_NB_GROUPS][MAX_NB_CHANNELS][MAX_NB_OF_SAMPLES_PER_CHANNEL];

int main()
{
	TIMER_initAdc(1000);
	
	GPIO_initPin(PORTD,PIN2,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTE,PIN1,ANALOG,PERIPHERAL);
	GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
	
	Adc_init(ArrayOfAdcChannelGroups[0]);
	Adc_init(ArrayOfAdcChannelGroups[1]);
	
	UART_init(UART1,UART_BAUD_9600);
	
	Adc_ValueGroupType G0_ResultBuffer[2] ,G1_ResultBuffer[1] ; 
	if(Adc_SetupResultBuffer(0,G0_ResultBuffer)!=E_OK){	}
	if(Adc_SetupResultBuffer(1,G1_ResultBuffer)!=E_OK){	}
	
	Adc_StartGroupConversion(0);
	Adc_StartGroupConversion(1);
	
	while(1)
	{
		
		
//		Adc_StartGroupConversion(1);

//		for(int i=0 ; i<3 ; i++)
//		{
			UART_sendString(UART1,"channel 1 sample: \n");
			UART_sendInt(UART1,G0_ResultBuffer[0]);
			UART_sendInt(UART1,G0_ResultBuffer[1]);
			UART_sendInt(UART1,G1_ResultBuffer[0]);
		
//		}
		
		
//		UART_sendString(UART1,"channel 1 sample: \n");
//		UART_sendInt(UART1,G0_ResultBuffer[0]);
//		UART_sendInt(UART1,G0_ResultBuffer[1]);
//		UART_sendInt(UART1,G1_ResultBuffer[0]);		
		
//		for(int i=0 ; i<3 ; i++)
//		{	
//			UART_sendString(UART1,"channel 1 sample: ");
//			UART_sendInt(UART1,adcResult[2][0][i]);
//			UART_sendString(UART1,"channel 2 sample: ");
//			UART_sendInt(UART1,adcResult[2][1][i]);
//			UART_sendString(UART1,"channel 3 sample: ");
//			UART_sendInt(UART1,adcResult[1][0][i]);
//		}
		
	}
}

