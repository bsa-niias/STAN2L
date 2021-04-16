//comment
//  HELP for brain
//       https://ru.stackoverflow.com/questions/376364/%D0%A7%D1%82%D0%BE-%D0%B4%D0%B5%D0%BB%D0%B0%D0%B5%D1%82-select-%D0%B8-fd-isset
//       https://crccalc 
//               (CRC-16/GENIBUS)

// system include
#include <stdio.h>
#include <stdlib.h>
//#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // close (...),
#include <errno.h>
#include <termios.h>
#include <string.h> // bzero (...),
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <time.h>
// for ipc queue
#include <sys/ipc.h>
#include <sys/msg.h>

// dev include
#include "source/crc/crc16.h"
#include "source/cfg/prgmcfg.h"
#include "mtx.hpp"
#include "console.hpp"
#include "sp.hpp"     // class SerialPort { ... }
#include "circlebuf.hpp"

typedef struct 
{
    uint8_t _BracketStart;
    uint8_t _StationID [6];
    uint8_t _MsgNum [4];
    uint8_t _DataHead;
    uint8_t _TUMSNum;
    uint8_t _DataGroup;
    uint8_t _DataSubGroup;
    uint8_t _Data [5];
    uint8_t _ChannelStatus;
    uint8_t _DiagType;
    uint8_t _Diag [5];
    uint8_t _Mif;
    uint8_t _CRC16 [4];
    uint8_t _BracketEnd;
//
} __attribute__((packed)) TTUMSIn;

typedef struct
{
    TTUMSIn _TUMSMessage;
    uint8_t _ps;
    uint8_t _vk;
//
} __attribute__((packed)) TTUMSInPacket;

typedef struct 
{
    uint8_t _BracketStart;
    uint8_t _StationID [6];
    uint8_t _MsgNum [4];
    uint8_t _DataHead;
    uint8_t _TUMSNum;
    uint8_t _DataGroup;
    uint8_t _DataSubGroup;
    uint8_t _CRC16 [4];
    uint8_t _BracketEnd;
//    
} __attribute__((packed)) TTUMSOut;

typedef struct 
{
    TTUMSOut _ARMMessage;
    uint8_t  _ps;
    uint8_t  _vk;
//
} __attribute__((packed)) TTUMSOutPacket;

// struct for connection with TUMS_n with using serial port
// - in global namespace
// - init in routine "main"
// - using in pthread routine
typedef struct
{
public:
    uint32_t      id;       // for output console stream
    std::string   devName;  // device name
    SerialPort_r  sp_read;
    SerialPort_w  sp_write;
    CircleBufferHelper<uint8_t> dataBuffer;

    bool          msgStart;
    std::vector<uint8_t> msgBuffer;

    TTUMSIn*      msg_in;
    TTUMSOut      msg_out; 
//    
} TSerial;

const uint32_t TUMS_MAX       = 8; // index == 0 ignored !!!
const uint32_t TUMS_FIRST_IDX = 1;
//const 

//namespace nsTUMS
//{
//    int count = 0;
//    //TSerial TUMS_conn [TUMS_MAX+1]; // index 0 - none use
//};

#include "stan2l_ipc.hpp"

////////
// main
int main (int argc, char** argv)
{
//var
    time_t t_t;
    struct tm t_tm;  
    struct timespec t_ts;

    // for queue
    key_t msgque_tumsts_key;
    int   msgque_tumsts_id;
    TTUMS_IPC_TS tumsts_ipcmsg;
    int   msgrcv_res;

//code
    std::cout << "Start ipc stan module (port for linux) ... " << std::endl;
    std::cout << "Version 0.0.xxxx (20210415.xx:xx:xx)!" << std::endl;

    // create queue for send TUMS TS
    msgque_tumsts_key = ftok ("/work/dev/stan2l", 11111);
    std::cout <<  "msgkey_tumsts = " << std::hex << msgque_tumsts_key << std::endl;        
    if (msgque_tumsts_key == -1)
    {
        std::cout <<  "Error init IPC queue (ftok (...) return -1)" << std::endl;
        return -2;
    }
    else;

    // get id messsage queue
    msgque_tumsts_id = msgget (msgque_tumsts_key, IPC_CREAT | 0666);
    std::cout << "msgque_tumsts_id = " << std::dec << msgque_tumsts_id << std::endl;        
    if (msgque_tumsts_id == -1)
    {
        std::cout << "Error create IPC(1) queue (msgget (...) return -1 (errno = " << errno << "))" << std::endl;
        return -2;
    }
    else;

    while (true)
    {
        // no wait message
        msgrcv_res = msgrcv (msgque_tumsts_id, 
                             (void*) &tumsts_ipcmsg, 
                             sizeof (TTUMS_IPC_TS) - sizeof (long), 
                             0, 
                             IPC_NOWAIT);

        // Process error
        if (msgrcv_res == -1)
        {
            if (errno == ENOMSG) // No data
            {
                std::cout << "." << std::flush;
            }
            else
            {
                std::cout << std::endl << "msgrcv (...) return : " << msgrcv_res << " (errno = " << errno << ")";
                        
                msgque_tumsts_id = msgget (msgque_tumsts_key, IPC_CREAT | 0666);
                std::cout << std::endl << "msgque_tumsts_id = " << std::dec << msgque_tumsts_id;
                if (msgque_tumsts_id == -1)
                {
                    std::cout << "Error create(2) IPC queue (msgget (...) return -1 (errno = " << errno << "))" << std::endl;
                    return -2;
                }
                else;
            }
            usleep (50000);
            continue;
        }
        else;

        // time is received message 
        t_t = time (NULL);
        localtime_r (&t_t, &t_tm);
        clock_gettime (CLOCK_REALTIME, &t_ts);
        std::cout << std::endl
                  << "\033[1;34m" << '['
                  << std::dec << std::setfill ('0') << std::setw (4) << t_tm.tm_year+1900 << '.'
                  << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_mon+1 << '.'
                  << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_mday << '('
                  << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_hour << ':'
                  << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_min << ':'
                  << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_sec << '_'
                  << std::dec << std::setfill ('0') << std::setw (3) << (uint32_t) t_ts.tv_nsec / 1000000
                  << ")]" << "\033[0m";
        
        // Check CRC16 very important
        // Last 

        // Process message
        switch (tumsts_ipcmsg.ipc_std_mtype)
        {
            case IPC_TUMS_TS: 
            {
                std::cout << "\033[1;33m" << /*std::setw (20) <<*/ (char*) &tumsts_ipcmsg._BracketStart << "\033[0m"; 
                std::cout << std::hex << std::uppercase << "(IPC_TUMS_TS:" << tumsts_ipcmsg._PacketNumber << ") ";              
            }
            break;
            default:
            {
                std::cout << "id = " << tumsts_ipcmsg.ipc_std_mtype;
            }
            break;
        }
        std::cout << std::flush;
    } // while (1) { ... }
}
