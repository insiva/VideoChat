include ../../build.mk

.PHONY: all o

ifeq ($(ARCHIVE_NAME),)
ARCHIVE_NAME = utils
else
CFLAGS += -fPIC
endif
	
all : o $(OBJS)
	$(AR) -r $(LDIR)/lib$(ARCHIVE_NAME).a $(OBJS)
	rm $(ODIR)/*.o

objs: o $(OBJS)

include ../../make.mk