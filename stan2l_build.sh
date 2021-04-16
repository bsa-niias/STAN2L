#!/bin/bash

g++ -o bin/stan2l circlebuf.cc serial_test.cc console.cc sp.cc source/crc/crc16.cc source/cfg/configfile.cc source/cfg/prgmcfg.cc -lpthread

#g++ -o bin/stan2l_ipc ipc_test.cc -lpthread
