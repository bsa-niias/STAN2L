#ifndef __MTX_HPP__
#define __MTX_HPP__

#include <pthread.h>

class Mtx
{
public:
     Mtx ()
     {
          mutexres = pthread_mutex_init (&hMtx, NULL);
     }

     int Lock ()
     {
          if (mutexres != 0) return mutexres;
          return pthread_mutex_lock (&hMtx);
     }

     int Unlock ()
     {
          if (mutexres != 0) return mutexres;
          return pthread_mutex_unlock (&hMtx);
     }

     int getStatus ()
     {
          return mutexres;
     }

     ~Mtx ()
     {
          pthread_mutex_destroy (&hMtx);
     }

private:
     int mutexres;
     pthread_mutex_t hMtx;
};

#endif