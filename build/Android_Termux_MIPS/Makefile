CC:=g++
CC2:=gcc
ASM:=nasm
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
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_android
include ../inc/makefile_cpua_mips
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_unk
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
