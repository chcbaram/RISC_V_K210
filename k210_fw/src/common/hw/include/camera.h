/*
 * camera.h
 *
 *  Created on: 2020. 10. 17.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_CAMERA_H_
#define SRC_COMMON_HW_INCLUDE_CAMERA_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_CAMERA



bool cameraInit(void);
bool cameraIsAvailable(void);
bool cameraRequestCapture(void);
bool cameraIsCaptured(void);
uint16_t *cameraGetFrameBuf(void);


#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_CAMERA_H_ */
