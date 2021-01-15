// http://dfe.petrsu.ru/koi/posob/X/events.htm
// http://www.linuxlib.ru/prog/xlib.htm
// https://habr.com/ru/post/211751/

// System include
#include <stdio.h>
#include <unistd.h>
#include <iostream>

// Window include
#include <X11/Xlib.h>

// Project include
//#include "prgmcfg.h"

namespace nsDisplay
{
     static Display* gDisplay = NULL;
     static int gScreen = 0;
     static Window gWindow {0};

     Window BaseWin;
};

//ProgramConfig* anaconda_cfg = NULL;

int main (void)
{
//const
//var

//code
     std::cout << "Start Anaconda graf module ..." << std::endl;

     //ProgramConfig anaconda_cfg (".//pereezd.cfg", "");
     //int ml_count = anaconda_cfg.GetIntParam ("MAINLINE.COUNT",0);
     //std::cout << "MAINLINE.COUNT : " << ml_count << std::endl;

     nsDisplay::gDisplay = NULL;
     nsDisplay::gDisplay = XOpenDisplay (NULL);
     if (nsDisplay::gDisplay == NULL)
     {
        std::cerr << "Link X-Server terminal : FAIL!" << std::endl;
        return -1;
     }
     else;
     std::cout << "Link X-Server terminal : OK!" << std::endl;
     std::cout << "X-Server/Connection Number : " << ConnectionNumber (nsDisplay::gDisplay) << std::endl;
     std::cout << "X-Server/Protocol : " << ProtocolVersion (nsDisplay::gDisplay) 
                                         << '.'
                                         << ProtocolRevision (nsDisplay::gDisplay) 
                                         << std::endl;
     nsDisplay::gScreen = DefaultScreen (nsDisplay::gDisplay);
     nsDisplay::gWindow = RootWindow (nsDisplay::gDisplay, nsDisplay::gScreen);

     nsDisplay::BaseWin = XCreateSimpleWindow (nsDisplay::gDisplay,
                                               nsDisplay::gWindow,
                                               100, 100,
                                               300, 300,
                                               0, 0,
                                               WhitePixel (nsDisplay::gDisplay, nsDisplay::gScreen)
                                              );
     XMapWindow (nsDisplay::gDisplay, nsDisplay::BaseWin);
     XFlush (nsDisplay::gDisplay);

     sleep (5);

     XDestroyWindow (nsDisplay::gDisplay, nsDisplay::BaseWin);
     if (nsDisplay::gDisplay != NULL) 
        XCloseDisplay (nsDisplay::gDisplay);
     else;

     return 0;
}