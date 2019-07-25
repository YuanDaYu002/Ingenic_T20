#!/bin/sh

OS=Linux
BOARD=x86
P2P=PPCS

#CC=arm-fullhan-linux-uclibcgnueabi-gcc
#CXX=arm-fullhan-linux-uclibcgnueabi-g++
#STRIP=arm-fullhan-linux-uclibcgnueabi-strip

CC=mips-linux-uclibc-gnu-gcc
CXX=mips-linux-uclibc-gnu-g++
STRIP=strip

make -f ./Makefile OS=${OS} BOARD=${BOARD} P2P=${P2P} CC=${CC} CXX=${CXX} STRIP=${STRIP} 
