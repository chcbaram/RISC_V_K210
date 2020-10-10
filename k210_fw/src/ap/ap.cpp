/*
 * ap.cpp
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "ap.h"


#define PIN_LED  13
#define GPIO_LED 3


void apInit(void)
{
  hwInit();
}

void apMain(void)
{
  uint32_t pre_time;
  uint32_t index = 0;

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      uartPrintf(_DEF_UART1, "index %d\n", index++);
    }

    if (uartAvailable(_DEF_UART1) > 0)
    {
      uartPrintf(_DEF_UART1, "rx 0x%X\n", uartRead(_DEF_UART1));
    }
  }
}

