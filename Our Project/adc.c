#include "adc.h"


void Adc_init(AdcChannel AdcChannel, HwTrigger HwTrigger)
{
	if(AdcChannel.AdcModule==ADC0)
	{	SYSCTL_RCGCADC_R |= 0x1;
		while(!(SYSCTL_PRADC_R |= 0x1));
	}
	else if(AdcChannel.AdcModule==ADC1)
	{	SYSCTL_RCGCADC_R |= 0x2;
		while(!(SYSCTL_PRADC_R |= 0x2));
	}
	
	Memory(AdcChannel.AdcModule,ADC_ACTSS) &= ~(1<<AdcChannel.Sequencer);
	Memory(AdcChannel.AdcModule,ADC_EMUX) |= (HwTrigger<<(AdcChannel.Sequencer*4)) ; 
	Memory(AdcChannel.AdcModule,ADC_IM) |= (1<<AdcChannel.Sequencer);
	if(AdcChannel.Sequencer==SS3)
	{	Memory(AdcChannel.AdcModule,ADC_SSMUX3) = AdcChannel.AdcInputPin;
		Memory(AdcChannel.AdcModule,ADC_SSCTL3) = 0x06;
		NVIC_EN0_R |= (1<<17);
	}
	Memory(AdcChannel.AdcModule,ADC_ACTSS) |= (1<<AdcChannel.Sequencer);
	Memory(AdcChannel.AdcModule,ADC_ISC) |= (1<<AdcChannel.Sequencer);

}

volatile uint32_t adcResult;

void Adc_SetupResultBuffer ( AdcModule AdcModule, volatile uint32_t *buffer_ptr )
{
	volatile aa =adcResult;
	*buffer_ptr = aa;
}

void ADC0SS3_Handler()
{
		adcResult = ADC0_SSFIFO3_R;
		ADC0_ISC_R = (1<<3);
}

