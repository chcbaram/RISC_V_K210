/*
 * gpio.c
 *
 *  Created on: 2020. 10. 10.
 *      Author: baram
 */




#include "gpio.h"
#include "cmdif.h"


typedef struct
{
  fpioa_function_t  port;
  uint16_t          pin;
  uint16_t          io_number;
  uint8_t           mode;
  gpio_pin_value_t  on_state;
  gpio_pin_value_t  init_state;
} gpio_tbl_t;


gpio_tbl_t gpio_tbl[GPIO_MAX_CH] =
{
  {FUNC_GPIOHS0 + _USE_GPIOHS_LCD_RST, _USE_GPIOHS_LCD_RST, 37, _DEF_OUTPUT, GPIO_PV_HIGH, GPIO_PV_HIGH},
  {FUNC_GPIOHS0 + _USE_GPIOHS_LCD_DCX, _USE_GPIOHS_LCD_DCX, 38, _DEF_OUTPUT, GPIO_PV_HIGH, GPIO_PV_HIGH},
};



void gpioCmdif(void);



bool gpioInit(void)
{
  uint32_t i;


  for (i=0; i<GPIO_MAX_CH; i++)
  {
    gpioPinMode(i, gpio_tbl[i].mode);

    if (gpio_tbl[i].mode == _DEF_OUTPUT)
    {
      gpioPinWrite(i, gpio_tbl[i].init_state);
    }
  }


  cmdifAdd("gpio", gpioCmdif);

  return true;
}

void gpioPinMode(uint8_t channel, uint8_t mode)
{
  gpio_drive_mode_t pin_mode = GPIO_DM_OUTPUT;


  if (channel >= GPIO_MAX_CH)
  {
    return;
  }

  switch (mode)
  {
    case _DEF_INPUT:
      pin_mode = GPIO_DM_INPUT;
      break;

    case _DEF_INPUT_PULLUP:
      pin_mode = GPIO_DM_INPUT_PULL_UP;
      break;

    case _DEF_INPUT_PULLDOWN:
      pin_mode = GPIO_DM_INPUT_PULL_DOWN;
      break;

    case _DEF_OUTPUT:
      pin_mode = GPIO_DM_OUTPUT;
      break;

    case _DEF_OUTPUT_PULLUP:
      pin_mode = GPIO_DM_OUTPUT;
      break;

    case _DEF_OUTPUT_PULLDOWN:
      pin_mode = GPIO_DM_OUTPUT;
      break;
  }

  fpioa_set_function(gpio_tbl[channel].io_number, gpio_tbl[channel].port);
  gpiohs_set_drive_mode(gpio_tbl[channel].pin, pin_mode);
}

void gpioPinWrite(uint8_t channel, uint8_t value)
{
  gpio_pin_value_t pin_value;

  if (channel >= GPIO_MAX_CH)
  {
    return;
  }

  if (value > 0)
  {
    pin_value = GPIO_PV_HIGH;
  }
  else
  {
    pin_value = GPIO_PV_LOW;
  }

  gpiohs_set_pin(gpio_tbl[channel].pin, pin_value);
}

uint8_t gpioPinRead(uint8_t channel)
{
  if (gpiohs_get_pin(gpio_tbl[channel].pin) == GPIO_PV_HIGH)
  {
    return _DEF_HIGH;
  }
  else
  {
    return _DEF_LOW;
  }
}

void gpioPinToggle(uint8_t channel)
{
  if (channel >= GPIO_MAX_CH)
  {
    return;
  }
}





//-- gpioCmdif
//
void gpioCmdif(void)
{
  bool ret = true;
  uint8_t number;
  uint8_t state;


  if (cmdifGetParamCnt() == 1)
  {
    if (cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (int i=0; i<GPIO_MAX_CH; i++)
        {
          cmdifPrintf("%d ", gpioPinRead(i));
        }
        cmdifPrintf("\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    if (cmdifHasString("read", 0) == true)
    {
      number = (uint8_t)cmdifGetParam(1);

      while(cmdifRxAvailable() == 0)
      {
        cmdifPrintf("gpio %d : %d\r\n", number, gpioPinRead(number));
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 3)
  {
    if (cmdifHasString("write", 0) == true)
    {
      number = (uint8_t)cmdifGetParam(1);
      state = (uint8_t)cmdifGetParam(2);

      gpioPinWrite(number, state);

      cmdifPrintf("gpio write %d : %d\r\n", number, gpioPinRead(number));
    }
    if (cmdifHasString("on", 0) == true)
    {
      uint32_t on_time;

      number = (uint8_t)cmdifGetParam(1);
      on_time = (uint32_t)cmdifGetParam(2);

      gpioPinWrite(number, 1);
      delay(on_time);
      gpioPinWrite(number, 0);

      cmdifPrintf("gpio on %d : %d ms\r\n", number, on_time);
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "gpio show\n");
    cmdifPrintf( "gpio read  0~%d\n", GPIO_MAX_CH-1);
    cmdifPrintf( "gpio write 0~%d [0:1]\n", GPIO_MAX_CH-1);
    cmdifPrintf( "gpio on 0~%d ms[0~3000]\n", GPIO_MAX_CH-1);
  }
}

