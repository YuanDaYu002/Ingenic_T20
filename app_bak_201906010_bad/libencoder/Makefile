include ../Make.param

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))
TARGET=libencoder.a
TEST_TARGET=test_encoder

.PHONY: all clean test

all:$(TARGET)
	@echo "CFLAGS = $(CFLAGS)"
	cp -f $(TARGET) $(MY_LIBOUT)/

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $< $(INC_FLAGS) $(CFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

%.o:%.c
	@$(CC) -o $@ -c $< $(CFLAGS) $(INC_FLAGS)

$(TARGET):$(OBJS)
	$(AR) -rc $@ $^

test:$(OBJS) ../main/main.o
	$(LD) $(LDFLAGS) -Map=$@.map -o $@ $^ --start-group $(LIBS) --end-group
	cp -f $(TEST_TARGET) $(TEST_TARGET).bin
	$(OBJDUMP) -d $(TEST_TARGET) > $(TEST_TARGET).asm

clean:
	-rm -f $(TARGET) *.o *.d *.d.*
