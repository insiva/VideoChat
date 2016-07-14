include ../../build.mk

ifeq ($(ARCHIVE_NAME),)
ARCHIVE_NAME = sim_rtp
else
CFLAGS += -fPIC
endif

.PHONY: send listen o all

all: o $(OBJS)
	$(AR) -r $(LDIR)/lib$(ARCHIVE_NAME).a $(OBJS)
	rm $(ODIR)/*.o
	
objs: o $(OBJS)

send: o $(ODIR)/send.o $(ODIR)/UdpSocket.o
	$(CC) $(ODIR)/send.o $(ODIR)/UdpSocket.o -o $(ODIR)/send
	rm $(ODIR)/*.o
recv: o $(ODIR)/recv.o $(ODIR)/UdpListener.o
	$(CC) $(ODIR)/recv.o $(ODIR)/UdpListener.o -o $(ODIR)/recv
	rm $(ODIR)/*.o

include ../../make.mk