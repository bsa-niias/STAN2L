// Release calculate checksum SRS-16

#ifndef __CRC16_H__
#define __CRC16_H__

#include <stdint.h>

extern uint16_t crc16_table [256];

uint16_t CalculateCRC16 (void *pData, uint32_t uiDataLen);
uint16_t RoutineCRC16Char2i16 (uint8_t uiCh);

#endif