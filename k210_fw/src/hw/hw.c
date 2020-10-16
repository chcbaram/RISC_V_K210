/*
 * hw.c
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "hw.h"
#include "lcd/st7789.h"



void hwInit(void)
{
  bspInit();

  cmdifInit();
  ledInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  gpioInit();

  lcdInit();
}




