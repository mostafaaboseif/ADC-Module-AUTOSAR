#include "gpio.h"

#define MAX_NB_CHANNELS 8
#define MAX_NB_GROUPS 8

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




typedef uint8_t Adc_GroupType;

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
ADC_TRIG_ALWAYS =0x0F,
NO_HW_TRIG
}Adc_HwTriggerSourceType;


typedef enum Adc_StreamBufferModeType{
ADC_STREAM_BUFFER_LINEAR,
ADC_STREAM_BUFFER_CIRCULAR
}Adc_StreamBufferModeType;
typedef enum SeqOffset{
ADC_SS_BASE   = ADC_SSMUX0,
ADC_SS_STEP   = ADC_SSMUX1 - ADC_SSMUX0,
ADC_SSMUX     = ADC_SSMUX0 - ADC_SSMUX0,
ADC_SSCTL     = ADC_SSCTL0 - ADC_SSMUX0,
ADC_SSFIFO    = ADC_SSFIFO0 - ADC_SSMUX0,
ADC_SSFSTAT  	= ADC_SSFSTAT0 - ADC_SSMUX0
}SeqOffset;

typedef enum Adc_HwTriggerSignalType{
ADC_HW_TRIG_RISING_EDGE, 
ADC_HW_TRIG_FALLING_EDGE, 
ADC_HW_TRIG_BOTH_EDGES
}Adc_HwTriggerSignalType;




typedef struct AdcChannelGroup{
uint8_t GroupId;
AdcModule AdcModule;
Sequencer Sequencer;
Adc_TriggerSourceType Adc_TriggerSourceType;
Adc_HwTriggerSourceType Adc_HwTriggerSourceType;
Adc_HwTriggerSignalType Adc_HwTriggerSignalType;
Adc_StreamBufferModeType Adc_StreamBufferModeType;
uint8_t GroupPriority;
uint8_t NbChannels;
AdcChannel ArrayOfAdcChannels[MAX_NB_CHANNELS];
}AdcChannelGroup;


//array to be intialized in the ADC_init function with the groups

void Adc_init(AdcChannelGroup);

void Adc_SetupResultBuffer (AdcModule , volatile uint32_t *buffer_ptr);

void Adc_StartGroupConversion ( Adc_GroupType Group );

void Adc_EnableGroupNotification(int groupId);

void Adc_DisableGroupNotification(int groupId);
