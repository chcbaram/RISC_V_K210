/*
 * st7789.c
 *
 *  Created on: 2020. 10. 10.
 *      Author: baram
 */




#include "lcd/st7789.h"

#ifdef _USE_HW_ST7789
#include "gpio.h"




#define _PIN_DEF_RST                0
#define _PIN_DEF_DCX                1

#define SIPEED_ST7789_RST_PIN       37
#define SIPEED_ST7789_DCX_PIN       38
#define SIPEED_ST7789_SS_PIN        36
#define SIPEED_ST7789_SCLK_PIN      39

#define SIPEED_ST7789_RST_GPIONUM   6
#define SIPEED_ST7789_DCX_GPIONUM   7
#define SIPEED_ST7789_SS            3



static uint8_t  spi_ch     = 0;
static uint32_t spi_freq   = 20000000;
static uint8_t  spi_dma_ch = 3;

static int32_t _width  = HW_LCD_WIDTH;
static int32_t _height = HW_LCD_HEIGHT;
static void (*frameCallBack)(void) = NULL;

volatile static bool  is_write_frame = false;

static void st7789InitRegs(void);
static void writecommand(uint8_t c);
static void writedata(uint8_t d);

void st7789FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void st7789SetRotation(uint8_t m);
bool st7789Reset(void);





static void TransferDoneISR(void)
{
  if (is_write_frame == true)
  {
    is_write_frame = false;
    if (frameCallBack != NULL)
    {
      frameCallBack();
    }
  }
}

static int dmaCallback(void *ctx)
{
  (void)ctx;

  TransferDoneISR();

  dmac_disable_channel_interrupt(spi_dma_ch);
  return 0;
}

bool st7789Init(void)
{

  st7789Reset();
  return true;
}

bool st7789Reset(void)
{

  gpioPinWrite(_PIN_DEF_DCX, _DEF_HIGH);
  gpioPinWrite(_PIN_DEF_RST, _DEF_HIGH);

  sysctl_clock_enable(SYSCTL_CLOCK_SPI0);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
  fpioa_set_function(SIPEED_ST7789_SS_PIN  , (fpioa_function_t)(FUNC_SPI0_SS0 + SIPEED_ST7789_SS));
  fpioa_set_function(SIPEED_ST7789_SCLK_PIN, (fpioa_function_t)FUNC_SPI0_SCLK);


  sysctl_set_spi0_dvp_data(1);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
  spi_set_clk_rate(spi_ch, spi_freq);


  //-- Reset Lcd
  //
  gpioPinWrite(_PIN_DEF_RST, _DEF_HIGH);
  delay(10);
  gpioPinWrite(_PIN_DEF_RST, _DEF_LOW);
  delay(100);
  gpioPinWrite(_PIN_DEF_RST, _DEF_HIGH);
  delay(100);



  st7789InitRegs();
  st7789SetRotation(1);

  dmac_irq_register(spi_dma_ch, dmaCallback, NULL, 5);
  dmac_disable_channel_interrupt(spi_dma_ch);

  return true;
}

bool st7789InitDriver(lcd_driver_t *p_driver)
{
  p_driver->init = st7789Init;
  p_driver->reset = st7789Reset;
  p_driver->setWindow = st7789SetWindow;
  p_driver->getWidth = st7789GetWidth;
  p_driver->getHeight = st7789GetHeight;
  p_driver->setCallBack = st7789SetCallBack;
  p_driver->sendBuffer = st7789SendBuffer;
  return true;
}

void st7789InitRegs(void)
{
#if 1
  writecommand(ST7789_SLPOUT);
  delay(120);

  writecommand(ST7789_MADCTL);
  writedata(0x00);

  writecommand(ST7789_COLMOD);
  writedata(0x05);
  //writedata(0x55);

  writecommand(ST7789_PORCTRL);
  writedata(0x0C);
  writedata(0x0C);
  writedata(0x00);
  writedata(0x33);
  writedata(0x33);

  writecommand(ST7789_GCTRL);
  writedata(0x35);

  writecommand(ST7789_VCOMS);
  writedata(0x38);

  writecommand(ST7789_LCMCTRL);
  writedata(0x2C);

  writecommand(ST7789_VDVVRHEN);
  writedata(0x01);

  writecommand(ST7789_VRHS);
  writedata(0x08);

  writecommand(ST7789_VDVS);
  writedata(0x20);

  writecommand(ST7789_FRCTRL2);
  writedata(0x0C);

  writecommand(ST7789_PWCTRL1);
  writedata(0xA4);
  writedata(0xA1);

  writecommand(ST7789_PVGAMCTRL);
  writedata(0xD0);
  writedata(0x00);
  writedata(0x0E);
  writedata(0x0E);
  writedata(0x15);
  writedata(0x0D);
  writedata(0x37);
  writedata(0x43);
  writedata(0x47);
  writedata(0x09);
  writedata(0x15);
  writedata(0x12);
  writedata(0x18);
  writedata(0x19);

  writecommand(ST7789_NVGAMCTRL);
  writedata(0xD0);
  writedata(0x00);
  writedata(0x05);
  writedata(0x0D);
  writedata(0x0C);
  writedata(0x06);
  writedata(0x2D);
  writedata(0x44);
  writedata(0x40);
  writedata(0x0E);
  writedata(0x1C);
  writedata(0x18);
  writedata(0x18);
  writedata(0x19);



  writecommand(ST7789_DISPON);
#else
  uint8_t data;

  writecommand(ST7789_SWRESET);
  delay(120);
  /*exit sleep*/
  writecommand(ST7789_SLPOUT);
  delay(120);

  /*pixel format*/
  writecommand(ST7789_COLMOD);
  writedata(0x55);

//  /lcd_set_direction(DIR_YX_RLDU);

  /*display on*/
  writecommand(ST7789_DISPON);
#endif
}


uint16_t st7789GetWidth(void)
{
  return HW_LCD_WIDTH;
}

uint16_t st7789GetHeight(void)
{
  return HW_LCD_HEIGHT;
}

void writecommand(uint8_t c)
{
  gpioPinWrite(_PIN_DEF_DCX, _DEF_LOW);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
  spi_init_non_standard(spi_ch, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                        SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
  spi_send_data_normal_dma(spi_dma_ch, spi_ch, SIPEED_ST7789_SS, (uint8_t *)(&c), 1, SPI_TRANS_CHAR);
}

void writedata(uint8_t d)
{
  gpioPinWrite(_PIN_DEF_DCX, _DEF_HIGH);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
  spi_init_non_standard(spi_ch, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                        SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
  spi_send_data_normal_dma(spi_dma_ch, spi_ch, SIPEED_ST7789_SS, (uint8_t *)(&d), 1, SPI_TRANS_CHAR);
}

void writebytes(uint8_t *p_data, uint32_t length)
{
  gpioPinWrite(_PIN_DEF_DCX, _DEF_HIGH);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
  spi_init_non_standard(spi_ch, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                        SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
  spi_send_data_normal_dma(spi_dma_ch, spi_ch, SIPEED_ST7789_SS, p_data, length, SPI_TRANS_CHAR);
}

void writeword(uint8_t *data_buf, uint32_t length)
{
  gpioPinWrite(_PIN_DEF_DCX, _DEF_HIGH);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 1);

  spi_init_non_standard(spi_ch, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                        SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
  spi_send_data_normal_dma(spi_dma_ch, spi_ch, SIPEED_ST7789_SS, (uint32_t *)data_buf, length, SPI_TRANS_INT);
}

void write_dma_start(uint8_t *data_buf, uint32_t length)
{
  gpioPinWrite(_PIN_DEF_DCX, _DEF_HIGH);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 1);

  spi_init_non_standard(spi_ch, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                        SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
  spi_send_dma_start(spi_dma_ch, spi_ch, SIPEED_ST7789_SS, (uint32_t *)data_buf, length);
}



void writefill(uint32_t *data_buf, uint32_t length)
{
  gpioPinWrite(_PIN_DEF_DCX, _DEF_HIGH);

  spi_init(spi_ch, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
  spi_init_non_standard(spi_ch, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                        SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
  spi_fill_data_dma(spi_dma_ch, spi_ch, SIPEED_ST7789_SS, data_buf, length);
}



#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04


void st7789SetRotation(uint8_t m)
{
  uint8_t rotation;

  rotation = m % 4; // Limit the range of values to 0-7

  writecommand(ST7789_MADCTL);
  switch (rotation)
  {
    case 0:
      writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
      break;
    case 1:
      writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
      break;
    case 2:
      writedata(MADCTL_RGB);
      break;
    case 3:
      writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
      break;
  }
}

void st7789SetWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
#if 1
  uint8_t data[4] = {0};

  data[0] = (uint8_t)(x0 >> 8);
  data[1] = (uint8_t)(x0);
  data[2] = (uint8_t)(x1 >> 8);
  data[3] = (uint8_t)(x1);
  writecommand(ST7789_CASET);
  writebytes(data, 4);

  data[0] = (uint8_t)(y0 >> 8);
  data[1] = (uint8_t)(y0);
  data[2] = (uint8_t)(y1 >> 8);
  data[3] = (uint8_t)(y1);
  writecommand(ST7789_RASET);
  writebytes(data, 4);

  writecommand(ST7789_RAMWR);
#else
  uint8_t buf[8];

  buf[0] = ST7789_CASET;
  buf[1] = x0>>8;
  buf[2] = x0>>0;
  buf[3] = x1>>8;
  buf[4] = x1>>0;
  writecommand(buf[0]);
  writedata(buf[1]);
  writedata(buf[2]);
  writedata(buf[3]);
  writedata(buf[4]);


  buf[0] = ST7789_RASET;
  buf[1] = y0>>8;
  buf[2] = y0>>0;
  buf[3] = y1>>8;
  buf[4] = y1>>0;
  writecommand(buf[0]);
  writedata(buf[1]);
  writedata(buf[2]);
  writedata(buf[3]);
  writedata(buf[4]);

  buf[0] = ST7789_RAMWR;
  writecommand(buf[0]);
#endif
}



void st7789FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  // Clipping
  if ((x >= _width) || (y >= _height)) return;

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if ((x + w) > _width)  w = _width  - x;
  if ((y + h) > _height) h = _height - y;

  if ((w < 1) || (h < 1)) return;



  st7789SetWindow(x, y, x + w - 1, y + h - 1);

  uint32_t data = ((uint32_t)color << 16) | (uint32_t)color;

  writefill(&data, w*h*4);
}

bool st7789SendBuffer(uint8_t *p_data, uint32_t length, uint32_t timeout_ms)
{
  is_write_frame = true;


  dmac_enable_channel_interrupt(spi_dma_ch);
  write_dma_start(p_data, length/2);

  return true;
}

bool st7789SetCallBack(void (*p_func)(void))
{
  frameCallBack = p_func;

  return true;
}


#endif
