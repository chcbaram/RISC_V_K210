/*
 * bsp.c
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "bsp.h"
#include "uart.h"




void bspInit(void)
{
  sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL1, 300000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

  plic_init();
  uarths_init();
  gpio_init();
  fpioa_init();
  dmac_init();

  sysctl_set_power_mode(SYSCTL_POWER_BANK6,SYSCTL_POWER_V18);
  sysctl_set_power_mode(SYSCTL_POWER_BANK7,SYSCTL_POWER_V18);
  sysctl_enable_irq();
}

void delay(uint32_t delay_ms)
{
  msleep(delay_ms);
}

uint32_t millis(void)
{
  return sysctl_get_time_us()/1000;
}

uint32_t micros(void)
{
  return sysctl_get_time_us();
}

