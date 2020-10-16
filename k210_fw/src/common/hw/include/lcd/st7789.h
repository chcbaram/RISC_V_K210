/*
 * st7789.h
 *
 *  Created on: 2020. 4. 1.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_ST7789_H_
#define SRC_COMMON_HW_INCLUDE_ST7789_H_


#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"


#ifdef _USE_HW_ST7789

#include "lcd.h"
#include "st7789_regs.h"





bool st7789Init(void);
bool st7789InitDriver(lcd_driver_t *p_driver);
bool st7789DrawAvailable(void);
bool st7789RequestDraw(void);
void st7789SetWindow(int32_t x, int32_t y, int32_t w, int32_t h);

uint16_t st7789GetWidth(void);
uint16_t st7789GetHeight(void);

bool st7789SetCallBack(void (*p_func)(void));
bool st7789SendBuffer(uint8_t *p_data, uint32_t length, uint32_t timeout_ms);
bool st7789DrawBuffer(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, uint16_t h);
bool st7789DrawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w);


#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_ST7789_H_ */
