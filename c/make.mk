o:
	@if [ ! -d $(ODIR) ];then mkdir $(ODIR); fi
	@if [ ! -d $(LDIR) ];then mkdir $(LDIR); fi

$(ODIR)/%.o: $(TDIR)/%.cpp
	$(CC) -c $(CFLAGS) $(LDFLAGS)  $(LIBS) $< -o $@

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c $(CFLAGS) $(LDFLAGS)  $(LIBS) $< -o $@
	
clean:
	rm -f $(ODIR)/*
	rm -f $(LDIR)/*