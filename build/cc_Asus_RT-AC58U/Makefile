STAGING_DIR:=/home/sswroom/openwrt/openwrt-sdk-18.06.0-ipq40xx_gcc-7.3.0_musl_eabi.Linux-x86_64/staging_dir/
TOOLCHAIN_DIR:=/home/sswroom/openwrt/openwrt-sdk-18.06.0-ipq40xx_gcc-7.3.0_musl_eabi.Linux-x86_64/staging_dir/toolchain-arm_cortex-a7+neon-vfpv4_gcc-7.3.0_musl_eabi/bin
#STAGING_DIR:=/home/sswroom/openwrt/openwrt-sdk-ipq40xx_gcc-7.3.0_musl_eabi.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=/home/sswroom/openwrt/openwrt-sdk-ipq40xx_gcc-7.3.0_musl_eabi.Linux-x86_64/staging_dir/toolchain-arm_cortex-a7+neon-vfpv4_gcc-7.3.0_musl_eabi/bin
export STAGING_DIR
CC:=$(TOOLCHAIN_DIR)/arm-openwrt-linux-g++
CC2:=$(TOOLCHAIN_DIR)/arm-openwrt-linux-gcc
ASM:=$(TOOLCHAIN_DIR)/arm-openwrt-linux-as
LD:=$(TOOLCHAIN_DIR)/arm-openwrt-linux-ld
STRIP:=$(TOOLCHAIN_DIR)/arm-openwrt-linux-strip
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -Os
CFLAGS += -fno-exceptions
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DNO_STACKTRACE=1
CFLAGS += -flax-vector-conversions
CFLAGS += -DNO_SWAP=1
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS := -nodefaultlibs -lc -static
#LIBS += -lstdc++
LIBS += -lsupc++
LIBS += -lgcc -lgcc_eh
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all
export LC_ALL=C

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_openwrt
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_sysvarm
include ../inc/makefile_cpu_unk
include ../inc/Makefile_linuxtest
