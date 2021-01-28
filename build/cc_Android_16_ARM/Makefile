CC:=~/android_16/arm-linux-androideabi/bin/arm-linux-androideabi-clang++
CC2:=~/android_16/arm-linux-androideabi/bin/arm-linux-androideabi-clang
ASM:=~/android_16/arm-linux-androideabi/bin/arm-linux-androideabi-as
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
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
include ../inc/makefile_os_android
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_android
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest

