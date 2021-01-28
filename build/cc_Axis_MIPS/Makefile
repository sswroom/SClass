CC:=/usr/local/mipsisa32r2el/r23/bin/mipsisa32r2el-axis-linux-gnu-g++
CC2:=/usr/local/mipsisa32r2el/r23/bin/mipsisa32r2el-axis-linux-gnu-gcc
ASM:=/usr/local/mipsisa32r2el/r23/bin/mipsisa32r2el-axis-linux-gnu-as
LD:=/usr/local/mipsisa32r2el/r23/bin/mipsisa32r2el-axis-linux-gnu-ld
STRIP:=/usr/local/mipsisa32r2el/r23/bin/mipsisa32r2el-axis-linux-gnu-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS := -lrt
LIBS += -rdynamic
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_cpua_mips
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_unk
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
