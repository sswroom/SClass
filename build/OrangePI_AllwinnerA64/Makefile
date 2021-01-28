CC:=g++
CC2:=gcc
ASM:=as
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -g
#CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DTIMEDWAIT_BUG=1
CFLAGS += -flax-vector-conversions
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -rdynamic
#LIBS += -Wl,--gc-sections
#LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux
include ../inc/makefile_cpua_sysvarm64
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_orangepi
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
