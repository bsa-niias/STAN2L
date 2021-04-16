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

// ipc
#include "stan2l_ipc.hpp"

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

namespace nsTUMS
{
    int count = 0;
    TSerial TUMS_conn [TUMS_MAX+1]; // index 0 - none use
};

////////
// thread_serial_work
static void* thread_tums_serial (void* thread_param)
{
//type's
// exception for serial port thread only
class sp_exception
{
public:
    sp_exception (uint32_t _e = -1) : error (_e) {}
    uint32_t getError () { return error; }
private:
    uint32_t error;    
};

//var
    TSerial* conn = NULL;
    int SysRoutineRes = -1;
    char read_c; 

//thread_code
    if (thread_param == NULL) 
    {
        std::cerr << ConsoleLock 
                  << "\033[1;31m" << "\"thread_param\" invalid. Exit." << "\033[0m" << std::endl 
                  << ConsoleUnlock;
        pthread_exit (reinterpret_cast<void*>(0xE0000001));
    }
    else;

    // global memory from "main" routine
    conn = (TSerial*)thread_param;
    std::cout << ConsoleLock 
              << "\"thread_id\"=" << conn->id << " start ..." << std::endl 
              << ConsoleUnlock;

    try
    {
        while (true)
        {
            SysRoutineRes = conn->sp_read.wait (); // "wait" use FD_ISSET
        
            // this "SysRoutineRes" after "wait" routine
            if (SysRoutineRes > 0); // can read data
            else
            {
                std::cerr << ConsoleLock 
                          << "\033[1;31m" << "select return:" << SysRoutineRes 
                          << "(errno:" << errno << ")" << "\033[0m" << std::endl
                          << ConsoleUnlock;
                if (SysRoutineRes == 0) continue; // Timeout == 2 sec. Sleep not need
                else;

                throw sp_exception (0xE0000002); // Error (SysRoutineRes < 0) -> reconection need!
            }
            
            // read data
            SysRoutineRes = conn->sp_read.Read_1b (&read_c);

            // this "SysRoutineRes" after "Read_1b" routine
            if (SysRoutineRes > 0) // read ok!
            {
                conn->dataBuffer.put (read_c);
                continue;
            }
            else
            if (SysRoutineRes == -1) // timeout or error!
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    std::cerr << ConsoleLock 
                              << "\033[1;31m" << "<TIME_OUT>" << "\033[0m" << std::fflush 
                              << ConsoleUnlock;            
                    usleep (1000);
                    continue;
                }
                else;
            }
            else;

            std::cerr << ConsoleLock 
                      << "\033[1;31m" << "read return:" << SysRoutineRes 
                      << "(errno:" << errno << ")" << "\033[0m" << std::endl
                      << ConsoleUnlock;
            throw sp_exception (0xE0000003); // Error read routine (<=0) -> reconection need!
            //
        } // end while (...)
    }
    catch (sp_exception& spe)
    {
        std::cerr << ConsoleLock 
                  << "\033[1;31m" << "sp_exception get:" << std::hex << spe.getError () 
                  << "(errno:" << errno << ")" << "\033[0m" << std::endl
                  << ConsoleUnlock;
        pthread_exit (reinterpret_cast<void*>(spe.getError ()));
    }

    pthread_exit (reinterpret_cast<void*>(0x00000000));
}

////////
// main
int main (int argc, char** argv)
{
//var
    time_t t_t;
    struct tm t_tm;  
    struct timespec t_ts;

    // configuration
    char strCFGParam [nsCFG::_BUFF_MAX+1];

    // serial connection
    int iSysRoutineRes = -1;
    TSerial& tums1_conn_ref = nsTUMS::TUMS_conn [TUMS_FIRST_IDX];

    // for read
    //char read_c;
    char d;
    unsigned int read_size;
    uint16_t uiCrc16Calc,
             uiCrc16Recv;
   
    // for write
    char _ps = 10;
    char _vk = 13;

    // for queue
    key_t msgque_tumsts_key;
    int   msgque_tumsts_id;
    TTUMS_IPC_TS tumsts_ipcmsg;
    int msgctl_res;
    int msgsnd_res;
    msqid_ds msgque_tumsts_ctl;
    const uint32_t MSGQUE_TUMSTS_SIZE = 250;

//code
    std::cout << "Start stan module (port for linux) ... " << std::endl;
    std::cout << "Version 0.0.xxxx (20210202.xx:xx:xx)!" << std::endl;

    // get configuration
    std::cout << "Configuration ... " << std::endl;
    ProgramConfig tums_cfg (".//tums_link.conf", "");
    nsTUMS::count = tums_cfg.GetIntParam ("TUMS.COUNT",0);
    std::cout << "TUMS's:" << nsTUMS::count << std::endl;
    if (nsTUMS::count <= 0)
    {
        std::cout <<  "No any TUMS's. Check \"tums_link.conf\". Exit (sorry)!\n" << std::endl;
        return -1;
    }
    else;
    if (nsTUMS::count > TUMS_MAX)
    {
        std::cout << "Any more TUMS's (max 16). Check \"tums_link.conf\". Exit (sorry)!\n"  << std::endl;
        return -1;
    }
    else;

    // get configuration
    for (uint32_t tums_iterator = 1; tums_iterator <= nsTUMS::count; tums_iterator++)
    {
        nsTUMS::TUMS_conn [tums_iterator].id = tums_iterator;

        snprintf (strCFGParam, 
                  nsCFG::_BUFF_MAX, 
                  "TUMS.%d.LINK.SERIAL.DEVICE", 
                  tums_iterator);
        nsTUMS::TUMS_conn [tums_iterator].devName = tums_cfg.GetStringParam (strCFGParam, "null");
    }

    // print configuration
    for (uint32_t tums_iterator = 1; tums_iterator <= nsTUMS::count; tums_iterator++)
    {
        std::cout << "TUMS : " << tums_iterator << std::endl;
        std::cout << "   internal_id:" << nsTUMS::TUMS_conn [tums_iterator].id << std::endl;
        std::cout << "   device_name:" << nsTUMS::TUMS_conn [tums_iterator].devName << std::endl;
    }

    tums1_conn_ref.sp_read.setDeviceName (tums1_conn_ref.devName.c_str());
    tums1_conn_ref.sp_read.Open ();
    tums1_conn_ref.sp_read.setConnectionAttr (B2400, CS8, ~PARENB, ~CSTOPB);
    tums1_conn_ref.sp_read.setTimeout (1, 0);

    tums1_conn_ref.sp_write.setDeviceName (tums1_conn_ref.devName.c_str());
    tums1_conn_ref.sp_write.Open ();
    tums1_conn_ref.sp_write.setConnectionAttr (B2400, CS8, ~PARENB, ~CSTOPB);
    tums1_conn_ref.sp_write.setTimeout (1, 0);
   
    // create queue for send TUMS TS
    // 1) get key message queue
    msgque_tumsts_key = ftok ("/work/dev/stan2l", 11111);
    std::cout << "msgque_tumsts_key = " << std::hex << msgque_tumsts_key << std::endl;        
    if (msgque_tumsts_key == -1)
    {
        std::cout << "Error get key IPC queue (ftok (...) return -1 (errno = " << errno << "))" << std::endl;
        return -2;
    }
    else;
    // 2) get or create message queue
    msgque_tumsts_id = msgget (msgque_tumsts_key, IPC_CREAT | 0666);
    std::cout << "msgque_tumsts_id = " << std::hex << msgque_tumsts_id << std::endl;        
    if (msgque_tumsts_id == -1)
    {
        std::cout << "Error create(1) IPC queue (msgget (...) return -1 (errno = " << errno << "))" << std::endl;
        return -2;
    }
    else;
    // 3) need delete message queue - delete old data
    msgctl_res = msgctl (msgque_tumsts_id, IPC_RMID, NULL);
    std::cout << "msgctl[1] (...) return : " << msgctl_res << " (errno = " << errno << ")" << std::endl;  
    // Need check error      
    // 4) renew messsage queue
    msgque_tumsts_id = msgget (msgque_tumsts_key, IPC_CREAT | 0666);
    std::cout << "msgque_tumsts_id = " << std::hex << msgque_tumsts_id << std::endl;        
    if (msgque_tumsts_id == -1)
    {
        std::cout << "Error create(2) IPC queue (msgget (...) return -1 (errno = " << errno << "))" << std::endl;
        return -2;
    }
    else;
    // 5) get queue size
    msgctl_res = msgctl (msgque_tumsts_id, IPC_STAT, &msgque_tumsts_ctl);
    std::cout << "msgctl[2] (...) return : " << msgctl_res << " (errno = " << errno << ")" << std::endl;  
    // 6) set queue size
    msgque_tumsts_ctl.msg_qbytes = MSGQUE_TUMSTS_SIZE;
    msgctl_res = msgctl (msgque_tumsts_id, IPC_SET, &msgque_tumsts_ctl);
    std::cout << "msgctl[3] (...) return : " << msgctl_res << " (errno = " << errno << ")" << std::endl;  
    // 7) it's ok!

    // run thread read data from serial_port
    pthread_t ptht;
    pthread_create (&ptht, NULL, thread_tums_serial, (void*)&nsTUMS::TUMS_conn [1]);

    // -----------
    // MAIN CIRCLE
    while (true)
    {
        // check buffer
        if (tums1_conn_ref.dataBuffer.empty ()) 
        {
            usleep (10000); 
            continue;
        }
        else;

        // get symbol
        d = tums1_conn_ref.dataBuffer.get ();

        // check start message
        if (tums1_conn_ref.msgStart == false) // ищем начало сообщения
        {
            // Здесь ошибка - если встречается Q(@@@@@ - сбой в логике программы до ')'
            if (d =='(') 
            {
                tums1_conn_ref.msgStart = true;
                tums1_conn_ref.msgBuffer.push_back (static_cast<uint8_t>(d));
            }
            else
            {
                std::cout << "\033[1;31m" << (char) d << "\033[0m"; // It's so #10#13 effect!!!
            } 
        }
        else
        {
            // analize message
            tums1_conn_ref.msgBuffer.push_back (static_cast<uint8_t>(d));
            if (d == ')')
            {
                if (tums1_conn_ref.msgBuffer.size () == sizeof (TTUMSIn)) // 33
                {
                    // time is received message 
                    t_t = time (NULL);
                    localtime_r (&t_t, &t_tm);
                    clock_gettime (CLOCK_REALTIME, &t_ts);
                    std::cout << "\033[1;34m" << '['
                              << std::dec << std::setfill ('0') << std::setw (4) << t_tm.tm_year+1900 << '.'
                              << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_mon+1 << '.'
                              << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_mday << '('
                              << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_hour << ':'
                              << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_min << ':'
                              << std::dec << std::setfill ('0') << std::setw (2) << t_tm.tm_sec << '_'
                              << std::dec << std::setfill ('0') << std::setw (3) << (uint32_t) t_ts.tv_nsec / 1000000
                              << ")]" << "\033[0m";

                    tums1_conn_ref.msgBuffer.push_back (0x00); // only for print to screen !!!
                    std::cout << "\033[1;33m" << (char*) &tums1_conn_ref.msgBuffer [0] << "\033[0m";

                    tums1_conn_ref.msg_in = (TTUMSIn*) &tums1_conn_ref.msgBuffer [0];
                    // Run crc16 routine
                    // Skeep '(', ')', CRC16 field
                    uiCrc16Calc = CalculateCRC16 (&tums1_conn_ref.msg_in->_StationID [0], sizeof (TTUMSIn)-1-4-1); //27
                    uiCrc16Recv = 0x0000;
                    uiCrc16Recv  =  RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_CRC16 [3]);
                    uiCrc16Recv |= (RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_CRC16 [2]) << 4);
                    uiCrc16Recv |= (RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_CRC16 [1]) << 8);
                    uiCrc16Recv |= (RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_CRC16 [0]) << 12);
                    if (uiCrc16Calc != uiCrc16Recv)
                    {
                        std::cout << "\033[1;31m" << "-CRC16(0x" << std::hex << uiCrc16Calc << ")>" << "'\033[0m";
                    }
                    else 
                    { 
                        std::cout << "\033[1;32m<+CRC16>\033[0m";

                        // send answer
                        bzero (&tums1_conn_ref.msg_out, sizeof (TTUMSOut));
                        tums1_conn_ref.msg_out._BracketStart  = '(';
                        tums1_conn_ref.msg_out._StationID [0] = tums1_conn_ref.msg_in->_StationID [0];
                        tums1_conn_ref.msg_out._StationID [1] = tums1_conn_ref.msg_in->_StationID [1];
                        tums1_conn_ref.msg_out._StationID [2] = tums1_conn_ref.msg_in->_StationID [2];
                        tums1_conn_ref.msg_out._StationID [3] = tums1_conn_ref.msg_in->_StationID [3];
                        tums1_conn_ref.msg_out._StationID [4] = tums1_conn_ref.msg_in->_StationID [4];
                        tums1_conn_ref.msg_out._StationID [5] = tums1_conn_ref.msg_in->_StationID [5];
                        tums1_conn_ref.msg_out._MsgNum [0]    = tums1_conn_ref.msg_in->_MsgNum [0];
                        tums1_conn_ref.msg_out._MsgNum [1]    = tums1_conn_ref.msg_in->_MsgNum [1];
                        tums1_conn_ref.msg_out._MsgNum [2]    = tums1_conn_ref.msg_in->_MsgNum [2];
                        tums1_conn_ref.msg_out._MsgNum [3]    = tums1_conn_ref.msg_in->_MsgNum [3];
                        tums1_conn_ref.msg_out._DataHead      = 'A'; //msg_in->_DataHead;
                        tums1_conn_ref.msg_out._TUMSNum       = tums1_conn_ref.msg_in->_TUMSNum;
                        tums1_conn_ref.msg_out._DataGroup     = tums1_conn_ref.msg_in->_DataGroup;
                        tums1_conn_ref.msg_out._DataSubGroup  = tums1_conn_ref.msg_in->_DataSubGroup;
                        uiCrc16Calc = CalculateCRC16 (&tums1_conn_ref.msg_out._StationID [0], 14); //sizeof (TTUMSOut)-2-4); //13
                        tums1_conn_ref.msg_out._CRC16 [3]     =  RoutineCRC16ui162Char ( uiCrc16Calc & 0x000F);
                        tums1_conn_ref.msg_out._CRC16 [2]     =  RoutineCRC16ui162Char ((uiCrc16Calc & 0x00F0) >> 4);
                        tums1_conn_ref.msg_out._CRC16 [1]     =  RoutineCRC16ui162Char ((uiCrc16Calc & 0x0F00) >> 8);
                        tums1_conn_ref.msg_out._CRC16 [0]     =  RoutineCRC16ui162Char ((uiCrc16Calc & 0xF000) >> 12);
                        tums1_conn_ref.msg_out._BracketEnd    = ')';
                        tums1_conn_ref.sp_write.Write ((char*) &tums1_conn_ref.msg_out, sizeof (TTUMSOut));
                        tums1_conn_ref.sp_write.Write (&_ps, 1);
                        tums1_conn_ref.sp_write.Write (&_vk, 1);

                        std::cout << "\033[1;35m" << std::setw (20) << (char*) &tums1_conn_ref.msg_out << "\033[0m"; // %.20s

                        // send ipc message
                        tumsts_ipcmsg.ipc_std_mtype  = IPC_TUMS_TS;
                        //
                        tumsts_ipcmsg._PacketNumber  = 0x0000;
                        tumsts_ipcmsg._PacketNumber |=  RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_MsgNum [3]);
                        tumsts_ipcmsg._PacketNumber |=  RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_MsgNum [2]) << 4;
                        tumsts_ipcmsg._PacketNumber |=  RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_MsgNum [1]) << 8;
                        tumsts_ipcmsg._PacketNumber |=  RoutineCRC16Char2ui16 (tums1_conn_ref.msg_in->_MsgNum [0]) << 12;
                        //
                        tumsts_ipcmsg._BracketStart  = tums1_conn_ref.msg_in->_BracketStart;
                        tumsts_ipcmsg._DataHead      = tums1_conn_ref.msg_in->_DataHead;
                        tumsts_ipcmsg._TUMSNum       = tums1_conn_ref.msg_in->_TUMSNum;
                        tumsts_ipcmsg._DataGroup     = tums1_conn_ref.msg_in->_DataGroup;
                        tumsts_ipcmsg._DataSubGroup  = tums1_conn_ref.msg_in->_DataSubGroup;
                        tumsts_ipcmsg._Data [0]      = tums1_conn_ref.msg_in->_Data [0];
                        tumsts_ipcmsg._Data [1]      = tums1_conn_ref.msg_in->_Data [1];
                        tumsts_ipcmsg._Data [2]      = tums1_conn_ref.msg_in->_Data [2];
                        tumsts_ipcmsg._Data [3]      = tums1_conn_ref.msg_in->_Data [3];
                        tumsts_ipcmsg._Data [4]      = tums1_conn_ref.msg_in->_Data [4];
                        tumsts_ipcmsg._ChannelStatus = tums1_conn_ref.msg_in->_ChannelStatus;
                        tumsts_ipcmsg._DiagType      = tums1_conn_ref.msg_in->_DiagType;
                        tumsts_ipcmsg._Diag [0]      = tums1_conn_ref.msg_in->_Diag [0];
                        tumsts_ipcmsg._Diag [1]      = tums1_conn_ref.msg_in->_Diag [1];
                        tumsts_ipcmsg._Diag [2]      = tums1_conn_ref.msg_in->_Diag [2];
                        tumsts_ipcmsg._Diag [3]      = tums1_conn_ref.msg_in->_Diag [3];
                        tumsts_ipcmsg._Diag [4]      = tums1_conn_ref.msg_in->_Diag [4];
                        tumsts_ipcmsg._Mif           = tums1_conn_ref.msg_in->_Mif; 
                        tumsts_ipcmsg._BracketEnd    = tums1_conn_ref.msg_in->_BracketEnd;
                        tumsts_ipcmsg._crc16         = 0x0000;
                        msgsnd_res = msgsnd (msgque_tumsts_id, 
                                             &tumsts_ipcmsg.ipc_std_mtype, 
                                             sizeof (TTUMS_IPC_TS) - sizeof (long),
                                             IPC_NOWAIT);
                        
                        std::cout << " ipc_msgsnd (IPC_TUMS_TS:" << std::hex << std::uppercase << "0x" << tumsts_ipcmsg._PacketNumber << ")";
                        if (msgsnd_res != 0)
                        {
                            std::cout << std::endl << std::dec << "msgsnd (...) return : " << msgsnd_res << " (errno = " << errno << ")";
                            msgctl_res = msgctl (msgque_tumsts_id, IPC_RMID, NULL);
                            std::cout << std::endl << std::dec << "msgctl[4] (...) return : " << msgctl_res << " (errno = " << errno << ")";
                            msgque_tumsts_id = msgget (msgque_tumsts_key, IPC_CREAT | 0666);
                            std::cout << std::endl << std::dec << "msgque_tumsts_id = " << msgque_tumsts_id;
                            msgctl_res = msgctl (msgque_tumsts_id, IPC_STAT, &msgque_tumsts_ctl);
                            std::cout << std::endl << "msgctl[5] (...) return : " << msgctl_res << " (errno = " << errno << ")";  
                            msgque_tumsts_ctl.msg_qbytes = MSGQUE_TUMSTS_SIZE;
                            msgctl_res = msgctl (msgque_tumsts_id, IPC_SET, &msgque_tumsts_ctl);
                            std::cout << std::endl << "msgctl[6] (...) return : " << msgctl_res << " (errno = " << errno << ")";  
                        }
                        else;
                    }
                }
                else
                {
                    std::cout << "\033[1;31m"  
                              << "<BAD_MSG_LEN (" << tums1_conn_ref.msgBuffer.size ()  << ")"
                              << "\033[0m";// << std::endl;
                    tums1_conn_ref.msgBuffer.push_back (0x00); // only for print to screen !!!
                    std::cout << "\033[1;31m" <<  (char*) &tums1_conn_ref.msgBuffer [0] << "\033[0m";// << std::endl;
                }
                tums1_conn_ref.msgStart = false;
                tums1_conn_ref.msgBuffer.clear ();
            }
            else
            {
                if (tums1_conn_ref.msgBuffer.size () > sizeof (TTUMSIn))
                {
                    std::cout << "\033[1;31m"  
                              << "<BUFFER_OVERFLOW_DATA_IS_RESET (LEN=" << tums1_conn_ref.msgBuffer.size () << ")>" //%zu
                              << "\033[0m";// << std::endl;
                    tums1_conn_ref.msgBuffer.push_back (0x00); // only for print to screen !!!
                    std::cout << "\033[1;31m" <<  (char*) &tums1_conn_ref.msgBuffer [0] << "\033[0m";// << std::endl;

                    tums1_conn_ref.msgStart = false;
                    tums1_conn_ref.msgBuffer.clear ();
                }
                else;
            }
        } 
        fflush (stdout);
        //
    } // while (1) { ... }
}
