#include <iostream>

#include "console.hpp"
#include "mtx.hpp"

ConsoleDbg& CD ()
{
    static ConsoleDbg static_cd;
    return static_cd;
}

std::ios_base& ConsoleLock   (std::ios_base& _Iosbase)
{
    CD().mtxCON.Lock ();
    return _Iosbase;
}

std::ios_base& ConsoleUnlock (std::ios_base& _Iosbase)
{
    CD().mtxCON.Unlock ();
    return _Iosbase;
}

ConsoleDbg::ConsoleDbg ()
{
    mtxCON.Lock ();
    std::cout << "ConsoleDbg init ..." << std::endl;
    mtxCON.Unlock ();
}

