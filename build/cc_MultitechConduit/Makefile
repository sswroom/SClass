TOOLCHAIN_DIR:=/home/sswroom/NetComm/SDK_Bovine_ntc_40w/compiler/bin
CC:=$(TOOLCHAIN_DIR)/arm-cdcs-linux-gnueabi-g++
CC2:=$(TOOLCHAIN_DIR)/arm-cdcs-linux-gnueabi-gcc
ASM:=$(TOOLCHAIN_DIR)/arm-cdcs-linux-gnueabi-as
LD:=$(TOOLCHAIN_DIR)/arm-cdcs-linux-gnueabi-ld
STRIP:=$(TOOLCHAIN_DIR)/arm-cdcs-linux-gnueabi-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -Os
CFLAGS += -fno-exceptions
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DNO_STACKTRACE=1
CFLAGS += -flax-vector-conversions
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -lrt
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all
#export LC_ALL=C

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_multitech
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
