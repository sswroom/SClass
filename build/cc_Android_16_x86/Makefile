CC:=~/android_16/i686-linux-android/bin/i686-linux-android-clang++
CC2:=~/android_16/i686-linux-android/bin/i686-linux-android-clang
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
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_android
include ../inc/makefile_cpua_sysvx86_32
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_android
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest

