#!/usr/bin/env bash


g++ -g -std=c++11 -I/usr/include/libftdi1 -c -o FTDI.o FTDI.cpp 
g++ FTDI.o -o main -lmpsse -lm -lftdi1