
/* ---------------------------------- INCLUDES ---------------------------------- */

#include "adc.h"

/* --------------------------- GLOBAL STATIC VARIABLES --------------------------- */

volatile Adc_ValueGroupType adcResult[MAX_NB_GROUPS][MAX_NB_CHANNELS][MAX_NB_OF_SAMPLES_PER_CHANNEL];
static uint8_t sampleNb[MAX_NB_GROUPS]={0};

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
	return 1;
}

void ADC0SS0_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC0,SS0);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels ; i++)
			adcResult[0][i][sampleNb[0]] = ADC0_SSFIFO0_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[0]=(sampleNb[0]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[0]=sampleNb[0]+1;
		if(sampleNb[0]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}
	ADC0_ISC_R = (1<<0);
}

void ADC0SS1_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC0,SS1);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[1][i][sampleNb[1]] = ADC0_SSFIFO1_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[1]=(sampleNb[1]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[1]=sampleNb[1]+1;
		if(sampleNb[1]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}
		
	ADC0_ISC_R = (1<<1);
}

void ADC0SS2_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC0,SS2);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[2][i][sampleNb[2]] = ADC0_SSFIFO2_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[2]=(sampleNb[2]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[2]=sampleNb[2]+1;
		if(sampleNb[2]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}

	ADC0_ISC_R = (1<<2);
}

void ADC0SS3_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC0,SS3);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[3][i][sampleNb[3]] = ADC0_SSFIFO3_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[3]=(sampleNb[3]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[3]=sampleNb[3]+1;
		if(sampleNb[3]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}

	ADC0_ISC_R = (1<<3);
}

void ADC1SS0_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC1,SS0);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[4][i][sampleNb[4]] = ADC1_SSFIFO0_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[4]=(sampleNb[4]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[4]=sampleNb[4]+1;
		if(sampleNb[4]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}

	ADC1_ISC_R = (1<<0);
}

void ADC1SS1_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC1,SS1);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[5][i][sampleNb[5]] = ADC1_SSFIFO1_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[5]=(sampleNb[5]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[5]=sampleNb[5]+1;
		if(sampleNb[5]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}
	ADC1_ISC_R = (1<<1);
}

void ADC1SS2_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC1,SS2);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[6][i][sampleNb[6]] = ADC1_SSFIFO2_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[6]=(sampleNb[6]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[6]=sampleNb[6]+1;
		if(sampleNb[6]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}
	ADC1_ISC_R = (1<<2);
}

void ADC1SS3_Handler()
{
	uint8_t Adc_GroupType = getGroupId(ADC1,SS3);
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Adc_GroupType].NbChannels; i++)
			adcResult[7][i][sampleNb[7]] = ADC1_SSFIFO3_R;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[7]=(sampleNb[7]+1)%ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples;
	if(ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[7]=sampleNb[7]+1;
		if(sampleNb[7]==ArrayOfAdcChannelGroups[Adc_GroupType].nbSamples)
			Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) &= ~(1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	}
	ADC1_ISC_R = (1<<3);
}

/* ----------------------------- APIs IMPLEMENTATION ----------------------------- */

void Adc_init(AdcChannelGroup AdcChannelGroup)
{
	if(AdcChannelGroup.AdcModule==ADC0)
	{	SYSCTL_RCGCADC_R |= 0x1;
		while(!(SYSCTL_PRADC_R |= 0x1));
		INT_ENABLE_ADC0(AdcChannelGroup.Sequencer);
	}
	else if(AdcChannelGroup.AdcModule==ADC1)
	{	SYSCTL_RCGCADC_R |= 0x2;
		while(!(SYSCTL_PRADC_R |= 0x2));
		INT_ENABLE_ADC1(AdcChannelGroup.Sequencer);
	}
		
		//disable sequencer
	Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) &= ~(1<<AdcChannelGroup.Sequencer);
	
		//enable interrupts
	Memory(AdcChannelGroup.AdcModule,ADC_IM) |= (1<<AdcChannelGroup.Sequencer);	
	
		//select trigger source
	if(AdcChannelGroup.Adc_TriggerSourceType == ADC_TRIG_SRC_SW)
		Memory(AdcChannelGroup.AdcModule,ADC_EMUX) |= (0<<(AdcChannelGroup.Sequencer*4)); 
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

void Adc_SetupResultBuffer ( Adc_GroupType Adc_GroupType ,volatile Adc_ValueGroupType** buffer_ptr )
{
	if (ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule==ADC0) 
 {  
		switch ((int)ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer)
		{
			case SS0 : (*buffer_ptr) = &adcResult[0][0][0]; break;  
			case SS1 : (*buffer_ptr) = &adcResult[1][0][0]; break;
			case SS2 : (*buffer_ptr) = &adcResult[2][0][0]; break;
			case SS3 : (*buffer_ptr) = &adcResult[3][0][0]; break;
		}
	} 
 else if (ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule==ADC1)
 {
	switch ((int)ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer)
		{
			case SS0 : (*buffer_ptr) = &adcResult[4][0][0]; break;  
			case SS1 : (*buffer_ptr) = &adcResult[5][0][0]; break;
			case SS2 : (*buffer_ptr) = &adcResult[6][0][0]; break;
			case SS3 : (*buffer_ptr) = &adcResult[7][0][0]; break;
		}
 }
else;
}


void Adc_StartGroupConversion ( Adc_GroupType Adc_GroupType )
{
	//to get the sequencer of the given group ID
	Sequencer Current_Sequencer;
	_Bool ValidGroup = 0;
	AdcChannelGroup* ChannelGroup;
	for( uint32_t i=0 ;i <= MAX_NB_GROUPS; i++){
		if ( ArrayOfAdcChannelGroups[i].Adc_GroupType == Adc_GroupType){
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
	ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StatusType = ADC_BUSY;
}

Adc_StatusType Adc_GetGroupStatus ( Adc_GroupType Group )
{
	return ArrayOfAdcChannelGroups[Group].Adc_StatusType;	
}

#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)		
//to enable interrupts for a specific group
void Adc_EnableGroupNotification(Adc_GroupType Adc_GroupType)
{
	AdcChannelGroup AdcChannelGroup = ArrayOfAdcChannelGroups[Adc_GroupType];
	Memory(AdcChannelGroup.AdcModule,ADC_IM) |= (1<<AdcChannelGroup.Sequencer);
	if(AdcChannelGroup.AdcModule==ADC0) 
		NVIC_EN0_R |= (1<<(14+AdcChannelGroup.Sequencer));
	else if(AdcChannelGroup.AdcModule==ADC1)	
		NVIC_EN1_R |= (1<<(16+AdcChannelGroup.Sequencer));	
}

//to disable interrupts for a specific group
void Adc_DisableGroupNotification(Adc_GroupType Adc_GroupType)
{
	AdcChannelGroup AdcChannelGroup = ArrayOfAdcChannelGroups[Adc_GroupType];
	Memory(AdcChannelGroup.AdcModule,ADC_IM) &= ~(1<<AdcChannelGroup.Sequencer);
	if(AdcChannelGroup.AdcModule==ADC0) 
		NVIC_EN0_R &= ~(1<<(14+AdcChannelGroup.Sequencer));
	else if(AdcChannelGroup.AdcModule==ADC1)	
		NVIC_EN1_R &= ~(1<<(16+AdcChannelGroup.Sequencer));	

}
#endif

Adc_StreamNumSampleType Adc_GetStreamLastPointer ( Adc_GroupType Adc_GroupType, volatile Adc_ValueGroupType** PtrToSamplePtr )
{
		if(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule==ADC0) 
			PtrToSamplePtr= (volatile Adc_ValueGroupType**)adcResult[ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer];
		else if(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule==ADC1) 
			PtrToSamplePtr= (volatile Adc_ValueGroupType**)adcResult[4+ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer];

	return **PtrToSamplePtr;//getGroupId(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
}
