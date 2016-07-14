include ../../build.mk

.PHONY: all o


ifeq ($(ARCHIVE_NAME),)
ARCHIVE_NAME = api
else
CFLAGS += -fPIC
endif

all : o $(OBJS)
	$(AR) -r $(LDIR)/lib$(ARCHIVE_NAME).a $(OBJS)
	rm $(ODIR)/*.o

objs: o $(OBJS)

include ../../make.mk