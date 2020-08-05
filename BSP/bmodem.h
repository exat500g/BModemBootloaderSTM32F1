#ifndef __BMODEM_H_
#define __BMODEM_H_

#include <stdint.h>

int bmodem_receive (uint32_t startAddr,uint32_t endAddr);
void bmodem_finish (void);

const char* bmodem_getErrMessage(void);

#endif
