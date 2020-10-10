/*
 * bsp.h
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_


#include "def.h"

#include "lib/bsp/include/bsp.h"

#include "sysctl.h"
#include "encoding.h"
#include "platform.h"
#include "sleep.h"
#include "fpioa.h"
#include "gpiohs.h"
#include "gpio.h"
#include "gpio_common.h"
#include "plic.h"
#include "uarths.h"



#ifdef __cplusplus
extern "C" {
#endif




#define logPrintf(...)    printf(__VA_ARGS__)




void bspInit(void);


void delay(uint32_t delay_ms);
uint32_t millis(void);
uint32_t micros(void);




#ifdef __cplusplus
}
#endif



#endif /* SRC_BSP_BSP_H_ */
