#CC:=~/android_21/aarch64-linux-android/bin/aarch64-linux-android-clang++
#CC2:=~/android_21/aarch64-linux-android/bin/aarch64-linux-android-clang
#ASM:=~/android_21/aarch64-linux-android/bin/aarch64-linux-android-as
NDK_PATH=~/android-ndk-r19c
CC:=$(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang++
CC2:=$(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang
ASM:=$(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android-as
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
CFLAGS += -fPIE
LIBS += -fPIE -pie
LIBS += -static-libstdc++
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_android
include ../inc/makefile_cpua_sysvarm64
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_android
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest

