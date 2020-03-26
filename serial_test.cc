//comment
//  HELP for brain
//       https://ru.stackoverflow.com/questions/376364/%D0%A7%D1%82%D0%BE-%D0%B4%D0%B5%D0%BB%D0%B0%D0%B5%D1%82-select-%D0%B8-fd-isset

// system include
#include <stdio.h>
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

// dev include
#include "source/crc/crc16.h"
#include "source/cfg/prgmcfg.h"

typedef struct 
{
    int     handle;
    struct  termios tio_old; 
    struct  termios tio;
    fd_set  fds_reads;
    timeval tv_reads; 
    bool bmsgStart;
    std::vector<uint8_t> msgBuffer;
    std::vector<uint8_t>::iterator BufferIterator;
//    
} TSerial;

const uint32_t TUMS_MAX = 8; // index == 0 ignored !!!
const uint32_t TUMS_FIRST_IDX = 1;
namespace nsTUMS
{
    int count = 0;
    TSerial TUMS_conn [TUMS_MAX+1];
};

int main (int argc, char** argv)
{
//var
   // configuration

   // serial connection
   int iSysRoutineRes = -1;
   TSerial& tums1_conn_ref = nsTUMS::TUMS_conn [TUMS_FIRST_IDX];

   // for read
   char read_c;
   unsigned int read_size;
   uint16_t uiCrc16Calc,
            uiCrc16Recv;

   // for write

//code
    printf ("Start stan module (port for linux) ... \n");
    printf ("Version 0.0.xxxx (20210111.xx:xx:xx)!\n");

    // get configuration
    ProgramConfig tums_cfg (".//tums_link.conf", "");
    nsTUMS::count = tums_cfg.GetIntParam ("TUMS.COUNT",0);
    printf ("TUMSs : %d\n", nsTUMS::count);
    if (nsTUMS::count <= 0)
    {
        printf ("No any TUMS's. Check \"tums_link.conf\". Exit (sorry)!\n");
        return -1;
    }
    else;
    if (nsTUMS::count > TUMS_MAX)
    {
        printf ("Any more TUMS's (max 16). Check \"tums_link.conf\". Exit (sorry)!\n");
        return -1;
    }
    else;


next_iteration:

   printf ("Open serial port : %s\n", tums_cfg.GetStringParam ("TUMS.1.LINK.SERIAL.DEVICE", "null").c_str());

   // Read/Write, 
   // No_TTY, 
   // No_DCD_check - none use
   iSysRoutineRes = open (tums_cfg.GetStringParam ("TUMS.1.LINK.SERIAL.DEVICE", "null").c_str(), 
                          O_RDWR | O_NOCTTY); // | O_NDELAY);  
   if (iSysRoutineRes == -1 )
   {
        printf ("Error open SerialPort: %s. Error code (%d)\n", 
                tums_cfg.GetStringParam ("TUMS.1.LINK.SERIAL.DEVICE", "null").c_str(),
                errno);
        return errno;
   }
   else;
   printf ("open return (as handle) : %d (errno : %d)\n", iSysRoutineRes, errno);

   // Save serial port handle
   //iSerialDev = iSysRoutineRes;
   //tums_conn_1 = &nsTUMS::TUMS_conn [TUMS_FIRST_IDX];
   //assert (tums_conn_1);
   tums1_conn_ref.handle = iSysRoutineRes;

   // Set serial port control flags
   iSysRoutineRes = tcgetattr (tums1_conn_ref.handle, 
                               &tums1_conn_ref.tio_old); // get old 
   printf ("tcgetattr return : %d (errno : %d)\n", iSysRoutineRes, errno);

   // new serial setting
   bzero (&tums1_conn_ref.tio, sizeof (tums1_conn_ref.tio));
   iSysRoutineRes = cfsetispeed (&tums1_conn_ref.tio, B2400);   // speed
   printf ("cfsetispeed return : %d (errno : %d)\n", iSysRoutineRes, errno);
   iSysRoutineRes = cfsetospeed (&tums1_conn_ref.tio, B2400);   
   printf ("cfsetospeed return : %d (errno : %d)\n", iSysRoutineRes, errno);

   //tums1_conn_ref.tio.c_cflag != BAUDRATE;     
   tums1_conn_ref.tio.c_cflag |= (CLOCAL | CREAD);
   // 8N1
   //tums1_conn_ref.tio.c_cflag &= ~CSIZE;     // size
   tums1_conn_ref.tio.c_cflag |= CS8;
   tums1_conn_ref.tio.c_cflag &= ~PARENB;      // parity
   tums1_conn_ref.tio.c_cflag &= ~CSTOPB;        // 1_stop - for example
   //tums1_conn_ref.tio.c_cflag |= CSTOPB;       // 2_stop
   // Disable flow control
   tums1_conn_ref.tio.c_cflag &= ~CRTSCTS;
   
   //tums1_conn_ref.tio.c_cflag &= ~(IXON | IXOFF | IXANY);
   //tums1_conn_ref.tio.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   tums1_conn_ref.tio.c_lflag = 0;
   tums1_conn_ref.tio.c_oflag = 0;
   tums1_conn_ref.tio.c_iflag = IGNPAR;         // ignore parity error

   tums1_conn_ref.tio.c_cc [VTIME] = 0;
   tums1_conn_ref.tio.c_cc [VMIN]  = 1;

   iSysRoutineRes = tcsetattr (tums1_conn_ref.handle, 
                               TCSANOW, 
                               &tums1_conn_ref.tio);
   printf ("tcsetattr return : %d (errno : %d)\n", iSysRoutineRes, errno);

   // call for none-blocking read
   //fcntl (fd, F_SETFL, FNDELAY);
   tums1_conn_ref.bmsgStart = false;
   while (true)
   {
        FD_ZERO (&tums1_conn_ref.fds_reads);       
        FD_SET  (tums1_conn_ref.handle, 
                &tums1_conn_ref.fds_reads);
        tums1_conn_ref.tv_reads.tv_sec  = 2;
        tums1_conn_ref.tv_reads.tv_usec = 0;
        iSysRoutineRes = select (1+tums1_conn_ref.handle, 
                                 &tums1_conn_ref.fds_reads, 
                                 0, 
                                 0, 
                                 &tums1_conn_ref.tv_reads);

        if (iSysRoutineRes > 0) 
        {
            if (FD_ISSET (tums1_conn_ref.handle, 
                          &tums1_conn_ref.fds_reads));
            else
            {
                // IT IS VARY BAD!!! - while break. Need for test
                break;
            }
        }
        else
        {
            printf ("\nselect return : %d (errno : %d)", iSysRoutineRes, errno); fflush (stdout);
            if (iSysRoutineRes == 0) continue; // Timeout
            else
            if (iSysRoutineRes < 0)  break;    // Error
            else;
        }

        // Read data
        read_size = 1;
        iSysRoutineRes = read (tums1_conn_ref.handle, 
                               &read_c, 
                               read_size);
        if (iSysRoutineRes > 0) // read ok!
        {
            printf ("%c", read_c);
            if (tums1_conn_ref.bmsgStart == false) // ищем начало сообщения
            {
               if (read_c =='(') // Здесь ошибка - если встречается Q(@@@@@ - сбой в логике программы
               {
                  tums1_conn_ref.bmsgStart = true;
                  tums1_conn_ref.msgBuffer.push_back (static_cast<uint8_t>(read_c));
               }
               else; 
            }
            else
            {
                tums1_conn_ref.msgBuffer.push_back (static_cast<uint8_t>(read_c));
                if (read_c == ')')
                {
                    if (tums1_conn_ref.msgBuffer.size () == 32)
                    {
                        printf (" <+NEW_MSG>");
                        
                        // Run crc16 routine
                        //BufferIterator = msgBuffer.begin ();
                        //BufferIterator++;
                        uiCrc16Calc = CalculateCRC16 (&tums1_conn_ref.msgBuffer [1], 26);
                        printf (" <CRC16 = 0x%04X>", uiCrc16Calc);
                        uiCrc16Recv = 0x0000;
                        uiCrc16Recv  =  RoutineCRC16Char2i16 (tums1_conn_ref.msgBuffer [30]);
                        uiCrc16Recv |= (RoutineCRC16Char2i16 (tums1_conn_ref.msgBuffer [29]) << 4);
                        uiCrc16Recv |= (RoutineCRC16Char2i16 (tums1_conn_ref.msgBuffer [28]) << 8);
                        uiCrc16Recv |= (RoutineCRC16Char2i16 (tums1_conn_ref.msgBuffer [27]) << 12);
                        if (uiCrc16Calc == uiCrc16Recv)
                            printf (" <+CRC16>");
                        else;
                    }
                    else
                    {
                        printf (" <-BAD_MSG_LEN (%zu)>", tums1_conn_ref.msgBuffer.size ());
                    }
                    tums1_conn_ref.bmsgStart = false;
                    tums1_conn_ref.msgBuffer.clear ();
                }
                else;
            } 
            fflush (stdout);
        }
        else
        if (iSysRoutineRes == -1) // timeout or error!
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                 printf ("<TIME_OUT>");  fflush (stdout);
                 usleep (10000);
                 continue;
            }
            else
            {  
                 printf ("read return : %d (errno : %d)\n", iSysRoutineRes, errno);
                 break;
            }
        }
        else
        {
            printf ("read return : %d (errno : %d)\n", iSysRoutineRes, errno);
            break;
        }
   }

   close (tums1_conn_ref.handle);
   goto next_iteration; // XA-XA-XA
}
