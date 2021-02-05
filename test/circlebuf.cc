#include <string.h>
#include <stdio.h>
#include <exception>
//#include <new>
#include <unistd.h>
#include <stdlib.h>

#include "circlebuf.hpp"

template <class BT> 
CircleBuffer<BT>::CircleBuffer (size_t _s)
{
    buffer = NULL;
    size   = 0;

    //try
    //{
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
    //}
    //catch (const std::bad_alloc& bae)
    //{
    //    buffer = NULL;
    //    size   = 0;
    //}
    
    read  = 1;
    write = 1;
}

template <class BT> 
bool CircleBuffer<BT>::empty (void)
{
    if (read == write) 
         return true;
    else return false;
}

template <class BT> 
void CircleBuffer<BT>::put (BT _d)
{
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

template <class BT> 
CircleBufferHelper<BT>::CircleBufferHelper (size_t _s) : buffer (_s)
{
}

template <class BT> 
size_t CircleBufferHelper<BT>::size (void) 
{ 
    return buffer.getBufferSize (); 
}

template <class BT> 
void CircleBufferHelper<BT>::put (BT _d) 
{ 
    buffer.put (_d); 
}

template <class BT> 
BT CircleBufferHelper<BT>::get (void) 
{ 
    return buffer.get (); 
}

template <class BT> 
bool CircleBufferHelper<BT>::empty (void) 
{ 
    return buffer.empty (); 
}



int main (int c, char** v)
{
   CircleBufferHelper<uint8_t> cbh (5);
   uint8_t d;

   printf ("01) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('0');
   printf ("02) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('1');
   printf ("03) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('2');
   printf ("04) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('3');
   printf ("05) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('4');
   printf ("06) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('5');
   printf ("07) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('6');
   printf ("08) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('7');
   printf ("09) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   d = cbh.get ();
   printf ("10) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   d = cbh.get ();
   printf ("11) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   cbh.put ('8');
   printf ("12) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('9');
   printf ("13) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('a');
   printf ("14) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   d = cbh.get ();
   printf ("15) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   cbh.put ('b');
   printf ("16) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   d = cbh.get ();
   printf ("17) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   d = cbh.get ();
   printf ("18) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   cbh.put ('c');
   printf ("19) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('d');
   printf ("20) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   cbh.put ('e');
   printf ("21) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   d = cbh.get ();
   printf ("22) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   d = cbh.get ();
   printf ("23) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   d = cbh.get ();
   printf ("24) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   d = cbh.get ();
   printf ("25) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   d = cbh.get ();
   printf ("26) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   cbh.put ('f');
   printf ("27) %lu(r), %lu(w)\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer ());

   d = cbh.get ();
   printf ("28) %lu(r), %lu(w) d=%c\n", cbh.dbg_ReadPointer (), cbh.dbg_WritePointer (), d);

   return 1;
}
