#!/bin/sh

cd app
mkdir ../build
g++ -std=c++11 -O2 -o ../build/main main.cpp response.cpp ../interact/demodulator.cpp ../interact/modulator.cpp
