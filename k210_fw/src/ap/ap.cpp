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
  uint32_t show_data[2];
  uint16_t x = 0;
  uint16_t y = 0;


  pre_time = millis();
  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);

      show_data[0] = lcdGetFpsTime();
      show_data[1] = lcdGetFps();
    }

    if (lcdDrawAvailable() > 0)
    {
      lcdClearBuffer(black);
      lcdPrintf(0,  0, white, "%d ms", show_data[0]);
      lcdPrintf(0, 16, white, "%d fps", show_data[1]);
      lcdPrintfRect(0, 0, LCD_WIDTH, LCD_HEIGHT, white, 3, LCD_ALIGN_H_CENTER | LCD_ALIGN_V_CENTER,  "RISC_V 보드");

      lcdDrawFillRect(x, 32, 30, 30, red);
      lcdDrawFillRect(lcdGetWidth()-x, 62, 30, 30, green);
      lcdDrawFillRect(x + 30, 92, 30, 30, blue);

      x += 2;

      x %= lcdGetWidth();
      y %= lcdGetHeight();

      lcdRequestDraw();
    }
  }
}

