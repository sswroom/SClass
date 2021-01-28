STAGING_DIR:=/home/sswroom/openwrt/openwrt-sdk-ar71xx-generic_gcc-7.3.0_musl.Linux-x86_64/staging_dir/
TOOLCHAIN_DIR:=/home/sswroom/openwrt/openwrt-sdk-ar71xx-generic_gcc-7.3.0_musl.Linux-x86_64/staging_dir/toolchain-mips_24kc_gcc-7.3.0_musl/bin
#STAGING_DIR:=/home/sswroom/openwrt/openwrt-sdk-ar71xx-generic_gcc-5.5.0_musl.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=/home/sswroom/openwrt/openwrt-sdk-ar71xx-generic_gcc-5.5.0_musl.Linux-x86_64/staging_dir/toolchain-mips_24kc_gcc-5.5.0_musl/bin
export STAGING_DIR
CC:=$(TOOLCHAIN_DIR)/mips-openwrt-linux-g++
CC2:=$(TOOLCHAIN_DIR)/mips-openwrt-linux-gcc
ASM:=$(TOOLCHAIN_DIR)/mips-openwrt-linux-as
LD:=$(TOOLCHAIN_DIR)/mips-openwrt-linux-ld
STRIP:=$(TOOLCHAIN_DIR)/mips-openwrt-linux-strip
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
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all
export LC_ALL=C.UTF-8

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_openwrt
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_mips
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
