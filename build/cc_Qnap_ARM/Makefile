CC:=~/qnap/cross-project/arm/marvell/bin/arm-none-linux-gnueabi-g++
CC2:=~/qnap/cross-project/arm/marvell/bin/arm-none-linux-gnueabi-gcc
ASM:=~/qnap/cross-project/arm/marvell/bin/arm-none-linux-gnueabi-as
LD:=~/qnap/cross-project/arm/marvell/bin/arm-none-linux-gnueabi-ld
STRIP:=~/qnap/cross-project/arm/marvell/bin/arm-none-linux-gnueabi-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -DFPUBUG
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
include ../inc/makefile_dev_qnap
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
