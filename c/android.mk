ANDROID_NDK := $(shell echo $$ANDROID_NDK)
GCC_VERSION := 4.6
ABI := armeabi
TARGET_PLATFORM := android-9
TARGET_ARCH := arch-arm


CROSS_PREFIX := $(ANDROID_NDK)/toolchains/arm-linux-androideabi-$(GCC_VERSION)/prebuilt/linux-x86_64/bin/arm-linux-androideabi-

LIBDIR :=$(VCDIR)/libs/android

CC := $(CROSS_PREFIX)g++
AR := $(CROSS_PREFIX)ar
STRIP := $(CROSS_PREFIX)strip

LDFLAGS += -L$(VCDIR)/libs/android

SYS_ROOT := $(ANDROID_NDK)/platforms/$(TARGET_PLATFORM)/$(TARGET_ARCH)
CFLAGS += --sysroot=$(SYS_ROOT)

STL_PATH := $(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/$(GCC_VERSION)
CFLAGS += -I $(SWIGDIR)/src
CFLAGS += -I $(ANDROIDDIR)/include
CFLAGS += -I $(STL_PATH)/include
CFLAGS += -I $(STL_PATH)/libs/$(ABI)/include
LDFLAGS += -L$(STL_PATH)/libs/$(ABI)
CFLAGS +=  -fexceptions


LIBS += -llog
LIBS += -lGLESv2
#LIBS += -lgnustl_shared
LIBS += -lgnustl_static
LIBS += -lstdc++
LIBS += -fuse-ld=bfd
