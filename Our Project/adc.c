
/* ---------------------------------- INCLUDES ---------------------------------- */

#include "adc.h"
#include "uart.h"
/* --------------------------- GLOBAL EXTERN VARIABLES --------------------------- */

extern AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS];

/* ----------------------------- APIs IMPLEMENTATION ----------------------------- */

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
		
		//disable sequencer
	Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) &= ~(1<<AdcChannelGroup.Sequencer);
	
		//enable interrupts
	Memory(AdcChannelGroup.AdcModule,ADC_IM) |= (1<<AdcChannelGroup.Sequencer);	
	
		//select trigger source
	if(AdcChannelGroup.Adc_TriggerSourceType == ADC_TRIG_SRC_SW)
		Memory(AdcChannelGroup.AdcModule,ADC_EMUX) |= (0<<(AdcChannelGroup.Sequencer*4)) ; 
	else if(AdcChannelGroup.Adc_TriggerSourceType == ADC_TRIG_SRC_HW)
		Memory(AdcChannelGroup.AdcModule,ADC_EMUX) |= (AdcChannelGroup.Adc_HwTriggerSourceType<<(AdcChannelGroup.Sequencer*4)) ;
	
		//assign analog input pins
	for(int i=0;i<AdcChannelGroup.NbChannels;i++)
	{
		Memory(AdcChannelGroup.AdcModule, ADC_SS_BASE + ADC_SS_STEP * AdcChannelGroup.Sequencer + ADC_SSMUX)
					|= (AdcChannelGroup.ArrayOfAdcChannels[i]<<(4*i));
		Memory(AdcChannelGroup.AdcModule,ADC_SS_BASE + ADC_SS_STEP * AdcChannelGroup.Sequencer + ADC_SSCTL)
					|= (0x04<<(4*i));
	}	
		Memory(AdcChannelGroup.AdcModule,ADC_SS_BASE + ADC_SS_STEP * AdcChannelGroup.Sequencer + ADC_SSCTL)
					|= (0x06<<(4*AdcChannelGroup.NbChannels));
		
		//assign priority for each group (sequencer)
	Memory(AdcChannelGroup.AdcModule,ADC_SSPRI) |= (AdcChannelGroup.GroupPriority<<(AdcChannelGroup.Sequencer*4));
	
		//enable sequencer
	Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) |= (1<<AdcChannelGroup.Sequencer);

}


extern volatile uint32_t adcResult[2][4];   // result [adc_number] [sequencer_number]
volatile uint32_t adcResult[2][4];

void ADC0SS0_Handler()
{
		adcResult[0][0] = ADC0_SSFIFO0_R; //change [] [] according to adc ,seq number 
		ADC0_ISC_R = (1<<0);   //clear interrupt pin by setting it 
}



void Adc_SetupResultBuffer ( AdcModule mod ,Sequencer s ,volatile uint32_t * buffer_ptr )
{
	if (mod==ADC0) 
 {  
		switch ((int)s)
		{
			case SS0 : (*buffer_ptr) = adcResult[0][0]; break;  
			case SS1 : (*buffer_ptr) ; break;
			case SS2 : (*buffer_ptr); break;
			case SS3 : (*buffer_ptr); break;

		}
		} 
 else if (mod==ADC1)
 {
	switch ((int)s)
		{
			case SS0 : (*buffer_ptr) ; break;
			case SS1 : (*buffer_ptr) ; break;
			case SS2 : (*buffer_ptr); break;
			case SS3 : (*buffer_ptr); break;

		}
 
 
 }
else;
}

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




#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)		
//to enable interrupts for a specific group
void Adc_EnableGroupNotification(int groupId)
{
	AdcChannelGroup AdcChannelGroup = ArrayOfAdcChannelGroups[groupId];
	Memory(AdcChannelGroup.AdcModule,ADC_IM) |= (1<<AdcChannelGroup.Sequencer);
	if(AdcChannelGroup.AdcModule==ADC0) 
		NVIC_EN0_R |= (1<<(14+AdcChannelGroup.Sequencer));
	else if(AdcChannelGroup.AdcModule==ADC1)	
		NVIC_EN1_R |= (1<<(16+AdcChannelGroup.Sequencer));	
}

//to disable interrupts for a specific group
void Adc_DisableGroupNotification(int groupId)
{
	AdcChannelGroup AdcChannelGroup = ArrayOfAdcChannelGroups[groupId];
	Memory(AdcChannelGroup.AdcModule,ADC_IM) &= ~(1<<AdcChannelGroup.Sequencer);
	if(AdcChannelGroup.AdcModule==ADC0) 
		NVIC_EN0_R &= ~(1<<(14+AdcChannelGroup.Sequencer));
	else if(AdcChannelGroup.AdcModule==ADC1)	
		NVIC_EN1_R &= ~(1<<(16+AdcChannelGroup.Sequencer));	

}
#endif

