
MY_PATH   = $(shell pwd)
DEV_PATH  = $(MY_PATH)/..
SDK_PATH  = $(DEV_PATH)/Ingenic-SDK-T10T20-3.12.0-20180320
MY_OUT    = $(MY_PATH)/out

#use uclibc ? 注意后边不能有空格
CONFIG_UCLIBC_BUILD=y

CROSS_COMPILE ?= mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip
OBJDUMP = $(CROSS_COMPILE)objdump

### CFLAGS #######################################################################
CFLAGS =  -O2 -Wall -march=mips32r2  

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
else
  
endif

### LDFLAGS #######################################################################
LDFLAGS = -L$(MY_PATH)/3rdlibs

ifeq ($(CONFIG_UCLIBC_BUILD), y)
SDK_LIB_DIR = $(SDK_PATH)/lib/uclibc
LDFLAGS += -muclibc
else
SDK_LIB_DIR = $(SDK_PATH)/lib/glibc
endif

LDFLAGS += -L$(SDK_LIB_DIR)
LDFLAGS += -Wl,-gc-sections
#LDFLAGS += stdc++


### INC_FLAGS ######################################################################
INC_FLAGS = -I$(SDK_PATH)/include/imp
INC_FLAGS += -I$(SDK_PATH)/include
INC_FLAGS += -I$(SDK_PATH)/include/sysutils
INC_FLAGS += -I$(MY_PATH)/include
INC_FLAGS += -I$(MY_PATH)/3rdinc
INC_FLAGS += -I$(MY_PATH)/3rdinc/faac
INC_FLAGS += -I$(MY_PATH)/3rdinc/PPCS
INC_FLAGS += -I$(MY_PATH)/3rdinc/cJSON

####################################################################################
SENSOR_TYPE ?=
SENSOR_LIBS =
#SENSOR_SETTINGS_DIR = $(SDK_PATH)/res/sensor_settings

### LIBS  ##########################################################################
SDK_LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libsysutils.a  $(SDK_LIB_DIR)/libalog.a

#LIBS +=  -limp  -lsysutils  -lalog 
LIBS = $(SDK_LIBS) -lpthread -ldl -lm  -lc  -lrt 
LIBS += -lfaac  -lcJSON 


########################################################################
#			make command 				       #
########################################################################
TARGET = app_t20.bin
SUBDIRS = libencoder libamazon libstream libfmp4Encoder  main 

C_FILES = $(foreach D, $(SUBDIRS),$(wildcard $(D)/*.c))
#H_FILES = $(foreach D, $(SUBDIRS),$(wildcard $(D)/*h))

.PHONY: all clean


all:$(TARGET)
	
%.o:%.c 
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(SDK_LIBS) 
	@ echo "C_FILES: $(C_FILES)"
	@ echo "CFLAGS = $(CFLAGS)"	
	$(CC) $(CFLAGS) $(LDFLAGS) $(C_FILES) -o $(MY_OUT)/$@  $(INC_FLAGS)  $(LIBS) 
	$(STRIP) $(MY_OUT)/$@
	cp -f $(MY_OUT)/$@ /home/nfs/
	cp -f $(MY_OUT)/$@ /mnt/hgfs/vmware_share/
	
clean:
	$(foreach D, $(SUBDIRS), rm -f $(D)/*.o)


