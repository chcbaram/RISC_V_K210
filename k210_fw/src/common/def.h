/*
 * def.h
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>




#define _DEF_LED1                   0
#define _DEF_LED2                   1
#define _DEF_LED3                   2
#define _DEF_LED4                   3


#define _DEF_UART1                  0
#define _DEF_UART2                  1
#define _DEF_UART3                  2
#define _DEF_UART4                  3







#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifndef map
#define map(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif



#endif /* SRC_COMMON_DEF_H_ */
