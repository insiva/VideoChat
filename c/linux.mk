CC := g++
AR := ar
STRIP := strip

LIBDIR :=$(VCDIR)/libs/linux

LDFLAGS += -L$(LIBDIR)

LIBS += -lpthread

JAVA_HOME := $(shell echo $$JAVA_HOME)
CFLAGS += -I $(JAVA_HOME)/include
CFLAGS += -I $(JAVA_HOME)/include/linux