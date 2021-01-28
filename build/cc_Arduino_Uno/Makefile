CC:=avr-g++
CC2:=avr-gcc
ASM:=avr-as
LD:=avr-ld
STRIP:=avr-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -DF_CPU=16000000UL -mmcu=atmega328p
CFLAGS += -O
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DNO_DB
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS := -mmcu=atmega328p
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_none
include ../inc/makefile_os_none_atmega328
include ../inc/makefile_cpua_avr
include ../inc/makefile_ui_unk
include ../inc/makefile_dev_arduino
include ../inc/makefile_cpu_atmega328
include ../inc/Makefile_linuxtest
