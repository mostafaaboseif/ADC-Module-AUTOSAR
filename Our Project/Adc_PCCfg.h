/* Adds / removes the services Adc_StartGroupConversion() and Adc_StopGroupConversion() from the code. */
#define ADC_ENABLE_START_STOP_GROUP_API			STD_ON

/* Determines, if the group notification mechanism (the functions to enable and disable the notifications) is available at runtime.*/
#define ADC_GRP_NOTIF_CAPABILITY 				STD_ON

/* Adds / removes the service Adc_ReadGroup() and from the code. */
#define ADC_READ_GROUP_API						STD_ON

/* used to enable or disable deinit api */
#define ADC_DEINIT_API 							STD_OFF		

/* used to enable or disable Development error detection for Adc driver */
#define ADC_DEV_ERROR_DETECT					STD_OFF

/* Enable or diable the limit checking feature in the adc driver */ 
#define ADC_ENABLE_LIMIT_CHECK					STD_OFF		

/* Determines, if the queuing mechanism is active in case of priority mechanism disabled*/
#define ADC_ENABLE_QUEUING						STD_OFF		

/* Adds / removes the services Adc_EnableHardwareTrigger() and Adc_DisableHardwareTrigger() from the code. */ 
#define ADC_HW_TRIGGER_API						STD_OFF

/* Adds / removes all power state management related APIs */
#define ADC_LOW_POWER_STATES_SUPPORT			STD_OFF

/* Enables / disables support of the ADCDriver to the asynchronous power state transition. */
#define ADC_POWER_STAT_ASYNCH_TRANSITION_MODE	STD_OFF

/* Adds / removes the service Adc_GetVersionInfo() from the code.  */
#define ADC_VERSION_INFO_API					STD_OFF

/*Determines whether a priority mechanism is available for prioritization of the conversion requests and if available*/ 
#define ADC_PRIORITY_IMPLEMENTATION				ADC_PRIORITY_HW

/* Alignment of ADC raw results in ADC result buffer (left/right alignment).*/
#define ADC_RESULT_ALIGNMENT					ADC_ALIGN_RIGHT

