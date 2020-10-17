/*
 * camera.c
 *
 *  Created on: 2020. 10. 17.
 *      Author: baram
 */




#include "camera.h"
#include "camera/ov2640.h"


#define CAMERA_BUF_MAX      2

static uint16_t  camera_buf_i = 0;
static uint16_t *camera_buf[CAMERA_BUF_MAX];

volatile bool is_requested_capture = false;
volatile bool is_finished_capture = false;


static int cameraIRQ(void *ctx)
{
  if (dvp_get_interrupt(DVP_STS_FRAME_FINISH))
  {
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_clear_interrupt(DVP_STS_FRAME_FINISH);

    if (is_finished_capture != true)
    {
      camera_buf_i ^= 1;
      uint32_t addr;
      addr = (uint32_t)camera_buf[camera_buf_i];
      dvp_set_display_addr(addr);
      is_finished_capture = true;
    }
    is_requested_capture = false;
  }
  else
  {
    dvp_start_convert();
    dvp_clear_interrupt(DVP_STS_FRAME_START);
  }
  return 0;
}



bool cameraInit(void)
{
  bool ret = true;

  /* Init DVP IO map and function settings */
  fpioa_set_function(42, FUNC_CMOS_RST);
  fpioa_set_function(44, FUNC_CMOS_PWDN);
  fpioa_set_function(46, FUNC_CMOS_XCLK);
  fpioa_set_function(43, FUNC_CMOS_VSYNC);
  fpioa_set_function(45, FUNC_CMOS_HREF);
  fpioa_set_function(47, FUNC_CMOS_PCLK);
  fpioa_set_function(41, FUNC_SCCB_SCLK);
  fpioa_set_function(40, FUNC_SCCB_SDA);


  dvp_init(8);
  dvp_set_xclk_rate(24000000);
  dvp_enable_burst();
  dvp_set_output_enable(0, 1);
  dvp_set_output_enable(1, 1);
  dvp_set_image_format(DVP_CFG_RGB_FORMAT);
  dvp_set_image_size(320, 240);


  ov2640Init();

  camera_buf_i = 0;
  for (int i=0; i<CAMERA_BUF_MAX; i++)
  {
    camera_buf[i] = iomem_malloc(320 * 240 * 2);
  }

  uint32_t addr;
  addr = (uint32_t)camera_buf[camera_buf_i];

  dvp_set_display_addr(addr);
  dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
  //dvp_disable_auto();
  dvp_enable_auto();



  plic_set_priority(IRQN_DVP_INTERRUPT, 1);
  plic_irq_register(IRQN_DVP_INTERRUPT, cameraIRQ, NULL);
  plic_irq_enable(IRQN_DVP_INTERRUPT);

  cameraStart();

  return ret;
}

bool cameraIsAvailable(void)
{
  return !is_requested_capture;
}

bool cameraStart(void)
{
  is_requested_capture = true;
  is_finished_capture = false;
  dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
  dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

  return true;
}

bool cameraRequestCapture(void)
{
  if (is_requested_capture == true)
  {
    return false;
  }

  is_requested_capture = true;
  is_finished_capture = false;
  dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
  dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

  return true;
}

bool cameraIsCaptured(void)
{
  return is_finished_capture;
}

uint16_t *cameraGetFrameBuf(void)
{
  return camera_buf[camera_buf_i^1];
}

