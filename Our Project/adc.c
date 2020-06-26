
/* ---------------------------------- INCLUDES ---------------------------------- */

#include "adc.h"

/* --------------------------- GLOBAL EXTERN VARIABLES --------------------------- */

extern AdcChannelGroup ArrayOfAdcChannelGroups[MAX_NB_GROUPS];

/*------------------------ LOCAL FUNCTIONS IMPLEMENTATION -------------------------*/

static uint8_t getGroupId(AdcModule AdcModule, Sequencer Sequencer)
{
	for(uint8_t i = 0 ; i<NB_OF_GROUPS ; i++)
	{
		if(ArrayOfAdcChannelGroups[i].AdcModule==AdcModule && ArrayOfAdcChannelGroups[i].Sequencer==Sequencer)
				return i;
	}
}
/* ----------------------------- APIs IMPLEMENTATION ----------------------------- */

void Adc_init(AdcChannelGroup AdcChannelGroup)
{
	if(AdcChannelGroup.AdcModule==ADC0)
	{	SYSCTL_RCGCADC_R |= 0x1;
		while(!(SYSCTL_PRADC_R |= 0x1));
	//	NVIC_EN0_R |= (1<<(14+AdcChannelGroup.Sequencer));
	INT_ENABLE_ADC0(AdcChannelGroup.Sequencer);
	}
	else if(AdcChannelGroup.AdcModule==ADC1)
	{	SYSCTL_RCGCADC_R |= 0x2;
		while(!(SYSCTL_PRADC_R |= 0x2));
	//	NVIC_EN1_R |= (1<<(16+AdcChannelGroup.Sequencer));
	INT_ENABLE_ADC1(AdcChannelGroup.Sequencer);
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
					|= (0x06<<(4*(AdcChannelGroup.NbChannels-1)));
		
		//assign priority for each group (sequencer)
	//Memory(AdcChannelGroup.AdcModule,ADC_SSPRI) |= (AdcChannelGroup.GroupPriority<<(AdcChannelGroup.Sequencer*4));
	
		//enable sequencer
	Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) |= (1<<AdcChannelGroup.Sequencer);

}


volatile uint32_t adcResult[8][8]; 

void Adc_SetupResultBuffer ( int groupId ,volatile uint32_t** buffer_ptr )
{
	if (ArrayOfAdcChannelGroups[groupId].AdcModule==ADC0) 
 {  
		switch ((int)ArrayOfAdcChannelGroups[groupId].Sequencer)
		{
			case SS0 : (*buffer_ptr) = &adcResult[0][0]; break;  
			case SS1 : (*buffer_ptr) = &adcResult[1][0]; break;
			case SS2 : (*buffer_ptr) = &adcResult[2][0]; break;
			case SS3 : (*buffer_ptr) = &adcResult[3][0]; break;
		}
	} 
 else if (ArrayOfAdcChannelGroups[groupId].AdcModule==ADC1)
 {
	switch ((int)ArrayOfAdcChannelGroups[groupId].Sequencer)
		{
			case SS0 : (*buffer_ptr) = &adcResult[4][0]; break;  
			case SS1 : (*buffer_ptr) = &adcResult[5][0]; break;
			case SS2 : (*buffer_ptr) = &adcResult[6][0]; break;
			case SS3 : (*buffer_ptr) = &adcResult[7][0]; break;
		}
 }
else;
}

void ADC0SS1_Handler()
{
	uint8_t groupId = getGroupId(ADC0,SS1);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[groupId].NbChannels; i++)
		adcResult[1][i] = ADC0_SSFIFO1_R;
	
	ADC0_ISC_R = (1<<1);
}

void ADC1SS0_Handler()
{
	uint8_t groupId = getGroupId(ADC1,SS0);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[groupId].NbChannels; i++)
		adcResult[4][i] = ADC1_SSFIFO0_R;
	
	ADC1_ISC_R = (1<<0);
}



void Adc_StartGroupConversion ( Adc_GroupType Group ){
	//to get the sequencer of the given group ID
	Sequencer Current_Sequencer;
	_Bool ValidGroup = 0;
	AdcChannelGroup* ChannelGroup;
	for( uint32_t i=0 ;i <= MAX_NB_GROUPS; i++){
		if ( ArrayOfAdcChannelGroups[i].GroupId == Group){
			ChannelGroup = &ArrayOfAdcChannelGroups[i];
			ValidGroup = 1;
			break;
		}
	}
	if (ValidGroup)
	{		if((ChannelGroup->Adc_TriggerSourceType)!= ADC_TRIG_SRC_SW)
			{
				/*handle trigger is not SW error*/
			}
			Current_Sequencer= ChannelGroup->Sequencer;
			if(ChannelGroup->AdcModule == ADC0) ADC0_PSSI_R = (1<< Current_Sequencer);
			else ADC1_PSSI_R = (1<< Current_Sequencer);
			
	}
	else
	{
		/* handle invalid  error*/
	}
}




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

