#ifndef __LOADER_SRC_H
#define __LOADER_SRC_H

#include "stm32h7xx_hal.h"

int Init (uint8_t configureMemoryMappedMode);
int Write (uint32_t Address, uint32_t Size, uint8_t *buffer);
//int MassErase (void);
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
int MassErase (void);
//int Read(uint32_t Address, uint32_t Size, uint8_t *Buffer);

#endif

