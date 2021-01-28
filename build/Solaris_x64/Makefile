AR:=ar
CC:=g++
CC2:=gcc
ASM:=yasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -m64
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -rdynamic
LIBS += -m64
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_solaris
include ../inc/makefile_cpua_sysvx86_64
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_solaris
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
