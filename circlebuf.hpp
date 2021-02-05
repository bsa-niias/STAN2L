#ifndef __CIRCLE_BUFFER__
#define __CIRCLE_BUFFER__

#include <stdint.h>
#include "mtx.hpp"

//template <class BT> class CircleBufferHelper;

template <class BT> class CircleBuffer
{
// BT and DT same type's - shadow template type not allow
template <class DT> friend class CircleBufferHelper;

public:
CircleBuffer (size_t _s)
{
    buffer = NULL;
    size   = 0;

    try
    {
        if (_s > 0)
        {
           buffer = new BT [_s+1];
           if (buffer != NULL)
           {
              // if swap is small or memory very big - SIGFAULT up and halt
              // this exception not rise - only halt
              memset (&buffer [0], 0x00, (_s+1)*sizeof (BT));  
              
              size = _s;
           }
           else;
        }
        else;
    }
    catch (const std::bad_alloc& bae)
    {
        buffer = NULL;
        size   = 0;
    }
    
    read  = 1;
    write = 1;
 }

~CircleBuffer ()
{
    if (buffer != NULL) delete [] buffer;   

    buffer = NULL;
    size   = 0;
}

size_t getBufferSize (void) 
{ 
    return size; 
}

protected:
bool empty (void)
{
    if (read == write) 
         return true;
    else return false;
}

void put (BT _d)
{
    if (buffer == NULL) return;   
    if (size <= 0) return;   
 
    buffer [write] = _d;
    
    if (write == size)
         write = 1;
    else write = 1 + write;

    if (read == write) // overflow
    {
       if (read == size)   
            read = 1;
       else read = 1 + read;
    }
    else;
}

BT get (void)
{
//var
   BT data; 

//code
    if (buffer == NULL) return 0xff;   
    if (size <= 0) return 0xff;   

    if (read == write)
    {
       return -1;
    }
    else
    {
        data = buffer [read];
        if (read == size)   
             read = 1;
        else read = 1 + read;
    }

    return data;
}

private:
    // index==0 not use
    BT*    buffer;   // buffer
    size_t size;     // size of buffer

    size_t read;     // first available read
    size_t write;    // first available write 
};


template <class BT> class CircleBufferHelper 
{
public:
CircleBufferHelper (size_t _s = 1024) : buffer (_s) 
{
}

~CircleBufferHelper () 
{
}

size_t size (void)
{
//var
    size_t res;

//code
    access.Lock ();
    res = buffer.getBufferSize (); 
    access.Unlock ();

    return res;
}

void put (BT _d)
{
    access.Lock ();
    buffer.put (_d); 
    access.Unlock ();
}

BT get (void)
{
//var
    BT res;

//code
    access.Lock ();
    res = buffer.get (); 
    access.Unlock ();

    return res;
}

bool empty (void)
{
//var
    bool res;

//code
    access.Lock ();
    res = buffer.empty (); 
    access.Unlock ();

    return res;
}

private:
    CircleBuffer<BT> buffer;
    Mtx access;

// helper routine for debug
public:
    size_t dbg_ReadPointer  () { return buffer.read; }
    size_t dbg_WritePointer () { return buffer.write; }
    BT dbg_at (size_t index) { return buffer.buffer [index]; }
};

#endif