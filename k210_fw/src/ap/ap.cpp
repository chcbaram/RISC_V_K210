/*
 * ap.cpp
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "ap.h"



spinlock_t lock = SPINLOCK_INIT;


int apMainCore1(void *ctx);



void apInit(void)
{
  hwInit();

  cmdifOpen(_DEF_UART1, 115200);


  register_core1(apMainCore1, NULL);
}

int apMainCore1(void *ctx)
{
  uint32_t count = 0;


  while(1)
  {
    spinlock_lock(&lock);
    printf("Core1 Run %d\n", count++);
    spinlock_unlock(&lock);
    delay(1000);
  }
}

void apMain(void)
{
  uint32_t pre_time;
  uint32_t show_data[2];
  uint16_t x = 0;
  uint16_t y = 0;


  cameraRequestCapture();

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

      spinlock_lock(&lock);
      printf("Core0 Run \n");
      spinlock_unlock(&lock);
    }

    if (lcdDrawAvailable() > 0 && cameraIsCaptured() == true)
    {
      lcdClearBuffer(black);

      uint16_t *camera_buf;
      uint16_t *lcd_buf;

      camera_buf = cameraGetFrameBuf();
      lcd_buf = lcdGetFrameBuffer();

      for (int i=0; i<320*240; i+=2)
      {
        lcd_buf[i+0] = camera_buf[i+1];
        lcd_buf[i+1] = camera_buf[i+0];
      }


      lcdDrawFillRect(0, 0, 48, 32, black);

      lcdPrintf(0,  0, red, "%d ms", show_data[0]);
      lcdPrintf(0, 16, red, "%d fps", show_data[1]);
      lcdPrintfRect(0, 0, LCD_WIDTH, LCD_HEIGHT, red, 3, LCD_ALIGN_H_CENTER | LCD_ALIGN_V_BOTTOM,  "RISC_V 보드");

      lcdDrawFillRect(x, 32, 30, 30, red);
      lcdDrawFillRect(lcdGetWidth()-x, 62, 30, 30, green);
      lcdDrawFillRect(x + 30, 92, 30, 30, blue);

      x += 2;

      x %= lcdGetWidth();
      y %= lcdGetHeight();



      cameraRequestCapture();
      lcdRequestDraw();
    }
  }
}

