AR:=ar
CC:=g++
CC2:=gcc
ASM:=nasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -Wall
CFLAGS += -g
#CFLAGS += -ggdb
#CFLAGS += -gdwarf-2
#CFLAGS += -g -gstabs
CFLAGS += -O
#CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -std=c++11

CFLAGS += -I ../../header
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
include ../inc/makefile_cpua_sysvx86_64
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_x86
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
