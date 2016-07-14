include ../../build.mk

.PHONY: send listen o all


ifeq ($(ARCHIVE_NAME),)
ARCHIVE_NAME = xlib
else
CFLAGS += -fPIC
endif

all: o $(OBJS)
	$(AR) -r $(LDIR)/lib$(ARCHIVE_NAME).a $(OBJS)
	rm $(ODIR)/*.o
	
objs: o $(OBJS)

x264: o $(ODIR)/x264.o $(ODIR)/X264Encoder.o
	$(CC) $(ODIR)/x264.o $(ODIR)/X264Encoder.o  $(CFLAGS) $(LDFLAGS)  $(LIBS)  -o $(ODIR)/x264
	rm $(ODIR)/*.o

send: o $(ODIR)/send.o $(ODIR)/UdpSocket.o
	$(CC) $(ODIR)/send.o $(ODIR)/UdpSocket.o $(CFLAGS) $(LDFLAGS)  $(LIBS) -o $(ODIR)/send
	rm $(ODIR)/*.o
recv: o $(ODIR)/recv.o $(ODIR)/UdpListener.o
	$(CC) $(ODIR)/recv.o $(ODIR)/UdpListener.o $(CFLAGS) $(LDFLAGS)  $(LIBS) -o $(ODIR)/recv
	rm $(ODIR)/*.o

thread: o $(ODIR)/thread.o $(ODIR)/XThread.o  $(ODIR)/XMessage.o
	$(CC) $(ODIR)/thread.o $(ODIR)/XThread.o $(ODIR)/XMessage.o $(CFLAGS) $(LDFLAGS)  $(LIBS) -o $(ODIR)/thread
	rm $(ODIR)/*.o

include ../../make.mk