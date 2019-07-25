#!/bin/sh

OS=Linux
BOARD=x64
P2P=PPCS

#CC=arm-fullhan-linux-uclibcgnueabi-gcc
#CXX=arm-fullhan-linux-uclibcgnueabi-g++
#STRIP=arm-fullhan-linux-uclibcgnueabi-strip

CC=gcc
CXX=g++
STRIP=strip

make -f ./Makefile OS=${OS} BOARD=${BOARD} P2P=${P2P} CC=${CC} CXX=${CXX} STRIP=${STRIP} 