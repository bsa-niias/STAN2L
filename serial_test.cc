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

// dev include
#include "source/crc/crc16.h"
#include "source/cfg/prgmcfg.h"

int main (int argc, char** argv)
{
//var
   int iSysRoutineRes = -1;
   int iSerialDev     = -1;
   struct termios tSer_old, 
                  tSer_new;
   // for timeouts
   fd_set fds_reads;
   timeval tv_reads; 
   // for read
   char read_c;
   unsigned int read_size;
   uint16_t uiCrc16Calc,
            uiCrc16Recv;

   bool bmsgStart;
   std::vector<uint8_t> msgBuffer;
   std::vector<uint8_t>::iterator BufferIterator;

   // for write

//code
   
     ProgramConfig tums_cfg (".//tums_link.conf", "");
     int tums_count = tums_cfg.GetIntParam ("TUMS.COUNT",0);
     printf ("TUMSs : %d\n", tums_count);

next_iteration:

   printf ("Hello, serial test!\n");
   printf ("Version 0.0.002 (20201231.130000)!\n");
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
   iSerialDev = iSysRoutineRes;

   // Set serial port control flags
   iSysRoutineRes = tcgetattr (iSerialDev, &tSer_old); // get old 
   printf ("tcgetattr return : %d (errno : %d)\n", iSysRoutineRes, errno);

   // new serial setting
   bzero (&tSer_new, sizeof (tSer_new));
   iSysRoutineRes = cfsetispeed (&tSer_new, B2400);   // speed
   printf ("cfsetispeed return : %d (errno : %d)\n", iSysRoutineRes, errno);
   iSysRoutineRes = cfsetospeed (&tSer_new, B2400);   
   printf ("cfsetospeed return : %d (errno : %d)\n", iSysRoutineRes, errno);

   //tSer_new.c_cflag != BAUDRATE;     
   tSer_new.c_cflag |= (CLOCAL | CREAD);
   // 8N1
   //tSer_new.c_cflag &= ~CSIZE;     // size
   tSer_new.c_cflag |= CS8;
   tSer_new.c_cflag &= ~PARENB;      // parity
   tSer_new.c_cflag &= ~CSTOPB;        // 1_stop - for example
   //tSer_new.c_cflag |= CSTOPB;       // 2_stop
   // Disable flow control
   tSer_new.c_cflag &= ~CRTSCTS;
   
   //tSer.c_cflag &= ~(IXON | IXOFF | IXANY);
   //tSer.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG);

   tSer_new.c_lflag = 0;
   tSer_new.c_oflag = 0;
   tSer_new.c_iflag = IGNPAR;         // ignore parity error

   tSer_new.c_cc [VTIME] = 0;
   tSer_new.c_cc [VMIN]  = 1;

   iSysRoutineRes = tcsetattr (iSerialDev, TCSANOW, &tSer_new);
   printf ("tcsetattr return : %d (errno : %d)\n", iSysRoutineRes, errno);

   // call for none-blocking read
   //fcntl (fd, F_SETFL, FNDELAY);
   bmsgStart = false;
   while (true)
   {
        //iSysRoutineRes = write (iSerialDev, "(ABCD)", 6);
        //printf ("write return : %d (errno : %d)\n", iSysRoutineRes, errno);
        //usleep (250000);

        FD_ZERO (&fds_reads);       
        FD_SET (iSerialDev, &fds_reads);
        tv_reads.tv_sec  = 2;
        tv_reads.tv_usec = 0;
        iSysRoutineRes = select (1+iSerialDev, &fds_reads, 0, 0, &tv_reads);

        if (iSysRoutineRes > 0) 
        {
            if (FD_ISSET (iSerialDev, &fds_reads));
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
        iSysRoutineRes = read (iSerialDev, &read_c, read_size);
        if (iSysRoutineRes > 0) // read ok!
        {
            printf ("%c", read_c);
            if (bmsgStart == false) // ищем начало сообщения
            {
               if (read_c =='(')
               {
                  bmsgStart = true;
                  msgBuffer.push_back (static_cast<uint8_t>(read_c));
               }
               else; 
            }
            else
            {
                msgBuffer.push_back (static_cast<uint8_t>(read_c));
                if (read_c == ')')
                {
                    if (msgBuffer.size () == 25)
                    {
                        printf (" <+NEW_MSG>");
                        
                        // Run crc16 routine
                        //BufferIterator = msgBuffer.begin ();
                        //BufferIterator++;
                        uiCrc16Calc = CalculateCRC16 (& msgBuffer [1], 19);
                        printf (" <CRC16 = 0x%04X>", uiCrc16Calc);
                        uiCrc16Recv = 0x0000;
                        uiCrc16Recv  =  RoutineCRC16Char2i16 (msgBuffer [23]);
                        uiCrc16Recv |= (RoutineCRC16Char2i16 (msgBuffer [22]) << 4);
                        uiCrc16Recv |= (RoutineCRC16Char2i16 (msgBuffer [21]) << 8);
                        uiCrc16Recv |= (RoutineCRC16Char2i16 (msgBuffer [20]) << 12);
                        if (uiCrc16Calc == uiCrc16Recv)
                            printf (" <+CRC16>");
                        else;
                    }
                    else
                    {
                        printf (" <-BAD_MSG_LEN (%zu)>", msgBuffer.size ());
                    }
                    bmsgStart = false;
                    msgBuffer.clear ();
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

   close (iSerialDev);
   goto next_iteration; // XA-XA-XA
}
