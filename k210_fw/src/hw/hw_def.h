/*
 * hw_def.h
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"


#define _USE_HW_FLASH
#define _USE_HW_CAMERA


#define _USE_HW_LED
#define      HW_LED_MAX_CH          3

#define _USE_HW_UART
#define      HW_UART_MAX_CH         1

#define _USE_HW_CMDIF
#define      HW_CMDIF_LIST_MAX              32
#define      HW_CMDIF_CMD_STR_MAX           16
#define      HW_CMDIF_CMD_BUF_LENGTH        128

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         2

#define _USE_HW_LCD
#define _USE_HW_ST7789
#define      HW_LCD_WIDTH           320
#define      HW_LCD_HEIGHT          240




#define _USE_GPIOHS_LED_R           0
#define _USE_GPIOHS_LED_G           1
#define _USE_GPIOHS_LED_B           2

#define _USE_GPIOHS_LCD_RST         3
#define _USE_GPIOHS_LCD_DCX         4



#endif /* SRC_HW_HW_DEF_H_ */
