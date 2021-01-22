#!/bin/bash

g++ -o bin/stan2l serial_test.cc source/crc/crc16.cc source/cfg/configfile.cc source/cfg/prgmcfg.cc
