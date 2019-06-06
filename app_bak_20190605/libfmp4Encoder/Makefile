
include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

##########################################################

####Flag##################################################
HLS_LDFLAG+=  -DDEBUG


###Lib####################################################


TARGET=libfmp4Encoder.a
TEST_TARGET=test_fmp4encoder



.PHONY: all clean test

all:$(TARGET)
	cp -f $(TARGET) $(MY_LIBOUT)/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) $(HLS_LDFLAG)  > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS) $(HLS_LDFLAG) 

$(TARGET):$(OBJS)
	$(AR) -rc $@ $^

test:$(OBJS) ../main/main.o
	$(LD) $(LDFLAGS) -Map=$@.map -o $@ $^ --start-group $(SDK_LIB) $(LIBDEP) -lencoder -lamazon -lstream -lfmp4Encode --end-group
	cp -f  $@ $(TEST_TARGET).bin
	$(OBJDUMP) -d $@ > $(TEST_TARGET).asm

clean:
	-rm -f $(TARGET) *.o *.d *.d.*



 

