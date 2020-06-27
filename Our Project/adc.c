/* ---------------------------------- INCLUDES ---------------------------------- */

#include "adc.h"

/* --------------------------- GLOBAL STATIC VARIABLES --------------------------- */

volatile Adc_ValueGroupType *resultBuffer[MAX_NB_GROUPS];
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


void Adc_Handler(Adc_GroupType Group)
{
	AdcChannelGroup AdcChannelGroup = ArrayOfAdcChannelGroups[Group];
	// taking the value of FIFOs in the result buffer
	for(int i=0 ; i<AdcChannelGroup.NbChannels ; i++)
		resultBuffer[Group][(i * AdcChannelGroup.nbSamples)+sampleNb[Group]] =
		Memory(AdcChannelGroup.AdcModule, ADC_SS_BASE + 
		ADC_SS_STEP *	AdcChannelGroup.Sequencer + ADC_SSFIFO);			
	
	if(AdcChannelGroup.Adc_GroupConvModeType==ADC_CONV_MODE_ONESHOT)
	{
		if(AdcChannelGroup.Adc_TriggerSourceType==ADC_TRIG_SRC_SW)
			Adc_StopGroupConversion(Group); 
		else if (AdcChannelGroup.Adc_TriggerSourceType==ADC_TRIG_SRC_HW)
		{
			Memory(AdcChannelGroup.AdcModule , ADC_EMUX) &= ~(1<<(AdcChannelGroup.Sequencer*4)) ; 
			Memory(AdcChannelGroup.AdcModule , ADC_PSSI) &= ~(1<<(AdcChannelGroup.Sequencer)) ; 
		}
	}
	if(AdcChannelGroup.Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[Group]=(sampleNb[Group]+1)%AdcChannelGroup.nbSamples;
	
	if(AdcChannelGroup.Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[Group]=sampleNb[Group]+1;
		if(sampleNb[Group]==AdcChannelGroup.nbSamples)
		{
			Memory(AdcChannelGroup.AdcModule,ADC_ACTSS) &= ~(1<<AdcChannelGroup.Sequencer);
			sampleNb[Group]=0;
		}
	}
	Memory(AdcChannelGroup.AdcModule,ADC_ISC) = (1<<AdcChannelGroup.Sequencer);

}

void ADC0SS0_Handler()
{
	Adc_Handler(getGroupId(ADC0,SS0));
}

void ADC0SS1_Handler()
{
	Adc_Handler(getGroupId(ADC0,SS1));
}
void ADC0SS2_Handler()
{
	Adc_Handler(getGroupId(ADC0,SS2));
}
void ADC0SS3_Handler()
{
	Adc_Handler(getGroupId(ADC0,SS3));
}
void ADC1SS0_Handler()
{
	Adc_Handler(getGroupId(ADC1,SS0));
}	
void ADC1SS1_Handler()
{
	Adc_Handler(getGroupId(ADC1,SS1));
}	
void ADC1SS2_Handler()
{
	Adc_Handler(getGroupId(ADC1,SS2));
}	
void ADC1SS3_Handler()
{
	Adc_Handler(getGroupId(ADC1,SS3));
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
	
	// Deactivate all seguencers 
	if(AdcChannelGroup.AdcModule==ADC0)
		ADC0_PSSI_R = 0x00 ; 
	else
		ADC1_PSSI_R = 0x00 ; 

}



#if (ADC_DEINIT_API==STD_ON)		
void Adc_DeInit (void)
{
	//Memory(AdcChannelGroup.AdcModule,ADC_SSPRI) |= (AdcChannelGroup.GroupPriority<<(AdcChannelGroup.Sequencer*4));	
	Memory(ADC0,ADC_EMUX) &= ~0xffffffff;
	Memory(ADC1,ADC_EMUX) &= ~0xffffffff;
	Memory(ADC0,ADC_SSCTL) &= ~0xffffffff;
	Memory(ADC1,ADC_SSCTL) &= ~0xffffffff;
	Memory(ADC0,ADC_IM) &= ~0xf;	
	Memory(ADC1,ADC_IM) &= ~0xf;	
	
	Memory(ADC0,ADC_ACTSS) &= ~0xF;
	Memory(ADC1,ADC_ACTSS) &= ~0xF;
	
	INT_DISABLE_ADC0;
	INT_DISABLE_ADC1;
	SYSCTL_RCGCADC_R&= ~0x3;
}


#endif


Std_ReturnType Adc_SetupResultBuffer( Adc_GroupType Group,volatile  Adc_ValueGroupType* DataBufferPtr )
{
/* taking the address of the result group buffer and save it in the DataBufferPtrAddr 
 DataBufferPtrAddr is array of pointers contain the adresses of the all result group buffer */
	resultBuffer[Group] = DataBufferPtr ;
						
	/* Result buffer has been setup successfully for the required group */
//	Adc_resultBufferInit[Group]=TRUE;
	
	return E_OK ; 
}

#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)
Std_ReturnType Adc_ReadGroup ( Adc_GroupType Group, volatile Adc_ValueGroupType* DataBufferPtr )
{
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Group].NbChannels ; i++)
		DataBufferPtr[i] = resultBuffer[Group][(i * ArrayOfAdcChannelGroups[Group].nbSamples)+sampleNb[Group]];
	return E_OK ; 
}
#endif

void Adc_StartGroupConversion ( Adc_GroupType Adc_GroupType )
{
	//to get the sequencer of the given group ID
	Sequencer Current_Sequencer;
	_Bool ValidGroup = 0;
	AdcChannelGroup* ChannelGroup;
	Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) |= (1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
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
			if(ChannelGroup->AdcModule == ADC0) ADC0_PSSI_R |= (1<< Current_Sequencer);
			else ADC1_PSSI_R |= (1<< Current_Sequencer);
			
	}
	else
	{
		/* handle invalid  error*/
	}
	ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StatusType = ADC_BUSY;
}

void Adc_StopGroupConversion( Adc_GroupType Group )
{
	uint8_t Current_Sequencer = ArrayOfAdcChannelGroups[Group].Sequencer ; 
	// 1. check if the trigger source is SW Trigger 
	if(ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType==ADC_TRIG_SRC_SW)
	{
		 //1.1. get which adc module is used for this group 
		if(ArrayOfAdcChannelGroups[Group].AdcModule==ADC0)
		{
			//1.1.1. Deactivate current sequncer 
			ADC0_PSSI_R &= ~ (1<< Current_Sequencer);
		}
		else
		{
				//1.1.2. Deactivate current sequncer 
			uint8_t Current_Sequencer = ArrayOfAdcChannelGroups[Group].Sequencer ; 
			ADC1_PSSI_R &= ~ (1<< Current_Sequencer);
		}
	}
	
	else if (ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType==ADC_TRIG_SRC_HW)
	{
		//error handling
	}
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


#if (ADC_HW_TRIGGER_API == STD_ON)
void Adc_EnableHardwareTrigger ( Adc_GroupType Group )
{
	Memory(ArrayOfAdcChannelGroups[Group].AdcModule,ADC_IM) |= (1<<ArrayOfAdcChannelGroups[Group].Sequencer);	
	
	if(ArrayOfAdcChannelGroups[Group].Adc_HwTriggerSourceType==ADC_TRIG_TIMER)
	{
		TIMER_initAdc(100);
	}
	else if (ArrayOfAdcChannelGroups[Group].Adc_HwTriggerSourceType==ADC_TRIG_EXTERNAL)
	{
		GPIO_initPin(PORTF,PIN4,DIGITAL,INPUT);	
		GPIO_initInterruptAdc(PORTF,PIN4,FALLING);
	}
}

void Adc_DisableHardwareTrigger ( Adc_GroupType Group )
{
	Memory(ArrayOfAdcChannelGroups[Group].AdcModule,ADC_IM) &= ~(1<<ArrayOfAdcChannelGroups[Group].Sequencer);	

	if(ArrayOfAdcChannelGroups[Group].Adc_HwTriggerSourceType==ADC_TRIG_TIMER)
	{
		SYSCTL_RCGCTIMER_R &= ~0x01;
	}
	else if (ArrayOfAdcChannelGroups[Group].Adc_HwTriggerSourceType==ADC_TRIG_EXTERNAL)
	{
		GPIO_initPin(PORTF,PIN4,DIGITAL,INPUT);	
		GPIO_initInterruptAdc(PORTF,PIN4,FALLING);
	}
}
#endif
#if (ADC_GET_STREAM_API==STD_ON)
Adc_StreamNumSampleType Adc_GetStreamLastPointer ( Adc_GroupType Group, volatile Adc_ValueGroupType** PtrToSamplePtr )
{
	volatile Adc_ValueGroupType *appBuffer; 
	if(Adc_ReadGroup(Group,appBuffer)!=E_OK){}
	  PtrToSamplePtr = &appBuffer;
	return sampleNb[Group];
}
#endif

