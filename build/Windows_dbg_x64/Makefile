AR:=ar
CC:=g++
CC2:=gcc
ASM:=yasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

DSHOWFLAGS := -I "/cygdrive/c/Program Files/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses"
CFLAGS += -I ../../header -DUNICODE -D_UNICODE
CFLAGS += -O
CFLAGS += -g
#CFLAGS += -ffunction-sections -fdata-sections
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -static
#LIBS += -Wl,--gc-sections
#LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_windows
include ../inc/makefile_cpua_win_x86_64
include ../inc/makefile_ui_windows
include ../inc/makefile_dev_win_x86
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
