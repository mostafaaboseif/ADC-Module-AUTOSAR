/* ---------------------------------- INCLUDES ---------------------------------- */

#include "adc.h"

/* --------------------------- GLOBAL STATIC VARIABLES --------------------------- */

volatile Adc_ValueGroupType *resultBuffer[MAX_NB_GROUPS];
static uint8_t sampleNb[MAX_NB_GROUPS]={0};

/* --------------------------- GLOBAL EXTERN VARIABLES --------------------------- */

extern Adc_ConfigType ArrayOfAdcChannelGroups[MAX_NB_GROUPS];

/*------------------------ LOCAL FUNCTIONS IMPLEMENTATION -------------------------*/

static uint8_t getGroupId(AdcModule AdcModule, Sequencer Sequencer)
{
	for(uint8_t i = 0 ; i<NB_OF_GROUPS ; i++)
		if(ArrayOfAdcChannelGroups[i].AdcModule==AdcModule && ArrayOfAdcChannelGroups[i].Sequencer==Sequencer)
				return i;
	return 1;
}
void Adc_Handler(Adc_GroupType Group)
{
	Adc_ConfigType* Adc_ConfigType = &ArrayOfAdcChannelGroups[Group];
	// taking the value of FIFOs in the result buffer
	for(int i=0 ; i<Adc_ConfigType->NbChannels ; i++)
		resultBuffer[Group][(i * Adc_ConfigType->nbSamples)+sampleNb[Group]] =
		Memory(Adc_ConfigType->AdcModule, ADC_SS_BASE + ADC_SS_STEP *	Adc_ConfigType->Sequencer + ADC_SSFIFO);			
	
	if(Adc_ConfigType->Adc_GroupConvModeType==ADC_CONV_MODE_ONESHOT)
	{
		if(Adc_ConfigType->Adc_TriggerSourceType==ADC_TRIG_SRC_SW)
			Adc_StopGroupConversion(Group); 
		else if (Adc_ConfigType->Adc_TriggerSourceType==ADC_TRIG_SRC_HW)
		{
			Memory(Adc_ConfigType->AdcModule , ADC_EMUX) &= ~(1<<(Adc_ConfigType->Sequencer*4)) ; 
			Memory(Adc_ConfigType->AdcModule , ADC_PSSI) &= ~(1<<(Adc_ConfigType->Sequencer)) ; 
		}
	}
	if(Adc_ConfigType->Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[Group]=(sampleNb[Group]+1)%Adc_ConfigType->nbSamples;
	
	if(Adc_ConfigType->Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[Group]=sampleNb[Group]+1;
		if(sampleNb[Group]==Adc_ConfigType->nbSamples)
		{
			Memory(Adc_ConfigType->AdcModule,ADC_ACTSS) &= ~(1<<Adc_ConfigType->Sequencer);
			sampleNb[Group]=0;
		}
	}
	Memory(Adc_ConfigType->AdcModule,ADC_ISC) = (1<<Adc_ConfigType->Sequencer);
	
	//Setting The ADC Channel Group Status
	if( Adc_ConfigType->Adc_GroupConvModeType   == ADC_CONV_MODE_CONTINUOUS)
	{
	//[SWS_Adc_00325] ADC_STREAM_COMPLETED:
	//If it is called in single access mode after one conversion round is completed.
	//If it is called in streaming access mode after the number of conversion rounds of the requested group have been finished.
		if( ( Adc_ConfigType->Adc_GroupConvModeType   == ADC_ACCESS_MODE_STREAMING && sampleNb[Group]==Adc_ConfigType->nbSamples ) 
				 || Adc_ConfigType->Adc_GroupAccessModeType == ADC_ACCESS_MODE_SINGLE )
						Adc_ConfigType->Adc_StatusType = ADC_STREAM_COMPLETED;
	//[SWS_Adc_00224] ADC_COMPLETED: If it is called after a conversion round (not the final one)
		else
			Adc_ConfigType->Adc_StatusType = ADC_COMPLETED;
	}
}
void ADC0SS0_Handler()
{
	Adc_Handler(getGroupId(ADC0,SS0));
}
void ADC0SS1_Handler()
{
	Adc_Handler(getGroupId(ADC0,SS1));
}
void ADCG0_ResultBuffer0SS2_Handler()
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

void Adc_init(Adc_ConfigType *Adc_ConfigType)
{
	if(Adc_ConfigType->AdcModule==ADC0)
	{	
		SYSCTL_RCGCADC_R |= 0x1;
		while(!(SYSCTL_PRADC_R |= 0x1));
		INT_ENABLE_ADC0(Adc_ConfigType->Sequencer);
	}
	else if(Adc_ConfigType->AdcModule==ADC1)
	{	
		SYSCTL_RCGCADC_R |= 0x2;
		while(!(SYSCTL_PRADC_R |= 0x2));
		INT_ENABLE_ADC1(Adc_ConfigType->Sequencer);
	}
	//Disable sequencer
	Memory(Adc_ConfigType->AdcModule,ADC_ACTSS) &= ~(1<<Adc_ConfigType->Sequencer);
	//Enable interrupts
	Memory(Adc_ConfigType->AdcModule,ADC_IM) |= (1<<Adc_ConfigType->Sequencer);	
	//Select trigger source
	if(Adc_ConfigType->Adc_TriggerSourceType == ADC_TRIG_SRC_SW)
		Memory(Adc_ConfigType->AdcModule,ADC_EMUX) |= (0<<(Adc_ConfigType->Sequencer*4)); 
	else if(Adc_ConfigType->Adc_TriggerSourceType == ADC_TRIG_SRC_HW)
		Memory(Adc_ConfigType->AdcModule,ADC_EMUX) |= (Adc_ConfigType->Adc_HwTriggerSourceType<<(Adc_ConfigType->Sequencer*4)) ;
	//Assign analog input pins
	for(int i=0;i<Adc_ConfigType->NbChannels;i++)
	{
		Memory(Adc_ConfigType->AdcModule, ADC_SS_BASE + ADC_SS_STEP * Adc_ConfigType->Sequencer + ADC_SSMUX)
					|= (Adc_ConfigType->ArrayOfAdcChannels[i]<<(4*i));
		Memory(Adc_ConfigType->AdcModule,ADC_SS_BASE + ADC_SS_STEP * Adc_ConfigType->Sequencer + ADC_SSCTL)
					|= (0x04<<(4*i));
	}	
		Memory(Adc_ConfigType->AdcModule,ADC_SS_BASE + ADC_SS_STEP * Adc_ConfigType->Sequencer + ADC_SSCTL)
					|= (0x06<<(4*(Adc_ConfigType->NbChannels-1)));
		
	//Assign priority for each group (sequencer)
	//Memory(Adc_ConfigType->AdcModule,ADC_SSPRI) |= (Adc_ConfigType->GroupPriority<<(Adc_ConfigType->Sequencer*4));
	
	//Enable sequencer
	Memory(Adc_ConfigType->AdcModule,ADC_ACTSS) |= (1<<Adc_ConfigType->Sequencer);
	
	//Deactivate all seguencers 
	if(Adc_ConfigType->AdcModule==ADC0)
		ADC0_PSSI_R = 0x00 ; 
	else
		ADC1_PSSI_R = 0x00 ;
	
	//[SWS_Adc_00221]
	//Set the status of Adc Group to IDLE before start conversion
	Adc_ConfigType->Adc_StatusType = ADC_IDLE;
}
#if (ADC_DEINIT_API==STD_ON)		
void Adc_DeInit (void)
{
	//Memory(Adc_ConfigType.AdcModule,ADC_SSPRI) |= (Adc_ConfigType.GroupPriority<<(Adc_ConfigType.Sequencer*4));	
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
	//[SWS_Adc_00423]?If development error detection for the ADC module is enabled: 
	//if the channel group ID is non-existing, the function Adc_SetupResultBuffer shall raise development error ADC_E_PARAM_GROUP and return without any action.
		for(int i=0;i<MAX_NB_GROUPS;i++)
		{
			if(ArrayOfAdcChannelGroups[i].Adc_GroupType== Group)
				break;
			if(i==MAX_NB_GROUPS-1)
				return E_NOT_OK ;
		}
	//[SWS_Adc_00433] ?If called while group is not in state ADC_IDLE, function Adc_SetupResultBuffer shall report a runtime error ADC_E_BUSY.
	if(!(ArrayOfAdcChannelGroups[Group].Adc_StatusType==ADC_IDLE))resultBuffer[Group] = DataBufferPtr ;			
	/* Result buffer has been setup successfully for the required group */
//Adc_resultBufferInit[Group]=TRUE;
	
	return E_OK ; 
}
#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)
Std_ReturnType Adc_ReadGroup ( Adc_GroupType Group, volatile Adc_ValueGroupType* DataBufferPtr )
	{ 
		//[SWS_Adc_00152]?If development error detection for the ADC module is enabled: 
		//if the group ID is non-existing, the function Adc_ReadGroup shall raise development
		for(int i=0;i<MAX_NB_GROUPS;i++)
		{
			if(ArrayOfAdcChannelGroups[i].Adc_GroupType== Group)
				break;
			if(i==MAX_NB_GROUPS-1)
				return E_NOT_OK ;
					
		
		}
	
	
	for(int i=0 ; i<ArrayOfAdcChannelGroups[Group].NbChannels ; i++)
		DataBufferPtr[i] = resultBuffer[Group][(i * ArrayOfAdcChannelGroups[Group].nbSamples)+sampleNb[Group]];
	
	//Setting The ADC Channel Group Status
	//[SWS_Adc_00221]
		//Set the status of Adc Group to IDLE if :
		// In continuous group conversion mode with linear streaming access mode and the group was in state ADC_STREAM_COMPLETED.
		// In one-shot SW conversion mode.
		// In groups with trigger source HW with linear streaming access mode and the group was in state ADC_STREAM_COMPLETED.
	  
	if((ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType    == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_STREAM_COMPLETED )
	 ||(ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType    == ADC_CONV_MODE_ONESHOT    && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType    == ADC_TRIG_SRC_SW )
	 ||(ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType    == ADC_TRIG_SRC_HW 				 && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_STREAM_COMPLETED ))
			ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_IDLE;
		
	//[SWS_Adc_00222]
		//Set the status of Adc Group to BUSY if :
			//In continuous group conversion mode with single access mode.
			//In continuous group conversion mode with circular streaming access mode. 
			//In continuous group conversion mode with linear streaming access mode and the group was in state ADC_COMPLETED.
			//In one-shot HW conversion mode and single access mode.
			//In one-shot HW conversion mode and circular streaming access mode.
			//In one-shot HW conversion mode and linear streaming access mode and the group was in state ADC_COMPLETED

	if( ( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_GroupAccessModeType  == ADC_ACCESS_MODE_SINGLE )   
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_CIRCULAR)
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_COMPLETED )
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_ONESHOT    && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType == ADC_TRIG_SRC_HW && ArrayOfAdcChannelGroups[Group].Adc_GroupAccessModeType  == ADC_ACCESS_MODE_SINGLE ) 
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_ONESHOT    && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType == ADC_TRIG_SRC_HW && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_CIRCULAR )
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_ONESHOT	   && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType == ADC_TRIG_SRC_HW && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_COMPLETED))
					ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_BUSY;

	return E_OK ; 
}
#endif
void Adc_StartGroupConversion ( Adc_GroupType Adc_GroupType )
{
	//To get the sequencer of the given group ID
	Sequencer Current_Sequencer;
	_Bool ValidGroup = 0;
	Adc_ConfigType* ChannelGroup;
	Memory(ArrayOfAdcChannelGroups[Adc_GroupType].AdcModule,ADC_ACTSS) |= (1<<ArrayOfAdcChannelGroups[Adc_GroupType].Sequencer);
	for( uint32_t i=0 ;i <= MAX_NB_GROUPS; i++)
	{
		if ( ArrayOfAdcChannelGroups[i].Adc_GroupType == Adc_GroupType)
		{
			ChannelGroup = &ArrayOfAdcChannelGroups[i];
			ValidGroup = 1;
			break;
		}
	}
	if (ValidGroup)
	{		
			if((ChannelGroup->Adc_TriggerSourceType)!= ADC_TRIG_SRC_SW)
			{
				/*handle trigger is not SW error*/
			}
			Current_Sequencer= ChannelGroup->Sequencer;
			if(ChannelGroup->AdcModule == ADC0) 
				ADC0_PSSI_R |= (1<< Current_Sequencer);
			else 
				ADC1_PSSI_R |= (1<< Current_Sequencer);
			
			//[SWS_Adc_00222]
			// Set the status of Adc Group to BUSY if a group start conversion with SW trigger source  
			ArrayOfAdcChannelGroups[Adc_GroupType].Adc_StatusType = ADC_BUSY;
	}
	else
	{
		/* handle invalid  error*/
	}
}

/*This function only called for SW Trigger*/
void Adc_StopGroupConversion( Adc_GroupType Group )
{
	uint8_t Current_Sequencer = ArrayOfAdcChannelGroups[Group].Sequencer ; 
	// 1. check if the trigger source is SW Trigger 
	if(ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType==ADC_TRIG_SRC_SW)
	{
		 //1.1. get which adc module is used for this group 
		if(ArrayOfAdcChannelGroups[Group].AdcModule==ADC0)
		{
			//1.1.1. Deactivate current sequncer of ADC0
			ADC0_PSSI_R &= ~ (1<< Current_Sequencer);
		}
		else
		{
			//1.1.2. Deactivate current sequncer of ADC1
			uint8_t Current_Sequencer = ArrayOfAdcChannelGroups[Group].Sequencer ; 
			ADC1_PSSI_R &= ~ (1<< Current_Sequencer);
		}
		//[SWS_Adc_00221]
		// Set the status of Adc group to IDLE after SW stop conversion
		ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_IDLE;
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
//To enable interrupts for a specific group
void Adc_EnableGroupNotification(Adc_GroupType Adc_GroupType)
{
	Adc_ConfigType Adc_ConfigType = ArrayOfAdcChannelGroups[Adc_GroupType];
	Memory(Adc_ConfigType.AdcModule,ADC_IM) |= (1<<Adc_ConfigType.Sequencer);
	if(Adc_ConfigType.AdcModule==ADC0) 
		NVIC_EN0_R |= (1<<(14+Adc_ConfigType.Sequencer));
	else if(Adc_ConfigType.AdcModule==ADC1)	
		NVIC_EN1_R |= (1<<(16+Adc_ConfigType.Sequencer));	
}
//To disable interrupts for a specific group
void Adc_DisableGroupNotification(Adc_GroupType Adc_GroupType)
{
	Adc_ConfigType Adc_ConfigType = ArrayOfAdcChannelGroups[Adc_GroupType];
	Memory(Adc_ConfigType.AdcModule,ADC_IM) &= ~(1<<Adc_ConfigType.Sequencer);
	if(Adc_ConfigType.AdcModule==ADC0) 
		NVIC_EN0_R &= ~(1<<(14+Adc_ConfigType.Sequencer));
	else if(Adc_ConfigType.AdcModule==ADC1)	
		NVIC_EN1_R &= ~(1<<(16+Adc_ConfigType.Sequencer));	
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
	//Set the status of Adc Group to BUSY once trigger is enabled for group with HW trigger source.
	ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_BUSY;
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
	//[SWS_Adc_00221]
	// Set the status of Adc Group to IDLE once trigger is disabled for group with HW trigger source 
	ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_IDLE;
}
#endif
#if (ADC_GET_STREAM_API==STD_ON)
Adc_StreamNumSampleType Adc_GetStreamLastPointer ( Adc_GroupType Group, volatile Adc_ValueGroupType** PtrToSamplePtr )
{
	volatile Adc_ValueGroupType *appBuffer; 
	if(Adc_ReadGroup(Group,appBuffer)!=E_OK){}
	  PtrToSamplePtr = &appBuffer;
	//Setting The ADC Channel Group Status
	//[SWS_Adc_00221]
		//Set the status of Adc Group to IDLE if :
		// In continuous group conversion mode with linear streaming access mode and the group was in state ADC_STREAM_COMPLETED.
		// In one-shot SW conversion mode.
		// In groups with trigger source HW with linear streaming access mode and the group was in state ADC_STREAM_COMPLETED.
	  
	if((ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType    == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_STREAM_COMPLETED )
	 ||(ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType    == ADC_CONV_MODE_ONESHOT    && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType    == ADC_TRIG_SRC_SW )
	 ||(ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType    == ADC_TRIG_SRC_HW 				 && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_STREAM_COMPLETED ))
			ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_IDLE;
		
	//[SWS_Adc_00222]
		//Set the status of Adc Group to BUSY if :
			//In continuous group conversion mode with single access mode.
			//In continuous group conversion mode with circular streaming access mode. 
			//In continuous group conversion mode with linear streaming access mode and the group was in state ADC_COMPLETED.
			//In one-shot HW conversion mode and single access mode.
			//In one-shot HW conversion mode and circular streaming access mode.
			//In one-shot HW conversion mode and linear streaming access mode and the group was in state ADC_COMPLETED

	if( ( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_GroupAccessModeType  == ADC_ACCESS_MODE_SINGLE )   
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_CIRCULAR)
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_CONTINUOUS && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_COMPLETED )
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_ONESHOT    && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType == ADC_TRIG_SRC_HW && ArrayOfAdcChannelGroups[Group].Adc_GroupAccessModeType  == ADC_ACCESS_MODE_SINGLE ) 
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_ONESHOT    && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType == ADC_TRIG_SRC_HW && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_CIRCULAR )
		||( ArrayOfAdcChannelGroups[Group].Adc_GroupConvModeType == ADC_CONV_MODE_ONESHOT	   && ArrayOfAdcChannelGroups[Group].Adc_TriggerSourceType == ADC_TRIG_SRC_HW && ArrayOfAdcChannelGroups[Group].Adc_StreamBufferModeType == ADC_STREAM_BUFFER_LINEAR && ArrayOfAdcChannelGroups[Group].Adc_StatusType == ADC_COMPLETED))
					ArrayOfAdcChannelGroups[Group].Adc_StatusType = ADC_BUSY;

	return sampleNb[Group];
}
#endif

