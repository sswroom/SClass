#IDEDIR=~/arduino-1.8.5
IDEDIR=~/arduino-1.8.8
CC:=$(IDEDIR)/hardware/tools/avr/bin/avr-g++
CC2:=$(IDEDIR)/hardware/tools/avr/bin/avr-gcc
ASM:=$(IDEDIR)/hardware/tools/avr/bin/avr-as
LD:=$(IDEDIR)/hardware/tools/avr/bin/avr-ld
STRIP:=$(IDEDIR)/hardware/tools/avr/bin/avr-strip
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
