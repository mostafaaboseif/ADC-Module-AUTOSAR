
/* ---------------------------------- INCLUDES ---------------------------------- */

#include "gpio.h"
#include "Adc_PCCfg.h"
#include "Std_Types.h"
#include "uart.h"
#include "timer.h"

/* ---------------------------------- CONSTANTS ---------------------------------- */

#define MAX_NB_CHANNELS 8
#define MAX_NB_GROUPS 8

/* ---------------------------------- PRIMITIVE TYPES ---------------------------------- */

typedef uint8_t Adc_ChannelType;
typedef uint8_t Adc_GroupType; //group index in array (0 - 7)
typedef int16_t Adc_ValueGroupType;
typedef uint8_t Adc_PrescaleType;
typedef uint8_t Adc_ConversionTimeType; 
typedef uint8_t Adc_SamplingTimeType;
typedef uint8_t Adc_ResolutionType;
typedef uint8_t Adc_GroupPriorityType; 
typedef uint8_t Adc_StreamNumSampleType;

/* ---------------------------------- ENUMS ---------------------------------- */
typedef enum 
{
FALSE,TRUE
}boolean; 
//In Progress
typedef enum Adc_StatusType{
ADC_IDLE,
ADC_BUSY,
ADC_COMPLETED,
ADC_STREAM_COMPLETED
}Adc_StatusType;

//Implementation Specific
typedef enum ADCRegOffset{
ADC_ACTSS       =   0x000,  
ADC_RIS         =   0x004, 
ADC_IM          =   0x008, 
ADC_ISC         =   0x00C, 
ADC_OSTAT       =   0x010, 
ADC_EMUX        =   0x014, 
ADC_USTAT       =   0x018, 
ADC_TSSEL       =   0x01C, 
ADC_SSPRI       =   0x020,
ADC_SPC         =   0x024,
ADC_PSSI        =   0x028,
ADC_SAC         =   0x030,
ADC_CTL         =   0x038,
ADC_SSMUX0      =   0x040,//base
ADC_SSCTL0      =   0x044,
ADC_SSFIFO0     =   0x048,//offset
ADC_SSFSTAT0    =   0x04C, 
ADC_SSOP0       =   0x050, 
ADC_SSDC0       =   0x054, 
ADC_SSEMUX0     =   0x058, 
ADC_SSTSH0      =   0x05C, 
ADC_SSMUX1      =   0x060,//step
ADC_SSCTL1      =   0x064,
ADC_SSFIFO1     =   0x068,
ADC_SSFSTAT1    =   0x06C, 
ADC_SSOP1       =   0x070, 
ADC_SSDC1       =   0x074, 
ADC_SSEMUX1     =   0x078,
ADC_SSTSH1      =   0x07C,
ADC_SSMUX2      =   0x080,
ADC_SSCTL2      =   0x084,
ADC_SSFIFO2     =   0x088,
ADC_SSFSTAT2    =   0x08C,
ADC_SSOP2       =   0x090,
ADC_SSDC2       =   0x094,
ADC_SSEMUX2     =   0x098, 
ADC_SSTSH2      =   0x09C, 
ADC_SSMUX3      =   0x0A0,
ADC_SSCTL3      =   0x0A4, 
ADC_SSFIFO3     =   0x0A8, 
ADC_SSFSTAT3    =   0x0AC, 
ADC_SSOP3       =   0x0B0, 
ADC_SSDC3       =   0x0B4, 
ADC_SSEMUX3     =   0x0B8, 
ADC_SSTSH3      =   0x0BC, 
ADC_PP          =   0xFC0, 
ADC_PC          =   0xFC4,
ADC_CC          =   0xFC8 
} ADCRegOffset;




typedef enum SeqOffset{
ADC_SS_BASE   = ADC_SSMUX0,
ADC_SS_STEP   = ADC_SSMUX1 - ADC_SSMUX0,
ADC_SSMUX     = ADC_SSMUX0 - ADC_SSMUX0,
ADC_SSCTL     = ADC_SSCTL0 - ADC_SSMUX0,
ADC_SSFIFO    = ADC_SSFIFO0 - ADC_SSMUX0,
ADC_SSFSTAT  	= ADC_SSFSTAT0 - ADC_SSMUX0
}SeqOffset;


typedef enum AdcChannel{
PE3,PE2,PE1,PE0,PD3,PD2,PD1,PD0,PE5 
}AdcChannel;
typedef enum Sequencer{
	SS0,SS1,SS2,SS3
}Sequencer;


typedef enum AdcModule{
	ADC0			= 0x40038000,
	ADC1	 		= 0x40039000
} AdcModule;



//AUTOSAR Requirements
typedef enum Adc_GroupAccessModeType{  
ADC_ACCESS_MODE_SINGLE,
ADC_ACCESS_MODE_STREAMING
}Adc_GroupAccessModeType;


typedef enum Adc_StreamBufferModeType{ 
ADC_STREAM_BUFFER_LINEAR,
ADC_STREAM_BUFFER_CIRCULAR
}Adc_StreamBufferModeType;

typedef enum Adc_GroupConvModeType{  
ADC_CONV_MODE_ONESHOT=0,
ADC_CONV_MODE_CONTINUOUS=0xF
}Adc_GroupConvModeType;

typedef enum Adc_TriggerSourceType{
ADC_TRIG_SRC_SW, 
ADC_TRIG_SRC_HW
}Adc_TriggerSourceType;

typedef enum Adc_HwTriggerSourceType{
ADC_TRIG_COMP0=1,
ADC_TRIG_COMP1,
ADC_TRIG_COMP2,
ADC_TRIG_EXTERNAL,
ADC_TRIG_TIMER,
ADC_TRIG_PWM0,
ADC_TRIG_PWM1,
ADC_TRIG_PWM2,
ADC_TRIG_PWM3,
ADC_TRIG_ALWAYS =0xF,
NO_HW_TRIG
}Adc_HwTriggerSourceType;

typedef enum Adc_HwTriggerSignalType{
ADC_HW_TRIG_RISING_EDGE, 
ADC_HW_TRIG_FALLING_EDGE, 
ADC_HW_TRIG_BOTH_EDGES
}Adc_HwTriggerSignalType;




typedef enum Adc_ResultAlignmentType{
ADC_ALIGN_LEFT,
ADC_ALIGN_RIGHT
}Adc_ResultAlignmentType;

typedef enum Adc_PriorityImplementationType{
ADC_PRIORITY_NONE,
ADC_PRIORITY_HW,
ADC_PRIORITY_HW_SW
}Adc_PriorityImplementationType;






/* ---------------------------------- STRUCTS ---------------------------------- */

typedef struct AdcChannelGroup{
Adc_GroupType Adc_GroupType;
AdcModule AdcModule;
Sequencer Sequencer;
Adc_TriggerSourceType Adc_TriggerSourceType;
Adc_HwTriggerSourceType Adc_HwTriggerSourceType;
Adc_HwTriggerSignalType Adc_HwTriggerSignalType;
Adc_GroupConvModeType Adc_GroupConvModeType;
Adc_GroupAccessModeType Adc_GroupAccessModeType;
Adc_StreamNumSampleType nbSamples;
Adc_StreamBufferModeType Adc_StreamBufferModeType;
Adc_StatusType Adc_StatusType;
Adc_GroupPriorityType GroupPriority;
Adc_ChannelType NbChannels;
AdcChannel ArrayOfAdcChannels[MAX_NB_CHANNELS];
}AdcChannelGroup;

/*----------------------------------- FUNCTION-LIKE MACROS -----------------------------------*/

#define INT_ENABLE_ADC0(X)    *NVIC_EN0 |= (1<<(14+X));
#define INT_ENABLE_ADC1(X)    *NVIC_EN1 |= (1<<(16+X));
#define INT_DISABLE_ADC0 			*NVIC_EN0 &= ~(0xF<<14);
#define INT_DISABLE_ADC1	    *NVIC_EN1 &= ~(0xF<<16);


/* ---------------------------------- FUNCTION DECLARATIONS ---------------------------------- */
// [SWS_Adc_00054] Adc_Init shall initialize the ADC hardware units and driver according to the configuration set referenced by ConfigPtr.
// [SWS_Adc_00056] The function Adc_Init shall only initialize the configured resources.
// [SWS_Adc_00246] If the hardware allows for only one usage of the register, the driver module implementing that functionality is responsible for initializing the register.⌋ (SRS_SPAL_12461)
// [SWS_Adc_00249] One-time writable registers that require initialization directly after reset shall be initialized by the startup code.⌋ (SRS_SPAL_12461)
// [SWS_Adc_00250] All other registers shall be initialized by the startup code.⌋ (SRS_SPAL_12461)
// [SWS_Adc_00077] The function Adc_Init shall disable the notifications and hardware trigger capability (if statically configured as active).⌋ (SRS_Adc_12318)
// [SWS_Adc_00307] The function Adc_Init shall set all groups to ADC_IDLE state.
void Adc_init(AdcChannelGroup);


//[SWS_Adc_00110] ⌈The function Adc_DeInit shall return all ADC HW Units to a state comparable to their power on reset state. Values of registers which are not writeable are excluded. It’s the responsibility of the hardware design that this state does not lead to undefined activities in the μC.⌋ (SRS_SPAL_12163)
//[SWS_Adc_00111] ⌈The function Adc_DeInit shall disable all used interrupts and notifications.⌋ (SRS_BSW_00336, SRS_SPAL_12163)
//[SWS_Adc_00358] ⌈The ADC module’s environment shall not call the function Adc_DeInit while any group is not in state ADC_IDLE. ()
//[SWS_Adc_00228] ⌈The function Adc_DeInit shall be pre compile time configurable On/Off by the configuration parameter: AdcDeInitApi.(SRS_BSW_00171)
#if (ADC_DEINIT_API==STD_ON)		
void Adc_DeInit (void);
#endif

//[SWS_Adc_00420] The function Adc_SetupResultBuffer shall initialize the result buffer pointer of the selected group with the address value passed as parameter.⌋ 
Std_ReturnType Adc_SetupResultBuffer( Adc_GroupType Group, volatile Adc_ValueGroupType* DataBufferPtr ); 

#if (ADC_ENABLE_START_STOP_GROUP_API==STD_ON)
//[SWS_Adc_00061] ⌈The function Adc_StartGroupConversion shall start the conversion of all channels of the requested ADC Channel group. Depending on the group configuration, one-shot or continuous conversion is started.⌋ (SRS_Adc_12364)
//[SWS_Adc_00431]⌈The function Adc_StartGroupConversion shall reset the internal result buffer pointer, that conversion result storage always starts, after calling Adc_StartGroupConversion, at the result buffer base address which was configured with Adc_SetupResultBuffer.⌋ ()
//[SWS_Adc_00156] ⌈The function Adc_StartGroupConversion shall NOT automatically enable the notification mechanism for that group (this has to be done by a separate API call).⌋ (SRS_Adc_12317, SRS_Adc_12318)
//[SWS_Adc_00146] ⌈The ADC module’s environment shall only call Adc_StartGroupConversion for groups configured with software trigger source.⌋ (SRS_Adc_12817, SRS_Adc_12364)
//[SWS_Adc_00259]⌈The function Adc_StartGroupConversion shall be pre-compile time configurable On/Off by the configuration parameter AdcEnableStartStopGroupApi.⌋ (SRS_BSW_00171)	
void Adc_StartGroupConversion ( Adc_GroupType groupId );



//[SWS_Adc_00385] ⌈When the ADC Channel Group is in one-shot and software-trigger mode, the function Adc_StopGroupConversion shall stop an ongoing conversion of the group.⌋ (SRS_Adc_12364)
//[SWS_Adc_00437] ⌈When the ADC Channel Group is in one-shot and software-trigger mode, the function Adc_StopGroupConversion shall remove a start/restart request of the group from the queue, if queuing is enabled and a start/restart request is stored in the queue.⌋ ()
//[SWS_Adc_00386] ⌈When the ADC Channel Group is in continuous-conversion and software-trigger mode, the function Adc_StopGroupConversion shall stop an ongoing conversion of the group.⌋ (SRS_Adc_12364)
void Adc_StopGroupConversion ( Adc_GroupType groupId );
#endif

#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)		
void Adc_EnableGroupNotification(Adc_GroupType groupId);
void Adc_DisableGroupNotification(Adc_GroupType groupId);
#endif

#if (ADC_GRP_NOTIF_CAPABILITY==STD_ON)
//[SWS_Adc_00075]⌈The function Adc_ReadGroup shall read the latest available conversion results of the requested group.⌋ ()
//[SWS_Adc_00113]⌈The function Adc_ReadGroup shall read the raw converted values without further scaling. The read values shall be aligned according the configuration parameter setting of ADC_RESULT_ALIGNMENT.⌋ (SRS_SPAL_12063, SRS_Adc_12819, SRS_Adc_12292, SRS_Adc_12824)
//[SWS_Adc_00122]⌈If applicable, the function Adc_ReadGroup shall mask out all information or diagnostic bits provided by the conversion but not belonging to the conversion results themselves.⌋ (SRS_Adc_12283, SRS_Adc_12819)
Std_ReturnType Adc_ReadGroup ( Adc_GroupType Group, volatile Adc_ValueGroupType* DataBufferPtr );
#endif

#if (ADC_HW_TRIGGER_API == STD_ON)
//[SWS_Adc_00144] ⌈A group with trigger source hardware, whose trigger was enabled with Adc_EnableHardwareTrigger, shall execute the group channel conversions, whenever a trigger event occurs.⌋ (SRS_Adc_12823)
//[SWS_Adc_00432] ⌈The function Adc_EnableHardwareTrigger shall reset the internal group result buffer pointer, that conversion result storage always starts, after calling Adc_EnableHardwareTrigger, at the result buffer base address which was configured with Adc_SetupResultBuffer.⌋ ()
//[SWS_Adc_00273] ⌈The ADC module’s environment shall guarantee that no concurrent conversions take place on the same HW Unit (happening of different hardware triggers at the same time).⌋ (SRS_Adc_12823)
void Adc_EnableHardwareTrigger ( Adc_GroupType Group );

//[SWS_Adc_00116] ⌈The function Adc_DisableHardwareTrigger shall disable the hardware trigger for the requested ADC Channel group.⌋ (SRS_Adc_12823)
//[SWS_Adc_00429] ⌈The function Adc_DisableHardwareTrigger shall remove any queued start/restart request for the requested ADC Channel group if queuing is enabled.⌋ ()
//[SWS_Adc_00145] ⌈The function Adc_DisableHardwareTrigger shall abort an ongoing conversion, if applicable (supported by the hardware).⌋ (SRS_Adc_12364)
//[SWS_Adc_00157] ⌈If enabled, the function Adc_DisableHardwareTrigger shall disable the notification mechanism for the requested group.⌋ (SRS_Adc_12317, SRS_Adc_12318, SRS_Adc_12364)
void Adc_DisableHardwareTrigger ( Adc_GroupType Group );
#endif

#if(ADC_GET_STREAM_API==STD_ON)
//[SWS_Adc_00214] ⌈The function Adc_GetStreamLastPointer shall set the pointer, passed as parameter (PtrToSamplePtr) to point in the ADC result buffer to the latest result of the first group channel of the last completed conversion round.⌋ (SRS_Adc_12292, SRS_Adc_12802)
//[SWS_Adc_00418]⌈All values which the ADC driver stores in the ADC result buffer, are left without further scaling and shall be aligned according the configuration parameter setting of ADC_RESULT_ALIGNMENT.⌋ ()
//[SWS_Adc_00387]⌈The function Adc_GetStreamLastPointer shall return the number of valid samples per channel, stored in the ADC result buffer.⌋ ()
Adc_StreamNumSampleType Adc_GetStreamLastPointer ( Adc_GroupType Group, volatile Adc_ValueGroupType** PtrToSamplePtr );
#endif
