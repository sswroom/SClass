AR:=ar
CC:=g++
CC2:=gcc
ASM:=nasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -Wall
CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -rdynamic
LIBS += -static-libstdc++
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_cpua_sysvx86_64
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_x86
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
