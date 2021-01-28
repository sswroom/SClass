TOOLCHAIN_DIR:=~/ROS-SDK-r3000-3.0.1/staging_dir/target-r3000/arm-sam9x25-linux-gnueabi/bin
CC:=$(TOOLCHAIN_DIR)/arm-sam9x25-linux-gnueabi-g++
CC2:=$(TOOLCHAIN_DIR)/arm-sam9x25-linux-gnueabi-gcc
ASM:=$(TOOLCHAIN_DIR)/arm-sam9x25-linux-gnueabi-as
LD:=$(TOOLCHAIN_DIR)/arm-sam9x25-linux-gnueabi-ld
STRIP:=$(TOOLCHAIN_DIR)/arm-sam9x25-linux-gnueabi-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -DDEBUGCON
CFLAGS += -DNO_CONSOLE
CFLAGS += -DHTTPPORT=8088
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -flax-vector-conversions
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -static
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_r3000
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
