CC:=~/android_21/mips64el-linux-android/bin/mips64el-linux-android-clang++
CC2:=~/android_21/mips64el-linux-android/bin/mips64el-linux-android-clang
ASM:=~/android_21/mips64el-linux-android/bin/mips64el-linux-android-as
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
CFLAGS += -fPIE
LIBS += -fPIE -pie
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_android
include ../inc/makefile_cpua_mips64
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_android
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest

