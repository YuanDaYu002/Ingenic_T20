include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
TARGET=libencoder.a
TEST_TARGET=test_encoder

.PHONY: all clean test

all:$(TARGET)
	#cp -f $(TARGET) $(MY_LIBOUT)/
	cp -f $(OBJS) $(MY_OBJOUT)/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS)

$(TARGET):$(OBJS)
	#$(AR) $@ $^


clean:
	-rm -f $(TARGET) *.o *.d *.d.*
