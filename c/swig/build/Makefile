include ../../build.mk

.PHONY: dir all clean

ifeq ($(ARCHIVE_NAME),)
ARCHIVE_NAME = swig
else
CFLAGS += -fPIC
endif

all: dir vc.i objs

objs: dir vc.i $(ODIR)/swig.o

vc.i:
	swig  -D__ANDROID__ -c++ -java -package $(JAVA_PACKAGE) -outdir $(SWIG_OUT_DIR) -o $(SDIR)/swig.cpp -I$(APIDIR)/include  $(MYDIR)/vc.i
dir:
	@if [ ! -d $(SWIG_OUT_DIR) ];then mkdir $(SWIG_OUT_DIR); fi
	@if [ ! -d $(SDIR) ];then mkdir $(SDIR); fi
	@if [ ! -d $(ODIR) ];then mkdir $(ODIR); fi

#$(ODIR)/%.o: $(SDIR)/%.cpp
#	$(CC) -c $(CFLAGS) $(LDFLAGS)  $(LIBS) $< -o $@
$(ODIR)/swig.o:$(SDIR)/swig.cpp
	$(CC) -c $(CFLAGS) $(LDFLAGS)  $(LIBS) $< -o $@
	
clean:
	rm -f $(ODIR)/*
	rm -f $(SDIR)/*
#	rm -f $(SWIG_OUT_DIR)/*