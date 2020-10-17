/*
 * ov2640.h
 *
 *  Created on: 2020. 10. 17.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_CAMERA_OV2640_H_
#define SRC_COMMON_HW_INCLUDE_CAMERA_OV2640_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#define OV2640_ADDR         0x60


int ov2640Init(void);
int ov2640ReadId(uint16_t *manuf_id, uint16_t *device_id);



#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_CAMERA_OV2640_H_ */
