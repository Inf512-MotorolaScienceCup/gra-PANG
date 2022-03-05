@echo off
title Build
mkdir build
cd build
cmake ..
cmake --build .
pause