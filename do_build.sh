#!/bin/sh

ARCH=aarch64-poky-linux
BUILD_ARCH=x86_64-pokysdk-linux
SDK_BASE=/opt/poky/2.1.3
SYSROOT_BASE=${SDK_BASE}/sysroots
SYSROOT=${SYSROOT_BASE}/${ARCH}
BUILD_SYSROOT=${SYSROOT_BASE}/${BUILD_ARCH}
PREFIX=/usr

ARCH_ARGS="--sysroot=${SYSROOT} -march=armv8-a -mtune=cortex-a57.cortex-a53 -mcpu=cortex-a57.cortex-a53+simd+fp"
DBG_ARGS="-g -ggdb"
CMN_ARGS="${ARCH_ARGS} ${DBG_ARGS} -pipe -fPIE -pie"

PATH=${PATH}:${BUILD_SYSROOT}/usr/bin/${ARCH}; export PATH
CC="${ARCH}-gcc ${ARCH_ARGS} "; export CC
CXX="${ARCH}-g++ ${ARCH_ARGS} "; export CXX
CFLAGS="${CMN_ARGS}"; export CFLAGS
CXXFLAGS="${CMN_ARGS}"; export CXXFLAGS
LDFLAGS="${CMN_ARGS} ${LD_OPT_ARGS} -Wl,--hash-style=gnu -Wl,--as-needed "; export LDFLAGS

. /opt/poky/2.1.3/environment-setup-aarch64-poky-linux

./configure \
	--prefix=${PREFIX}	\
	--target=${ARCH}	\
	--build=${BUILD_ARCH}	\
	--host=${ARCH}	\
  || exit 1

make -j4
