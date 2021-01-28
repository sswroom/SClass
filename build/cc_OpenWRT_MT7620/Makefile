STAGING_DIR:=~/openwrt/OpenWrt-SDK-15.05.1-ramips-mt7620_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/
TOOLCHAIN_DIR:=~/openwrt/OpenWrt-SDK-15.05.1-ramips-mt7620_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
#STAGING_DIR:=~/openwrt/OpenWrt-SDK-15.05-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/OpenWrt-SDK-15.05-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
CC:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-uclibc-g++
CC2:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-uclibc-gcc
ASM:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-uclibc-as
LD:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-uclibc-ld
STRIP:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-uclibc-strip
export STAGING_DIR
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -DNO_STACKTRACE=1
CFLAGS += -Os
CFLAGS += -fno-exceptions
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -flax-vector-conversions
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS := -nodefaultlibs -lgcc -lc
#LIBS += -lstdc++
LIBS += -lsupc++
LIBS += -lm
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_openwrt
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_mips
include ../inc/makefile_cpu_mt7620
include ../inc/Makefile_linuxtest
