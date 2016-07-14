ANDROID := 1
DEBUG := 0

ANDROID_PROJECT_DIR := /home/zzh/workspace/VideoChat/android

VCDIR :=/home/zzh/workspace/VideoChat/c
UTILSDIR :=$(VCDIR)/utils
XLIBSDIR :=$(VCDIR)/xlib
MEDIADIR :=$(VCDIR)/media
SRDIR :=$(VCDIR)/sim_rtp
APIDIR :=$(VCDIR)/api
SWIGDIR := $(VCDIR)/swig

MYDIR := $(CURDIR)/..
ODIR :=$(MYDIR)/output
LDIR :=$(MYDIR)/libs
SDIR := $(MYDIR)/src
TDIR := $(MYDIR)/test

CFLAGS :=
LDFLAGS := 
LIBS :=
IFLAGS :=

#CFLAGS += -fPIC
CFLAGS += -I $(VCDIR)/include
CFLAGS += -I $(UTILSDIR)/include
CFLAGS += -I $(XLIBSDIR)/include
CFLAGS += -I $(MEDIADIR)/include
CFLAGS += -I $(SRDIR)/include
CFLAGS += -I $(APIDIR)/include
CFLAGS += -I $(SWIGDIR)/src

CFLAGS += -std=c++11

LIBS += -lavcodec
LIBS += -lavformat
LIBS += -lavutil
LIBS += -lswscale
LIBS += -lz
LIBS += -lswresample
#LIBS += -llzma

LIBS += -lx264

LIBS += -ldl

CFLAGS += -Wall

ifeq ('$(DEBUG)','1')
CFLAGS += -g
else
CFLAGS += -O3
endif

ifeq ($(ANDROID),1)
include $(VCDIR)/android.mk
else
include $(VCDIR)/linux.mk
endif


OBJS := $(subst /src/,/output/,$(patsubst %.cpp,%.o,$(wildcard $(SDIR)/*.cpp)))

FLAGS := $(CFLAGS) $(LDFLAGS) $(LIBS)

JAVA_PACKAGE := com.matteo.vc.jni
SWIG_OUT_DIR := $(ANDROID_PROJECT_DIR)/src/com/matteo/vc/jni