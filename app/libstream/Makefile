include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

##########################################################
#INC_FLAGS+= -I$(MY_PATH)/3rdinc/PPCS 
#INC_FLAGS+= -I$(MY_PATH)/3rdinc/curl 
####Flag##################################################
#MEDIA_SERVER_LDFLAG= -lPPCS_API -lcurl -lc -lpthread
MEDIA_SERVER_LDFLAG+=  -DDEBUG

#LiteOS 系统必须使用支持唤醒的参数（Advanced P2P(RTOS/Wakeup)）
MEDIA_SERVER_LDFLAG+= -DP2P_SUPORT_WAKEUP  
MEDIA_SERVER_LDFLAG+= -DENABLE_NTP_CLIENT_DEBUG

###Lib####################################################
MEDIA_SERVER_LIB =-L../3rdlibs


TARGET=libstream.a
TEST_TARGET=test_stream

.PHONY: all clean test

all:$(TARGET)
	@ #cp -f $(TARGET) $(MY_LIBOUT)/
	cp -f $(OBJS) $(MY_OBJOUT)/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) $(MEDIA_SERVER_LDFLAG) $(MEDIA_SERVER_LIB)  > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS) $(MEDIA_SERVER_LDFLAG) $(MEDIA_SERVER_LIB)

$(TARGET):$(OBJS)
	#$(AR)  $@ $^


clean:
	-rm -f $(TARGET) *.o *.d *.d.*
