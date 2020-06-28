/* ---------------------------------- INCLUDES ---------------------------------- */

#include "adc.h"

/* --------------------------- GLOBAL STATIC VARIABLES --------------------------- */

volatile Adc_ValueGroupType *resultBuffer[MAX_NB_GROUPS];
static uint8_t sampleNb[MAX_NB_GROUPS]={0};

/* --------------------------- GLOBAL EXTERN VARIABLES --------------------------- */

extern Adc_ConfigType ArrayOfAdcChannels[MAX_NB_GROUPS];

/*------------------------ LOCAL FUNCTIONS IMPLEMENTATION -------------------------*/

static uint8_t getGroupId(AdcModule AdcModule, Sequencer Sequencer)
{
	for(uint8_t i = 0 ; i<NB_OF_GROUPS ; i++)
	{
		if(ArrayOfAdcChannels[i].AdcModule==AdcModule && ArrayOfAdcChannels[i].Sequencer==Sequencer)
				return i;
	}
	return 1;
}


void Adc_Handler(Adc_GroupType Group)
{
	Adc_ConfigType Adc_ConfigType = ArrayOfAdcChannels[Group];
	// taking the value of FIFOs in the result buffer
	for(int i=0 ; i<Adc_ConfigType.NbChannels ; i++)
		resultBuffer[Group][(i * Adc_ConfigType.nbSamples)+sampleNb[Group]] =
		Memory(Adc_ConfigType.AdcModule, ADC_SS_BASE + 
		ADC_SS_STEP *	Adc_ConfigType.Sequencer + ADC_SSFIFO);			
	
	if(Adc_ConfigType.Adc_GroupConvModeType==ADC_CONV_MODE_ONESHOT)
	{
		if(Adc_ConfigType.Adc_TriggerSourceType==ADC_TRIG_SRC_SW)
			Adc_StopGroupConversion(Group); 
		else if (Adc_ConfigType.Adc_TriggerSourceType==ADC_TRIG_SRC_HW)
		{
			Memory(Adc_ConfigType.AdcModule , ADC_EMUX) &= ~(1<<(Adc_ConfigType.Sequencer*4)) ; 
			Memory(Adc_ConfigType.AdcModule , ADC_PSSI) &= ~(1<<(Adc_ConfigType.Sequencer)) ; 
		}
	}
	if(Adc_ConfigType.Adc_StreamBufferModeType==ADC_STREAM_BUFFER_CIRCULAR)
		sampleNb[Group]=(sampleNb[Group]+1)%Adc_ConfigType.nbSamples;
	
	if(Adc_ConfigType.Adc_StreamBufferModeType==ADC_STREAM_BUFFER_LINEAR)
	{
		sampleNb[Group]=sampleNb[Group]+1;
		if(sampleNb[Group]==Adc_ConfigType.nbSamples)
		{
			Memory(Adc_ConfigType.AdcModule,ADC_ACTSS) &= ~(1<<Adc_ConfigType.Sequencer);
			sampleNb[Group]=0;
		}
	}
	Memory(Adc_ConfigType.AdcModule,ADC_ISC) = (1<<Adc_ConfigType.Sequencer);

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

void Adc_init(const Adc_ConfigType* ConfigPtr )
{
	if(ConfigPtr->AdcModule==ADC0)
	{	SYSCTL_RCGCADC_R |= 0x1;
		while(!(SYSCTL_PRADC_R |= 0x1));
		INT_ENABLE_ADC0(ConfigPtr->Sequencer);
	}
	else if(ConfigPtr->AdcModule==ADC1)
	{	SYSCTL_RCGCADC_R |= 0x2;
		while(!(SYSCTL_PRADC_R |= 0x2));
		INT_ENABLE_ADC1(ConfigPtr->Sequencer);
	}
		//disable sequencer
	Memory(ConfigPtr->AdcModule,ADC_ACTSS) &= ~(1<<ConfigPtr->Sequencer);
	
		//enable interrupts
	Memory(ConfigPtr->AdcModule,ADC_IM) |= (1<<ConfigPtr->Sequencer);	
	
		//select trigger source
	if(ConfigPtr->Adc_TriggerSourceType == ADC_TRIG_SRC_SW)
		Memory(ConfigPtr->AdcModule,ADC_EMUX) |= (0<<(ConfigPtr->Sequencer*4)); 
	else if(ConfigPtr->Adc_TriggerSourceType == ADC_TRIG_SRC_HW)
		Memory(ConfigPtr->AdcModule,ADC_EMUX) |= (ConfigPtr->Adc_HwTriggerSourceType<<(ConfigPtr->Sequencer*4)) ;
	
		//assign analog input pins
	for(int i=0;i<ConfigPtr->NbChannels;i++)
	{
		Memory(ConfigPtr->AdcModule, ADC_SS_BASE + ADC_SS_STEP * ConfigPtr->Sequencer + ADC_SSMUX)
					|= (ConfigPtr->ArrayOfAdcChannels[i]<<(4*i));
		Memory(ConfigPtr->AdcModule,ADC_SS_BASE + ADC_SS_STEP * ConfigPtr->Sequencer + ADC_SSCTL)
					|= (0x04<<(4*i));
	}	
		Memory(ConfigPtr->AdcModule,ADC_SS_BASE + ADC_SS_STEP * ConfigPtr->Sequencer + ADC_SSCTL)
					|= (0x06<<(4*(ConfigPtr->NbChannels-1)));
		
		//assign priority for each group (sequencer)
	//Memory(ConfigPtr->AdcModule,ADC_SSPRI) |= (ConfigPtr->GroupPriority<<(ConfigPtr->Sequencer*4));
	
		//enable sequencer
	Memory(ConfigPtr->AdcModule,ADC_ACTSS) |= (1<<ConfigPtr->Sequencer);
	
	// Deactivate all seguencers 
	if(ConfigPtr->AdcModule==ADC0)
		ADC0_PSSI_R = 0x00 ; 
	else
		ADC1_PSSI_R = 0x00 ; 
	
	//enable the analog input pins
	for(int i = 0 ; i< ConfigPtr->NbChannels; i++)
	{
		if(ConfigPtr->ArrayOfAdcChannels[i]==PE0) GPIO_initPin(PORTE,PIN0,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PE1) GPIO_initPin(PORTE,PIN1,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PE2) GPIO_initPin(PORTE,PIN2,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PE3) GPIO_initPin(PORTE,PIN3,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PE4) GPIO_initPin(PORTE,PIN4,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PE5) GPIO_initPin(PORTE,PIN5,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PD0) GPIO_initPin(PORTD,PIN0,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PD1) GPIO_initPin(PORTD,PIN1,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PD2) GPIO_initPin(PORTD,PIN2,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PD3) GPIO_initPin(PORTD,PIN3,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PB4) GPIO_initPin(PORTB,PIN4,ANALOG,PERIPHERAL);
		if(ConfigPtr->ArrayOfAdcChannels[i]==PB5) GPIO_initPin(PORTB,PIN5,ANALOG,PERIPHERAL);		
	}
}



#if (ADC_DEINIT_API==STD_ON)		
void Adc_DeInit (void)
{
	//Memory(ConfigPtr->AdcModule,ADC_SSPRI) |= (ConfigPtr->GroupPriority<<(ConfigPtr->Sequencer*4));	
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
			
	return E_OK ; 
}

#if (ADC_ENABLE_START_STOP_GROUP_API==STD_ON)		
void Adc_StartGroupConversion ( Adc_GroupType Group )
{
	//to get the sequencer of the given group ID
	Sequencer Current_Sequencer;
	_Bool ValidGroup = 0;
	Adc_ConfigType* ChannelGroup;
	Memory(ArrayOfAdcChannels[Group].AdcModule,ADC_ACTSS) |= (1<<ArrayOfAdcChannels[Group].Sequencer);
	for( uint32_t i=0 ;i <= MAX_NB_GROUPS; i++){
		if ( ArrayOfAdcChannels[i].Adc_GroupType == Group){
			ChannelGroup = &ArrayOfAdcChannels[i];
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
	ArrayOfAdcChannels[Group].Adc_StatusType = ADC_BUSY;
}

void Adc_StopGroupConversion( Adc_GroupType Group )
{
	uint8_t Current_Sequencer = ArrayOfAdcChannels[Group].Sequencer ; 
	// 1. check if the trigger source is SW Trigger 
	if(ArrayOfAdcChannels[Group].Adc_TriggerSourceType==ADC_TRIG_SRC_SW)
	{
		 //1.1. get which adc module is used for this group 
		if(ArrayOfAdcChannels[Group].AdcModule==ADC0)
		{
			//1.1.1. Deactivate current sequncer 
			ADC0_PSSI_R &= ~ (1<< Current_Sequencer);
		}
		else
		{
				//1.1.2. Deactivate current sequncer 
			uint8_t Current_Sequencer = ArrayOfAdcChannels[Group].Sequencer ; 
			ADC1_PSSI_R &= ~ (1<< Current_Sequencer);
		}
	}
	
	else if (ArrayOfAdcChannels[Group].Adc_TriggerSourceType==ADC_TRIG_SRC_HW)
	{
		//error handling
	}
}
#endif

#if (ADC_READ_GROUP_API==STD_ON)
Std_ReturnType Adc_ReadGroup ( Adc_GroupType Group, volatile Adc_ValueGroupType* DataBufferPtr )
{
	for(int i=0 ; i<ArrayOfAdcChannels[Group].NbChannels ; i++)
		DataBufferPtr[i] = resultBuffer[Group][(i * ArrayOfAdcChannels[Group].nbSamples)+sampleNb[Group]];
	return E_OK ; 
}
#endif

Adc_StatusType Adc_GetGroupStatus ( Adc_GroupType Group )
{
	return ArrayOfAdcChannels[Group].Adc_StatusType;	
}


#if (ADC_HW_TRIGGER_API == STD_ON)
void Adc_EnableHardwareTrigger ( Adc_GroupType Group )
{
	Memory(ArrayOfAdcChannels[Group].AdcModule,ADC_IM) |= (1<<ArrayOfAdcChannels[Group].Sequencer);	
	
	if(ArrayOfAdcChannels[Group].Adc_HwTriggerSourceType==ADC_TRIG_TIMER)
	{
		TIMER_initAdc(100);
	}
	else if (ArrayOfAdcChannels[Group].Adc_HwTriggerSourceType==ADC_TRIG_EXTERNAL)
	{
		GPIO_initPin(PORTF,PIN4,DIGITAL,INPUT);	
		GPIO_initInterruptAdc(PORTF,PIN4,FALLING);
	}
}

void Adc_DisableHardwareTrigger ( Adc_GroupType Group )
{
	Memory(ArrayOfAdcChannels[Group].AdcModule,ADC_IM) &= ~(1<<ArrayOfAdcChannels[Group].Sequencer);	

	if(ArrayOfAdcChannels[Group].Adc_HwTriggerSourceType==ADC_TRIG_TIMER)
	{
		SYSCTL_RCGCTIMER_R &= ~0x01;
	}
	else if (ArrayOfAdcChannels[Group].Adc_HwTriggerSourceType==ADC_TRIG_EXTERNAL)
	{
		/*disable gpio pin*/
	}
}
#endif

#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)		
//to enable interrupts for a specific group
void Adc_EnableGroupNotification(Adc_GroupType Group)
{
	Adc_ConfigType Adc_ConfigType = ArrayOfAdcChannels[Group];
	Memory(Adc_ConfigType.AdcModule,ADC_IM) |= (1<<Adc_ConfigType.Sequencer);
	if(Adc_ConfigType.AdcModule==ADC0) 
		NVIC_EN0_R |= (1<<(14+Adc_ConfigType.Sequencer));
	else if(Adc_ConfigType.AdcModule==ADC1)	
		NVIC_EN1_R |= (1<<(16+Adc_ConfigType.Sequencer));	
}

//to disable interrupts for a specific group
void Adc_DisableGroupNotification(Adc_GroupType Group)
{
	Adc_ConfigType Adc_ConfigType = ArrayOfAdcChannels[Group];
	Memory(Adc_ConfigType.AdcModule,ADC_IM) &= ~(1<<Adc_ConfigType.Sequencer);
	if(Adc_ConfigType.AdcModule==ADC0) 
		NVIC_EN0_R &= ~(1<<(14+Adc_ConfigType.Sequencer));
	else if(Adc_ConfigType.AdcModule==ADC1)	
		NVIC_EN1_R &= ~(1<<(16+Adc_ConfigType.Sequencer));	

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

