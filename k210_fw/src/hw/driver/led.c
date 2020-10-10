/*
 * led.c
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "led.h"
#include "fpioa.h"
#include "gpiohs.h"


typedef struct
{
  fpioa_function_t port;
  uint16_t         pin;
  uint16_t         io_number;
  gpio_pin_value_t on_state;
  gpio_pin_value_t off_state;
} led_tbl_t;


const led_tbl_t led_tbl[LED_MAX_CH] =
{
  {FUNC_GPIOHS0, 0, 14, GPIO_PV_LOW, GPIO_PV_HIGH, GPIO_PV_LOW},
  {FUNC_GPIOHS1, 1, 13, GPIO_PV_LOW, GPIO_PV_HIGH, GPIO_PV_LOW},
  {FUNC_GPIOHS2, 2, 12, GPIO_PV_LOW, GPIO_PV_HIGH, GPIO_PV_LOW},
};


static bool is_init = false;
static gpio_pin_value_t pin_state[LED_MAX_CH];

bool ledInit(void)
{
  uint32_t i;


  for (i=0; i<LED_MAX_CH; i++)
  {
    fpioa_set_function(led_tbl[i].io_number, led_tbl[i].port);
    gpiohs_set_drive_mode(led_tbl[i].pin, (gpio_drive_mode_t)GPIO_DM_OUTPUT);

    ledOff(i);
  }


  is_init = true;

  return true;
}

bool ledIsInit(void)
{
  return is_init;
}

void ledOn(uint8_t ch)
{
  if (ch < LED_MAX_CH)
  {
    gpiohs_set_pin(led_tbl[ch].pin, led_tbl[ch].on_state);
    pin_state[ch] = led_tbl[ch].on_state;
  }
}

void ledOff(uint8_t ch)
{
  if (ch < LED_MAX_CH)
  {
    gpiohs_set_pin(led_tbl[ch].pin, led_tbl[ch].off_state);
    pin_state[ch] = led_tbl[ch].off_state;
  }
}

void ledToggle(uint8_t ch)
{
  if (ch < LED_MAX_CH)
  {
    if (pin_state[ch] == led_tbl[ch].on_state)
    {
      ledOff(ch);
    }
    else
    {
      ledOn(ch);
    }
  }
}
