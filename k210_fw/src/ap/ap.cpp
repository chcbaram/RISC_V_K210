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

  cmdifOpen(_DEF_UART1, 115200);
}

void apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();
  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}

