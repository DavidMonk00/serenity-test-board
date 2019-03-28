#!/usr/bin/env bash


g++ -g -std=c++11 -I/usr/include/libftdi1 -c -o FTDI.o FTDI.cpp
g++ -g -std=c++11 -I/usr/include/libftdi1 -c -o main.o main.cpp
g++ -g -std=c++11 -I/usr/include/libftdi1 -c -o I2CFanout.o I2CFanout.cpp
g++ FTDI.o main.o I2CFanout.o -o main -lm -lftdi1
