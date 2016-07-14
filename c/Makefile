include build.mk

ODIR := $(VCDIR)/output
TDIR := $(VCDIR)/test

#SUBDIRS := $(MEDIADIR)
SUBDIRS := $(SWIGDIR) $(APIDIR) $(SRDIR) $(XLIBSDIR) $(MEDIADIR) $(UTILSDIR)

.PHONY: o all clean

LDFLAGS += $(foreach n,$(SUBDIRS),-L$(n)/libs)
#LIBS := $(foreach n,$(SUBDIRS),-l$(notdir $(n))) $(LIBS)
OBJS := $(foreach n,$(SUBDIRS),$(n)/output/*.o)
ifeq ($(ANDROID),1)
SO_PATH:=$(ANDROID_PROJECT_DIR)/libs/armeabi/libVideoChat.so
V7A_SO_PATH:=$(ANDROID_PROJECT_DIR)/libs/armeabi-v7a/libVideoChat.so
else
SO_PATH:=$(ODIR)/libVideoChat.so
endif

all:libVideoChat.so

test: o subdirs m1 m2
#	$(CC) $(ODIR)/main.o $(OBJS) -o $(ODIR)/main $(CFLAGS) $(LDFLAGS)  $(LIBS)
	
m1: $(ODIR)/m1.o
	$(CC) $(ODIR)/m1.o $(OBJS) -o $(ODIR)/m1 $(CFLAGS) $(LDFLAGS)  $(LIBS)
	
m2: $(ODIR)/m2.o
	$(CC) $(ODIR)/m2.o $(OBJS) -o $(ODIR)/m2 $(CFLAGS) $(LDFLAGS)  $(LIBS)
	
test2: libVideoChat.so m11 m22

m11: $(ODIR)/m1.o
	$(CC) $(ODIR)/m1.o -o $(ODIR)/m1 $(CFLAGS) $(LDFLAGS) -L$(ODIR)  $(LIBS) -lVideoChat
	
m22: $(ODIR)/m2.o
	$(CC) $(ODIR)/m2.o -o $(ODIR)/m2 $(CFLAGS) $(LDFLAGS) -L$(ODIR)  $(LIBS) -lVideoChat
	
libVideoChat.so: o subdirs
	$(CC) $(OBJS) -o $(SO_PATH)  $(CFLAGS) -fPIC -shared -Wl,--whole-archive $(LDFLAGS)  $(LIBS) -Wl,--no-whole-archive
ifneq ('$(DEBUG)','1')
	$(STRIP) $(SO_PATH)
endif
ifeq ('$(ANDROID)','1')
	cp $(SO_PATH) $(V7A_SO_PATH)
endif

subdirs:
	for subdir in $(SUBDIRS); do \
	( $(MAKE) objs -C $$subdir/build ARCHIVE_NAME=$${subdir##*/}) || exit 1; \
	done

o:
	@if [ ! -d $(ODIR) ];then mkdir $(ODIR); fi
	
$(ODIR)/%.o: $(TDIR)/%.cpp
	$(CC) -c $(CFLAGS) $(LDFLAGS)  $(LIBS) $< -o $@
	
clean:
	for subdir in $(SUBDIRS); do \
	( $(MAKE) clean -C $$subdir/build) || exit 1; \
	done
	rm -f $(ODIR)/*
	rm -f $(SO_PATH)
ifeq ('$(ANDROID)','1')
	rm -f $(V7A_SO_PATH)
endif
