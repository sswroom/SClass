#CC:=~/android_21/x86_64-linux-android/bin/x86_64-linux-android-clang++
#CC2:=~/android_21/x86_64-linux-android/bin/x86_64-linux-android-clang
NDK_PATH=~/android-ndk-r19c
CC:=$(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android21-clang++
CC2:=$(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android21-clang
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
CFLAGS += -fPIE
LIBS += -fPIE -pie
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_android
include ../inc/makefile_cpua_sysvx86_64
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_android
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest

