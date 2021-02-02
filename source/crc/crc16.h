// Release calculate checksum SRS-16

#ifndef __CRC16_H__
#define __CRC16_H__

#include <stdint.h>

extern uint16_t crc16_table [256];

uint16_t CalculateCRC16 (void *pData, uint32_t uiDataLen);
uint16_t RoutineCRC16Char2ui16 (uint8_t uiCh);

uint8_t RoutineCRC16ui162Char (uint16_t ui);

#endif