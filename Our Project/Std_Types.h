#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "stdint.h"

/*
 * Describes the standard return Type Definitions used in the project 
 */
typedef uint8_t Std_ReturnType;


#define STD_HIGH	0x01u		/* Standard HIGH */
#define STD_LOW		0x00u		/* Stanard  low */

#define STD_ACTIVE	0x01u		/* Logical State Active*/
#define STD_IDLE	0x00u		/* Logical State Idle*/

#define STD_ON		0x01u		/* Standard On*/
#define STD_OFF		0x00u		/* Standard Off*/

#define E_OK		((Std_ReturnType)0x00u)		/* Function Return Ok */
#define E_NOT_OK	((Std_ReturnType)0x01u)		/* Function Return NOT Ok */


#endif
