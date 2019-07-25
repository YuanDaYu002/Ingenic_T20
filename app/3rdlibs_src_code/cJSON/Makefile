
TARGET=libcJSON.a
CC=mips-linux-uclibc-gnu-gcc
CFLAGS=-march=mips32r2
LDFLAGS=

all:
	$(CC) -o  cJSON.o  -c cJSON.c $(CFLAGS) $(LDFLAGS)
	ar -cr $(TARGET) cJSON.o  	

.PHONY: clean

clean:
	rm -f 	*.o
	rm -f	$(TARGET)

