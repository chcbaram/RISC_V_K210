/*
 * main.cpp
 *
 *  Created on: 2020. 10. 9.
 *      Author: baram
 */




#include "main.h"



#define cmb() __asm__ __volatile__ ("" ::: "memory")
extern "C"{ void * __dso_handle = 0 ;}


int main(void)
{
  apInit();
  apMain();

  return 0;
}
