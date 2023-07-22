AR:=ar
CC:=g++
CC2:=gcc
ASM:=nasm
LD:=ld
STRIP:=strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -std=c++11
CFLAGS += -I /opt/local/include
CFLAGS += -I /opt/local/include/mariadb/mysql
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS :=
LIBS += -rdynamic
#LIBS += -Wl,--gc-sections
LIBS += -Wl,-dead_strip
LIBS += -L /opt/local/lib
#LIBS += -L /opt/local/lib/mariadb/mysql

include ../inc/makefile_common
include ../inc/makefile_os_macos
include ../inc/makefile_cpua_sysvarm64
include ../inc/makefile_ui_gtk
include ../inc/makefile_dev_mac
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
