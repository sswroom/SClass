TOOLCHAIN_DIR:=/opt/poky/1.6.3/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi
CC:=$(TOOLCHAIN_DIR)/arm-poky-linux-gnueabi-g++
CC2:=$(TOOLCHAIN_DIR)/arm-poky-linux-gnueabi-gcc
ASM:=$(TOOLCHAIN_DIR)/arm-poky-linux-gnueabi-as
LD:=$(TOOLCHAIN_DIR)/arm-poky-linux-gnueabi-ld
STRIP:=$(TOOLCHAIN_DIR)/arm-poky-linux-gnueabi-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -flax-vector-conversions
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_eurotech
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
