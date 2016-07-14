include ../../build.mk

.PHONY: send listen o all

ifeq ($(ARCHIVE_NAME),)
ARCHIVE_NAME = media
else
CFLAGS += -fPIC
endif



all: o $(OBJS)
	$(AR) -r $(LDIR)/lib$(ARCHIVE_NAME).a $(OBJS)
	rm $(ODIR)/*.o

objs: o $(OBJS)

x264: o $(ODIR)/x264.o $(ODIR)/H264Encoder.o $(ODIR)/H264Decoder.o
	$(CC) $(ODIR)/x264.o $(ODIR)/H264Encoder.o $(ODIR)/H264Decoder.o  $(CFLAGS) $(LDFLAGS)  $(LIBS)  -o $(ODIR)/x264
	rm $(ODIR)/*.o

include ../../make.mk