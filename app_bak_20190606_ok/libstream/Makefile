include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

.PHONY: all clean

all:$(OBJS)
	cp -f *.o  $(MY_OBJOUT)/
	

%.o:%.c
	@$(CC) -o $@ -c $< $(LDFLAG) $(CFLAGS) $(INC_FLAGS)

clean:
	rm -f  *.o 
