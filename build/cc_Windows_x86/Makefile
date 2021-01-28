CC:=g++
CC2:=gcc
ASM:=yasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_windows
include ../inc/makefile_cpua_win_x86_32
include ../inc/makefile_ui_windows
include ../inc/makefile_dev_win_x86
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
