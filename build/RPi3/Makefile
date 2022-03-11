CC:=g++
CC2:=gcc
ASM:=as
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -Wsign-conversion
CFLAGS += -Wconversion
CFLAGS += -Wswitch-enum
CFLAGS += -finline-functions 
CFLAGS += -finline-small-functions 
CFLAGS += -findirect-inlining -Wno-strict-aliasing
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -flax-vector-conversions
CFLAGS += -mfpu=neon-vfpv4
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
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_rpi
include ../inc/makefile_cpu_bcm2836
include ../inc/Makefile_linuxtest
