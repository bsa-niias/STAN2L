#include <string.h>
#include <stdio.h>
#include <exception>
#include <new>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "circlebuf.hpp"

static void catch_f1 (int signo)
{
   printf ("catch_f1\n");
   exit (0);
}

static void catch_f2 (int signo)
{
   printf ("catch_f2\n");
   exit (0);
}

static void catch_f3 (int signo)
{
   printf ("catch_f3\n");
   exit (0);
}

static void catch_f4 (int signo)
{
   printf ("catch_f4\n");
   exit (0);
}

static void catch_f5 (int signo)
{
   printf ("catch_f5\n");
   exit (0);
}

static void catch_f6 (int signo)
{
   printf ("catch_f6\n");
   exit (0);
}

static void catch_f7 (int signo)
{
   printf ("catch_f7\n");
   exit (0);
}

template <class BT> 
CircleBuffer<BT>::CircleBuffer (uint32_t _s)
{
    buffer = NULL;
    size   = 0;

    try
    {
        if (_s > 0)
        {
           buffer = new BT [_s];
           if (buffer != NULL)
           {
              printf (" ok_0!\n");

              printf ("%lu,%u,%lu,%p ", sizeof (BT), _s, sizeof (BT)*_s, buffer); 
              fflush (stdout); 

              //printf ("%u ", sizeof (*buffer));
              memset (&buffer [0], 0x00, _s);  
              //for (uint32_t i = 0; i < _s; i++) 
              //{
              //    printf ("%u\n",i);
              //    buffer [i] = 0x00;
              //}
              printf (" ok_1!\n");
           }
           else;
        }
        else;
    }
    catch (const std::bad_alloc& bae)
    {
        buffer = NULL;
        size   = 0;
        printf ("exc0!\n");
    }
    //catch (const std::bad_array_new_length& banl)
    //{
    //    buffer = NULL;
    //    size   = 0;
    //    printf ("exc1!\n");
    //}
    catch (...)
    {
        buffer = NULL;
        size   = 0;
        printf ("exc12\n");
    }
}


int main (int c, char** v)
{
    char* buffer;  

    signal (SIGABRT, catch_f1); 
    signal (SIGFPE, catch_f2); 
    signal (SIGILL, catch_f3); 
    signal (SIGINT, catch_f4); 
    signal (SIGSEGV, catch_f5); 
    signal (SIGTERM, catch_f6); 
    signal (SIGKILL, catch_f7); 

/*
   for (int i=0;;)
   {
        printf ("%d\n", i);
        i = i + 1000000;
   }
return 0;
*/
   int _s = 100000;
   for (int i=0;;i++)
   {
       printf ("%d ",i); 
       //CircleBuffer<char>* cb1 = new CircleBuffer<char> (1000000);

           buffer = new char [_s];
           if (buffer != NULL)
           {
              printf (" ok_0!\n");

              printf ("%lu,%d,%lu,%p ", sizeof (char), _s, (sizeof (char))*_s, buffer); 
              fflush (stdout); 

              //printf ("%u ", sizeof (*buffer));
              //memset (&buffer [0], 0x00, _s);  
              for (int j = 0; j < _s; j++) 
              {
                    //printf ("%u\n",i);
                    //buffer [j] = 0x00;
                    //usleep (1);
              }
              printf (" ok_1!\n");
           }
           else;


       usleep (500);
   }

}
