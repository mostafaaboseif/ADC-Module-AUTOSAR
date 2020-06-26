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

//	volatile Adc_ValueGroupType* buffer_ptr1 = NULL;
//	volatile Adc_ValueGroupType* buffer_ptr2 = NULL;
//	
//	Adc_SetupResultBuffer (0 , &buffer_ptr1 );
//	Adc_SetupResultBuffer (1 , &buffer_ptr2 );
//	
//	volatile Adc_ValueGroupType* buffer_ptr3 = buffer_ptr1++;
	
	volatile Adc_ValueGroupType** PtrToSamplePtr;
	Adc_ValueGroupType G0_ResultBuffer[2] ,G1_ResultBuffer[1] ; 
	if(Adc_SetupResultBuffer(1,G1_ResultBuffer)!=E_OK)
	{
		// raising an error
	}
	while(1)
	{
		
	//	Adc_StartGroupConversion(0);
		Adc_StartGroupConversion(1);

		//Adc_GetStreamLastPointer(1,PtrToSamplePtr);
	
	//	UART_sendInt(UART1,Adc_GetStreamLastPointer(1,PtrToSamplePtr));

//			UART_sendString(UART1,"channel 0 sample: \r\n");
//  		UART_sendInt(UART1,G1_ResultBuffer[0]);
/*
		UART_sendString(UART1,"channel 1 sample: \r\n");
		UART_sendInt(UART1,G0_ResultBuffer[1]);
*/ 
//			
		
		
//		for(int i=0 ; i<3 ; i++)
//		{	
//			UART_sendString(UART1,"channel 1 sample: ");
//			UART_sendInt(UART1,adcResult[2][0][i]);
//			UART_sendString(UART1,"channel 2 sample: ");
//			UART_sendInt(UART1,adcResult[2][1][i]);
//			UART_sendString(UART1,"channel 3 sample: ");
//			UART_sendInt(UART1,adcResult[1][0][i]);
//		}
		
//		UART_sendString(UART1,"1st Result: ");
//		UART_sendInt(UART1,*buffer_ptr1);		
//		UART_sendString(UART1,"2nd Result: ");
//		UART_sendInt(UART1,*buffer_ptr2);		
//		UART_sendString(UART1,"3rd Result: ");
//		UART_sendInt(UART1,*buffer_ptr3);			
	}
}

