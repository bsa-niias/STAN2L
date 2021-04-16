#ifndef __STAN2L_IPC_HPP__
#define __STAN2L_IPC_HPP__

// ipc
const long IPC_TUMS_TS = 1; // for TTUMS_ipc_ts::ipc_std_mtype

typedef struct 
{
    long     ipc_std_mtype; 
    uint16_t _PacketNumber;
    uint8_t  _BracketStart;
    uint8_t  _DataHead;
    uint8_t  _TUMSNum;
    uint8_t  _DataGroup;
    uint8_t  _DataSubGroup;
    uint8_t  _Data [5];
    uint8_t  _ChannelStatus;
    uint8_t  _DiagType;
    uint8_t  _Diag [5];
    uint8_t  _Mif;
    uint8_t  _BracketEnd;
    uint16_t _crc16; // include from '(' to ')' - current == 0x0000
//
} __attribute__((packed)) TTUMS_IPC_TS;


#endif