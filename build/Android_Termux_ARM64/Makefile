CC:=g++
CC2:=gcc
ASM:=as
LD:=ld
STRIP:=strip
CLASSDIR:=../../
ANDROID_API:=$(shell getprop ro.build.version.sdk)

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -flax-vector-conversions
#CFLAGS += -D__ANDROID_API__=$(ANDROID_API)
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
include ../inc/makefile_cpua_sysvarm64
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_android
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
