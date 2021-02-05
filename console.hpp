#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

//#include <iostream>

#include <iostream>

#include "mtx.hpp"

class ConsoleDbg
{
public:
    ConsoleDbg ();
   
//private:        
    Mtx mtxCON;
};

ConsoleDbg& CD ();

std::ios_base& ConsoleLock   (std::ios_base& _Iosbase);
std::ios_base& ConsoleUnlock (std::ios_base& _Iosbase);

#endif