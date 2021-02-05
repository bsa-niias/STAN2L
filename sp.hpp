#ifndef __SP_HPP__
#define __SP_HPP__

#include <string>
#include <termios.h>

class SerialPort
{
public:
    SerialPort ();
    SerialPort (const char* _dev);

    const char* getDeviceName ();
    void setDeviceName (const char* _dev);

    int getDeviceHandle ();

    virtual int Open () =0;
    int Close ();

    virtual int setConnectionAttr (speed_t _Speed, tcflag_t _Len, tcflag_t _Parity, tcflag_t _Stops) =0;
    void setTimeout (time_t sec = 0, long usec = 0);

protected:
    // Device name in Linux spacename
    std::string DevName;

    // result of system routines
    int SysRoutineRes;
    // handle device  
    int DevHandle;
    struct termios tio_old; 
    struct termios tio;

    time_t timeoutSec;
    long   timeoutUSec;
};

class SerialPort_r: public SerialPort
{
public:
    virtual int Open ();
    virtual int setConnectionAttr (speed_t _Speed, tcflag_t _Len, tcflag_t _Parity, tcflag_t _Stops);

    // using select & timeout
    // ... first call "setTimeout (...)" routine
    // ... wrap "FD_SET"
    int wait ();
    // ... wrap "FD_ISSET"
    int check_ready_data ();

    int Read_1b (char* _Data);
    //int Read_Nb (char* _Data);
};

class SerialPort_w: public SerialPort
{
public:
    virtual int Open ();
    virtual int setConnectionAttr (speed_t _Speed, tcflag_t _Len, tcflag_t _Parity, tcflag_t _Stops);

    int Write (void* _Data, size_t _DataLen);
};

#endif