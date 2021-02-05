#include <string.h>
#include <stdio.h>
#include <exception>
//#include <new>
#include <unistd.h>
#include <stdlib.h>

#include "circlebuf.hpp"

/*
template <class BT> 
CircleBuffer<BT>::CircleBuffer (size_t _s)
{
}
*/

/*
template <class BT> 
CircleBuffer<BT>::~CircleBuffer ()
{
    if (buffer != NULL) delete [] buffer;   

    buffer = NULL;
    size   = 0;
}
*/

/*
template <class BT> 
bool CircleBuffer<BT>::empty (void)
{
    if (read == write) 
         return true;
    else return false;
}
*/

/*
template <class BT> 
void CircleBuffer<BT>::put (BT _d)
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

template <class BT> 
BT CircleBuffer<BT>::get (void)
{
//var
   BT data; 

//code
    if (buffer == NULL) return;   
    if (size <= 0) return;   

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
*/

/*
template <class BT> 
CircleBufferHelper<BT>::CircleBufferHelper (size_t _s) : buffer (_s)
{
}

template <class BT> 
CircleBufferHelper<BT>::~CircleBufferHelper ()
{
}
*/

/*
template <class BT> 
size_t CircleBufferHelper<BT>::size (void) 
{ 
//var
    size_t res;

//code
    access.Lock ();
    res = buffer.getBufferSize (); 
    access.Unlock ();

    return res;
}
*/

/*
template <class BT> 
void CircleBufferHelper<BT>::put (BT _d) 
{ 
    access.Lock ();
    buffer.put (_d); 
    access.Unlock ();
}
*/

/*
template <class BT> 
BT CircleBufferHelper<BT>::get (void) 
{ 
//var
    BT res;

//code
    access.Lock ();
    res = buffer.get (); 
    access.Unlock ();

    return res;
}
*/

/*
template <class BT> 
bool CircleBufferHelper<BT>::empty (void) 
{ 
//var
    bool res;

//code
    access.Lock ();
    res = buffer.empty (); 
    access.Unlock ();

    return res;
}
*/