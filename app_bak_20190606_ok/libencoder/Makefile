
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET = libencoder.a



.PHONY: all clean


all:$(TARGET)




%.o:%.c 
	$(CC) -c $(CFLAGS) $< -o $@




clean:



	
