STAGING_DIR:=~/openwrt/openwrt-sdk-18.06.0-ramips-mt76x8_gcc-7.3.0_musl.Linux-x86_64/staging_dir/
TOOLCHAIN_DIR:=~/openwrt/openwrt-sdk-18.06.0-ramips-mt76x8_gcc-7.3.0_musl.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin
#STAGING_DIR:=~/openwrt/lede-sdk-17.01.4-ramips-mt7688_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/lede-sdk-17.01.4-ramips-mt7688_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin
#STAGING_DIR:=~/openwrt/lede-sdk-17.01.4-ramips-mt7628_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/lede-sdk-17.01.4-ramips-mt7628_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin
export STAGING_DIR
CC:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-g++
CC2:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-gcc
ASM:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-as
LD:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-ld
STRIP:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-strip
CLASSDIR:=../../
export LC_ALL=C

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
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_openwrt
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_mips
include ../inc/makefile_cpu_mt7628
include ../inc/Makefile_linuxtest
