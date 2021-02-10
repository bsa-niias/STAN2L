#include "sp.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

SerialPort::SerialPort ()
{
    DevName = "/dev/null";
    DevName.push_back (0x00);

    bzero (&tio_old, sizeof (tio_old));
    bzero (&tio, sizeof (tio));
    DevHandle     = -1;
    SysRoutineRes = -1;

    timeoutSec  = 0;
    timeoutUSec = 0;
}

SerialPort::SerialPort (const char* _dev)
{
    DevName = "/dev/null";

    if (_dev != NULL)  DevName = _dev;
    DevName.push_back (0x00);

    bzero (&tio_old, sizeof (tio_old));
    bzero (&tio, sizeof (tio));
    DevHandle     = -1;
    SysRoutineRes = -1;

    timeoutSec  = 0;
    timeoutUSec = 0;
}

const char* SerialPort::getDeviceName ()
{
    return DevName.c_str ();
}

void SerialPort::setDeviceName (const char* _dev)
{
    if (_dev == NULL) return;

    DevName.clear ();
    DevName = _dev;
    DevName.push_back (0x00);   
}

int SerialPort::getDeviceHandle () 
{
    return DevHandle;
}

int SerialPort::Close ()
{
    SysRoutineRes = ::close (DevHandle);

    if (SysRoutineRes == 0) 
         DevHandle = -1; 
    else;

    return SysRoutineRes;
}

void SerialPort::setTimeout (time_t sec, long usec)
{
    timeoutSec  = sec;
    timeoutUSec = usec;
}

/////////////

int SerialPort_r::Open ()
{
    SysRoutineRes = ::open (DevName.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY); // O_RDWR | O_NDELAY);  

    if (SysRoutineRes == -1) 
         return -1;
    else; 
    
    DevHandle = SysRoutineRes;
    return DevHandle;
}

int SerialPort_r::setConnectionAttr (speed_t _Speed, tcflag_t _Len, tcflag_t _Parity, tcflag_t _Stops)
{
    SysRoutineRes = ::tcgetattr (DevHandle, &tio_old); // get old 
    //printf ("tcgetattr return : %d (errno : %d)\n", iSysRoutineRes, errno);

    // new serial setting
    bzero (&tio, sizeof (tio));
    SysRoutineRes = ::cfsetispeed (&tio, _Speed);   // speed
    //printf ("cfsetispeed return : %d (errno : %d)\n", SysRoutineRes, errno);
    //SysRoutineRes = ::cfsetospeed (&tio, _Speed);   
    //printf ("cfsetospeed return : %d (errno : %d)\n", SysRoutineRes, errno);

    //tio.c_cflag != BAUDRATE;     
    tio.c_cflag |= (CLOCAL | CREAD);
    // 8N1
    //tio.c_cflag &= ~CSIZE;     // size
    tio.c_cflag |= _Len;    //CS8;
    tio.c_cflag &= _Parity; //~PARENB;      // parity
    tio.c_cflag &= _Stops;  //~CSTOPB;      // 1_stop 
    //tio.c_cflag |= CSTOPB;                // 2_stop - for example
    // Disable flow control
    tio.c_cflag &= ~CRTSCTS;
   
    //tio.c_cflag &= ~(IXON | IXOFF | IXANY);
    //tio.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_lflag = 0;
    tio.c_oflag = 0;
    tio.c_iflag = IGNPAR;         // ignore parity error

    tio.c_cc [VTIME] = 0;
    tio.c_cc [VMIN]  = 1;

    SysRoutineRes = ::tcsetattr (DevHandle, TCSANOW, &tio);
    //printf ("tcsetattr return : %d (errno : %d)\n", SysRoutineRes, errno);
    
    return SysRoutineRes;
}

int SerialPort_r::Read_1b (char* _Data)
{
//var
    int read_size = 1;
    char read_c; 

//code   
    SysRoutineRes = read (DevHandle, &read_c, read_size);
    if (SysRoutineRes > 0)
    {
       *_Data = read_c;
    }
    else;

    return SysRoutineRes;
}

int SerialPort_r::wait ()
{
//var
    fd_set        fds_reads;
    timeval       tv_reads; 
    
//code   
    FD_ZERO (&fds_reads);       
    FD_SET  (DevHandle,  &fds_reads);
    tv_reads.tv_sec  = timeoutSec;
    tv_reads.tv_usec = timeoutUSec;
    SysRoutineRes = select (1+DevHandle, 
                            &fds_reads, 
                            0, 
                            0, 
                            &tv_reads);

    if (SysRoutineRes > 0)                        
    {
        // return SysRoutineRes > 0 !!!! see below
        if (FD_ISSET (DevHandle, &fds_reads)); 
        else
        {
            // is it impassible?
            return 0;
        }
    }
    else;

    return SysRoutineRes;
}

/////////////

int SerialPort_w::Open ()
{
    SysRoutineRes = ::open (DevName.c_str(), O_WRONLY | O_NOCTTY); // O_RDWR | O_NDELAY);  

    if (SysRoutineRes == -1) 
         return -1;
    else; 
    
    DevHandle = SysRoutineRes;
    return DevHandle;
}

int SerialPort_w::setConnectionAttr (speed_t _Speed, tcflag_t _Len, tcflag_t _Parity, tcflag_t _Stops)
{
    SysRoutineRes = ::tcgetattr (DevHandle, &tio_old); // get old 
    //printf ("tcgetattr return : %d (errno : %d)\n", iSysRoutineRes, errno);

    // new serial setting
    bzero (&tio, sizeof (tio));
    //SysRoutineRes = ::cfsetispeed (&tio, _Speed);   // speed
    //printf ("cfsetispeed return : %d (errno : %d)\n", SysRoutineRes, errno);
    SysRoutineRes = ::cfsetospeed (&tio, _Speed);   
    //printf ("cfsetospeed return : %d (errno : %d)\n", SysRoutineRes, errno);

    //tio.c_cflag != BAUDRATE;     
    tio.c_cflag |= (CLOCAL | CREAD);
    // 8N1
    //tio.c_cflag &= ~CSIZE;     // size
    tio.c_cflag |= _Len;    //CS8;
    tio.c_cflag &= _Parity; //~PARENB;      // parity
    tio.c_cflag &= _Stops;  //~CSTOPB;      // 1_stop 
    //tio.c_cflag |= CSTOPB;                // 2_stop - for example
    // Disable flow control
    tio.c_cflag &= ~CRTSCTS;
   
    //tio.c_cflag &= ~(IXON | IXOFF | IXANY);
    //tio.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_lflag = 0;
    tio.c_oflag = 0;
    tio.c_iflag = IGNPAR;         // ignore parity error

    tio.c_cc [VTIME] = 0;
    tio.c_cc [VMIN]  = 1;

    SysRoutineRes = ::tcsetattr (DevHandle, TCSANOW, &tio);
    //printf ("tcsetattr return : %d (errno : %d)\n", SysRoutineRes, errno);
    
    return SysRoutineRes;
}

int SerialPort_w::Write (void* _Data, size_t _DataLen)
{
     return write (DevHandle, (void*) _Data, _DataLen);
}

