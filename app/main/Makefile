include ../Make.param

SRCS=$(wildcard $(MY_PATH)/main/*.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
TARGET=app_t20

.PHONY: all clean

all:$(TARGET)
	cp -f $(TARGET) $(MY_OUT)/
	cp -f $(TARGET) /work/tftp/
	cp -f $(TARGET) /home/nfs/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS)
	

$(TARGET): $(OBJS)
	@echo "OBJS = $(OBJS)"
	$(CC) $(MY_OBJOUT)/*.o  $^ -o $@  $(CFLAGS) $(LDFLAGS) $(INC_FLAGS) $(LIBS)
#采用LD的方式有问题，暂不采用此方式
#$(LD) $(LDFLAGS) -Map=$@.map -o $@ $^  -melf32ltsmip -gc-sections --start-group $(LIBS) --end-group
	$(OBJDUMP) -d $@ > $@.asm

clean:
	-rm -f $(TARGET) *.o *.d *.d.*
	-rm -f $(MY_OBJOUT)/*.o
