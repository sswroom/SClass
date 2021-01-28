CC:=~/Synology/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-g++
CC2:=~/Synology/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-gcc
ASM:=~/Synology/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-as
LD:=~/Synology/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-ld
STRIP:=~/Synology/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-strip
#CC:=~/Synology/build_env/ds.6281-6.1/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-g++
#CC2:=~/Synology/build_env/ds.6281-6.1/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-gcc
#ASM:=~/Synology/build_env/ds.6281-6.1/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-as
#LD:=~/Synology/build_env/ds.6281-6.1/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-ld
#STRIP:=~/Synology/build_env/ds.6281-6.1/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-strip
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
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
