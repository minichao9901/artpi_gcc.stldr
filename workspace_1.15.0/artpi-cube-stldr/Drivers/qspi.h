#ifndef __QSPI_H
#define __QSPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

int qspi_deinit(void);
int qspi_init(int flashsize);

int qspi_write(const void *pbuf, int size);
int qspi_read(void * const pbuf, int size);

int qspi_command(unsigned int ins, unsigned int addr, unsigned int dummy, unsigned int insmode, unsigned int addrmode, unsigned int addrsize, unsigned int datamode);
int qspi_memory_mapped_config(unsigned int command, unsigned int addrbit);

#ifdef __cplusplus
}
#endif

#endif
