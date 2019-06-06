include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
TARGET=app_t20

.PHONY: all clean

all:$(TARGET)
	cp -f $(TARGET).bin $(MY_OUT)/
	cp -f $(TARGET).bin /work/tftp/
	cp -f $(TARGET).bin /home/nfs/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS)

$(TARGET):$(OBJS)
	$(LD) $(LDFLAGS) -Map=$@.map -o $@ $^ --start-group $(LIBS)  --end-group
	cp $@ $@.bin
	$(OBJDUMP) -d $@ > $@.asm

clean:
	-rm -f $(TARGET) *.o *.d *.d.*
