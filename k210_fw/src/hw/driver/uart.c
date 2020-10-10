/*
 * uart.c
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "uart.h"
#include "qbuffer.h"
#ifdef _USE_HW_VCP
#include "vcp.h"
#endif



#define UART_MODE_POLLING       0
#define UART_MODE_INTERRUPT     1
#define UART_MODE_DMA           2
#define UART_MODE_VCP           3

#define UART_HW_NONE            0
#define UART_HW_STM32_VCP       1
#define UART_HW_STM32_UART      2



#define UART_RX_BUF_LENGTH      1024





typedef struct
{
  bool     is_open;
  uint8_t  ch;
  uint32_t baud;
  uint8_t  tx_mode;
  uint8_t  rx_mode;
  uint8_t  hw_driver;

  uint8_t  rx_buf[UART_RX_BUF_LENGTH];

  qbuffer_t qbuffer_rx;

  uart_device_number_t handle;
} Uart_t;



static __attribute__((section(".sram_d3")))  Uart_t uart_tbl[UART_MAX_CH];





static void uartStartRx(uint8_t channel);
static int uartCallbackCh1(void *ctx);


bool uartInit(void)
{
  uint8_t i;


  for (i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open  = false;
    uart_tbl[i].rx_mode  = UART_MODE_POLLING;
    uart_tbl[i].tx_mode  = UART_MODE_POLLING;
    uart_tbl[i].hw_driver = UART_HW_NONE;
  }

  return true;
}

bool uartOpen(uint8_t channel, uint32_t baud)
{
  bool ret = false;
  Uart_t *p_uart;


  if (channel >= UART_MAX_CH)
  {
    return false;
  }

  switch(channel)
  {
    case _DEF_UART1:
      p_uart = &uart_tbl[channel];

      p_uart->baud      = baud;
      p_uart->hw_driver = UART_HW_STM32_UART;
      p_uart->rx_mode   = UART_MODE_INTERRUPT;
      p_uart->tx_mode   = UART_MODE_POLLING;
      p_uart->handle =  UART_DEVICE_3;


      fpioa_set_function(4, FUNC_UART3_RX);
      fpioa_set_function(5, FUNC_UART3_TX);
      uart_init(p_uart->handle);
      uart_configure(p_uart->handle, baud, 8, UART_STOP_1, UART_PARITY_NONE);

      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);

      uart_set_receive_trigger(p_uart->handle, UART_RECEIVE_FIFO_1);
      uart_irq_register(p_uart->handle, UART_RECEIVE, uartCallbackCh1, NULL, 5);

      //delay(10);

      uartFlush(channel);

      p_uart->is_open  = true;

      uartStartRx(channel);
      ret = true;
      break;

    case _DEF_UART2:
      p_uart = &uart_tbl[channel];

      p_uart->baud     = baud;
      p_uart->is_open  = true;

      p_uart->rx_mode  = UART_MODE_VCP;
      p_uart->tx_mode  = UART_MODE_VCP;
      p_uart->hw_driver = UART_HW_STM32_VCP;
      uartStartRx(channel);
      ret = true;
      break;
  }

  return ret;
}

bool uartClose(uint8_t channel)
{
  bool ret = false;


  if (channel >= UART_MAX_CH)
  {
    return false;
  }


  if (uart_tbl[channel].is_open == true && uart_tbl[channel].hw_driver == UART_HW_STM32_UART)
  {
  }


  return ret;
}

void uartStartRx(uint8_t channel)
{
  Uart_t *p_uart = &uart_tbl[channel];

  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
  }
}

uint32_t uartGetBaud(uint8_t channel)
{
  if (channel >= UART_MAX_CH)
  {
    return 0;
  }

  return uart_tbl[channel].baud;
}

uint32_t uartAvailable(uint8_t channel)
{
  uint32_t ret = 0;
  Uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }
#ifdef _USE_HW_VCP
  if (p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpAvailable();
  }
#endif
  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    ret = qbufferAvailable(&uart_tbl[channel].qbuffer_rx);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    ret = qbufferAvailable(&p_uart->qbuffer_rx);
  }

  return ret;
}

void uartFlush(uint8_t channel)
{
#ifdef _USE_HW_VCP
  if (uart_tbl[channel].rx_mode == UART_MODE_VCP)
  {
    vcpFlush();
  }
#endif

  if (uart_tbl[channel].rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferFlush(&uart_tbl[channel].qbuffer_rx);
  }
  if (uart_tbl[channel].rx_mode == UART_MODE_DMA)
  {
    //uart_tbl[channel].qbuffer_rx.ptr_in  = uart_tbl[channel].qbuffer_rx.length - ((DMA_Channel_TypeDef *)uart_tbl[channel].hdma_rx->Instance)->CNDTR;
    //uart_tbl[channel].qbuffer_rx.ptr_out = uart_tbl[channel].qbuffer_rx.ptr_in;
  }
}

void uartPutch(uint8_t channel, uint8_t ch)
{
  uartWrite(channel, &ch, 1 );
}

uint8_t uartGetch(uint8_t channel)
{
  uint8_t ret = 0;


  while(1)
  {
    if (uartAvailable(channel) > 0)
    {
      ret = uartRead(channel);
      break;
    }
  }

  return ret;
}

int32_t uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length)
{
  int32_t ret = 0;
  Uart_t *p_uart = &uart_tbl[channel];


#ifdef _USE_HW_VCP
  if (p_uart->tx_mode == UART_MODE_VCP)
  {
    vcpWrite(p_data, length);
  }
#endif
  if (p_uart->tx_mode == UART_MODE_POLLING)
  {
    ret = uart_send_data(p_uart->handle, (const char *)p_data, length);
  }
  if (p_uart->tx_mode == UART_MODE_DMA)
  {
    //if (HAL_UART_Transmit_DMA(p_uart->handle, p_data, length) == HAL_OK)
    {
      ret = length;
    }
  }
  return ret;
}

uint8_t uartRead(uint8_t channel)
{
  uint8_t ret = 0;
  Uart_t *p_uart = &uart_tbl[channel];


#ifdef _USE_HW_VCP
  if (p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpRead();
  }
#endif
  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  return ret;
}

int32_t uartPrintf(uint8_t channel, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];

  if (uart_tbl[channel].is_open != true)
  {
    return 0;
  }

  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  ret = uartWrite(channel, (uint8_t *)print_buffer, len);

  return ret;
}

static int uartCallbackCh1(void *ctx)
{
  char data;
  (void)ctx;

  while(uart_receive_data(uart_tbl[_DEF_UART1].handle, &data, 1))
  {
    qbufferWrite(&uart_tbl[_DEF_UART1].qbuffer_rx, (uint8_t *)&data, 1);
  }
  return 0;
}
