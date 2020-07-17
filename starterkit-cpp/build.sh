#!/bin/sh

cd app
mkdir ../build
g++ -std=c++11 -o ../build/main main.cpp
