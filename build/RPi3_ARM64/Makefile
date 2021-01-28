CC:=g++
CC2:=gcc
ASM:=as
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DTIMEDWAIT_BUG
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
include ../inc/makefile_os_linux
include ../inc/makefile_cpua_sysvarm64
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_rpi3_novc
include ../inc/makefile_cpu_bcm2836
include ../inc/Makefile_linuxtest
