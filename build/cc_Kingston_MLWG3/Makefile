STAGING_DIR:=~/openwrt/OpenWrt-SDK-15.05.1-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/
TOOLCHAIN_DIR:=~/openwrt/OpenWrt-SDK-15.05.1-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
#STAGING_DIR:=~/openwrt/OpenWrt-SDK-15.05-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/OpenWrt-SDK-15.05-ramips-mt7628_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin
CC:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-g++
CC2:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-gcc
ASM:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-as
LD:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-ld
STRIP:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-strip

#STAGING_DIR:=~/openwrt/openwrt-sdk-18.06.0-ramips-mt76x8_gcc-7.3.0_musl.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/openwrt-sdk-18.06.0-ramips-mt76x8_gcc-7.3.0_musl.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin
#STAGING_DIR:=~/openwrt/lede-sdk-17.01.4-ramips-mt7688_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/lede-sdk-17.01.4-ramips-mt7688_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin
#STAGING_DIR:=~/openwrt/openwrt-sdk-ramips-mt76x8_gcc-7.3.0_musl.Linux-x86_64/staging_dir/
#TOOLCHAIN_DIR:=~/openwrt/openwrt-sdk-ramips-mt76x8_gcc-7.3.0_musl.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin
#CC:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-g++
#CC2:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-gcc
#ASM:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-as
#LD:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-ld
#STRIP:=$(TOOLCHAIN_DIR)/mipsel-openwrt-linux-musl-strip
export STAGING_DIR
CLASSDIR:=../../

CFLAGS += -I ../../header
CFLAGS += -Os
CFLAGS += -fno-exceptions
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DNO_STACKTRACE=1
CFLAGS += -DNO_GETIFADDRS
CFLAGS += -flax-vector-conversions
CFLAGS += -fno-rtti
ASMFLAGS :=
CSUFFIX :=
ASMSUFFIX :=
CLASS_OBJS :=
LIBS := 
#LIBS += -v
LIBS += -Wl,--gc-sections
LIBS += -Wl,--strip-all
#LIBS += -static-libstdc++
#LIBS += -static
#LIBS += -static-libgcc
LIBS += -nodefaultlibs
LIBS += -lc
#LIBS += -lc_so
LIBS += -lgcc
#LIBS += -lsupc++
LIBS += -lm
#LIBS += -ldl
#LIBS += -lnsl
#LIBS += -lutil
#LIBS += -lthread_db
#LIBS += -lpthread_nonshared
#LIBS += -latomic
#LIBS += -lcrypt
#LIBS += -l :uclibc_nonshared.a
#LIBS += -lstdc++
#LIBS += -l :libc.a
#LIBS += -l :libstdc++.a
#LIBS += -l :libuClibc-0.9.33.2.so
#LIBS += -l :ld-uClibc.so.0
#LIBS += -l :crt1.o
ENVOBJS := uClibcDeps.o
export LC_ALL=C

include ../inc/makefile_common
include ../inc/makefile_os_linux_nobt
include ../inc/makefile_dev_kingston_mlw
include ../inc/makefile_ui_unk
include ../inc/makefile_cpua_mips
include ../inc/makefile_cpu_mt7628
include ../inc/Makefile_linuxtest
