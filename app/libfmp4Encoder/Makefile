
include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

##########################################################

####Flag##################################################
HLS_LDFLAG+=  -DDEBUG


###Lib####################################################


TARGET=libfmp4Encoder.a
TEST_TARGET=test_fmp4encoder



.PHONY: all clean 

all:$(TARGET)
	#cp -f $(TARGET) $(MY_LIBOUT)/
	cp -f $(OBJS) $(MY_OBJOUT)/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) $(HLS_LDFLAG)  > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS) $(HLS_LDFLAG) 

$(TARGET):$(OBJS)
	#$(AR) $@ $^



clean:
	-rm -f $(TARGET) *.o *.d *.d.*



 

