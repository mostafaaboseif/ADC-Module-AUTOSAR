#include "adc.h"

extern AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS];

void Adc_init(AdcChannelGroup AdcChannelGroup)
{
	if(AdcChannelGroup.AdcModule==ADC0)
	{	SYSCTL_RCGCADC_R |= 0x1;
		while(!(SYSCTL_PRADC_R |= 0x1));
		NVIC_EN0_R |= (1<<(14+AdcChannelGroup.Sequencer));
	}
	else if(AdcChannelGroup.AdcModule==ADC1)
	{	SYSCTL_RCGCADC_R |= 0x2;
		while(!(SYSCTL_PRADC_R |= 0x2));
		NVIC_EN1_R |= (1<<(16+AdcChannelGroup.Sequencer));
	}
	
	Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) &= ~(1<<AdcChannelGroup.Sequencer);
	Memory(AdcChannelGroup.AdcModule,ADC_EMUX) |= (AdcChannelGroup.HwTrigger<<(AdcChannelGroup.Sequencer*4)) ; 
	Memory(AdcChannelGroup.AdcModule,ADC_IM) |= (1<<AdcChannelGroup.Sequencer);
	
	for(int i=0;i<AdcChannelGroup.NbChannels;i++)
	{
		Memory(AdcChannelGroup.AdcModule, ADC_SS_BASE + ADC_SS_STEP * AdcChannelGroup.Sequencer + ADC_SSMUX)
					|= (AdcChannelGroup.ArrayOfAdcChannels[i]<<(4*i));
		Memory(AdcChannelGroup.AdcModule,ADC_SS_BASE + ADC_SS_STEP * AdcChannelGroup.Sequencer + ADC_SSCTL)
					|= (0x06<<(4*i));
	}	
	
	//Memory(AdcChannelGroup.AdcModule,ADC_SSMUX3) |= AdcChannelGroup.InputPin;
	//Memory(AdcChannelGroup.AdcModule,ADC_SSCTL3) |= 0x06;
	
	Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) |= (1<<AdcChannelGroup.Sequencer);
	Memory(AdcChannelGroup.AdcModule,ADC_ISC) |= (1<<AdcChannelGroup.Sequencer);

}

//volatile uint32_t adcResult;

//void Adc_SetupResultBuffer ( AdcModule AdcModule, volatile uint32_t *buffer_ptr )
//{
//	volatile aa =adcResult;
//	*buffer_ptr = aa;
//}

//void ADC0SS3_Handler()
//{
//		adcResult = ADC0_SSFIFO3_R;
//		ADC0_ISC_R = (1<<3);
//}

//void Adc_StartGroupConversion ( Adc_GroupType Group ){	
//	//to get the sequencer of the given group ID
//	Sequencer Current_Sequencer;
//	for( int i=0 ;i <= 8; i++){
//		if ( ArrayOfAdcChannelGroups[i].GroupId == Group){
//			Current_Sequencer= ArrayOfAdcChannelGroups[i].Sequencer;
//			break;
//		}
//	}
//	ADC0_PSSI_R = (1<< Current_Sequencer);
//}
