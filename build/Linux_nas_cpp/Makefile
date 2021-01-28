CC:=g++
CC2:=gcc
ASM:=yasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -g
#CFLAGS += -ffunction-sections -fdata-sections
#CFLAGS += -mavx
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -rdynamic
#LIBS += -Wl,--gc-sections
#LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_cpua_cpp
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_unk
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
