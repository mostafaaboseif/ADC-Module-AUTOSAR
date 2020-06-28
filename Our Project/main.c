#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Adc_ConfigType ArrayOfAdcChannels[MAX_NB_GROUPS];

extern volatile Adc_ValueGroupType adcResult[MAX_NB_GROUPS][MAX_NB_CHANNELS][MAX_NB_OF_SAMPLES_PER_CHANNEL];

//extern volatile Adc_ValueGroupType *resultBuffer[MAX_NB_GROUPS];


int main()
{	
		
	Adc_init(&ArrayOfAdcChannels[0]);
	Adc_init(&ArrayOfAdcChannels[1]);
	
	UART_init(UART1,UART_BAUD_9600);
	
	volatile Adc_ValueGroupType G0_ResultBuffer[6] ,G1_ResultBuffer[2] ; 
	volatile Adc_ValueGroupType G0_appBuffer[2] , G1_appBuffer[1]; 
	if(Adc_SetupResultBuffer(0,G0_ResultBuffer)!=E_OK){	}
	if(Adc_SetupResultBuffer(1,G1_ResultBuffer)!=E_OK){	}
	
	
	while(1)
	{
		Adc_StartGroupConversion(0);
		Adc_StartGroupConversion(1);

	//	Adc_GetStreamLastPointer ( 0,  PtrToSamplePtr );

	  if(Adc_ReadGroup(0,G0_appBuffer)!=E_OK){}
	  if(Adc_ReadGroup(1,G1_appBuffer)!=E_OK){}
		UART_sendString(UART1,"----------------------- \n");
		for(int i=0 ; i<6 ; i++)	
		{
			UART_sendInt(UART1,G0_ResultBuffer[i]);
		}			
		UART_sendString(UART1,"**** \n");
		for(int i=0 ; i<2 ; i++)	
		{
			UART_sendInt(UART1,G1_ResultBuffer[i]);
		}		
	}
}

