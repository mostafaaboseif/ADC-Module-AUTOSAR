#include "adc.h"


Adc_ConfigType ArrayOfAdcChannels[MAX_NB_GROUPS] = {
{
0,    //ID
ADC0,
SS0,
ADC_TRIG_SRC_SW, 
NO_HW_TRIG,
ADC_HW_TRIG_RISING_EDGE,
ADC_CONV_MODE_CONTINUOUS,
ADC_ACCESS_MODE_SINGLE,
3,
ADC_STREAM_BUFFER_CIRCULAR,
ADC_IDLE,
0,   //Priority
2,   //Nb of channels
{PE3,PE1} 
}
,
{
1,    //ID
ADC0,
SS1, 
ADC_TRIG_SRC_SW, 
NO_HW_TRIG,
ADC_HW_TRIG_RISING_EDGE, 
ADC_CONV_MODE_CONTINUOUS,
ADC_ACCESS_MODE_SINGLE,
2,
ADC_STREAM_BUFFER_CIRCULAR,
ADC_IDLE,
1,   //Priority
1,   //Nb of channels
{PD2} 
}
};
