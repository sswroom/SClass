CC:=~/Synology/aarch64-unknown-linux-gnueabi/bin/aarch64-unknown-linux-gnueabi-g++
CC2:=~/Synology/aarch64-unknown-linux-gnueabi/bin/aarch64-unknown-linux-gnueabi-gcc
ASM:=~/Synology/aarch64-unknown-linux-gnueabi/bin/aarch64-unknown-linux-gnueabi-as
LD:=~/Synology/aarch64-unknown-linux-gnueabi/bin/aarch64-unknown-linux-gnueabi-ld
STRIP:=~/Synology/aarch64-unknown-linux-gnueabi/bin/aarch64-unknown-linux-gnueabi-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -flax-vector-conversions
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS := -lrt
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_synology
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_sysvarm64
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
