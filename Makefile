PREFIX ?= /usr
SSN = OpenTorpedo
DATADIR = $(PREFIX)/share/OpenTorpedo
OPTDIR = /opt/OpenTorpedo
VERSION = 1.4

all:
	$(MAKE) VERSION=$(VERSION) PREFIX=$(PREFIX) OPTDIR=$(OPTDIR) DATADIR=$(DATADIR) -C src
	gzip -c OpenTorpedo.6 > OpenTorpedo.6.gz

clean:
	rm -f src/*.o $(SSN)
	rm -f OpenTorpedo.6.gz

windows:
	$(MAKE) -C src -f Makefile.windows

install: all
	mkdir -p $(PREFIX)/bin
	mkdir -p $(DATADIR)/data
	mkdir -p $(DATADIR)/images
	mkdir -p $(DATADIR)/ships
	mkdir -p $(DATADIR)/sounds
	mkdir -p $(PREFIX)/share/pixmaps
	mkdir -p $(PREFIX)/share/applications
	mkdir -p $(PREFIX)/share/man/man6/
	cp $(SSN) $(PREFIX)/bin/
	cp images/* $(DATADIR)/images
	cp data/* $(DATADIR)/data
	cp ships/* $(DATADIR)/ships
	cp sounds/* $(DATADIR)/sounds
	cp OpenTorpedo.png $(PREFIX)/share/pixmaps
	cp OpenTorpedo.desktop $(PREFIX)/share/applications
	cp OpenTorpedo.6.gz $(PREFIX)/share/man/man6

optinstall: all
	mkdir -p $(OPTDIR)
	mkdir -p $(OPTDIR)/data
	mkdir -p $(OPTDIR)/images
	mkdir -p $(OPTDIR)/ships
	mkdir -p $(OPTDIR)/sounds
	mkdir -p $(PREFIX)/share/applications
	mkdir -p $(PREFIX)/share/pixmaps
	cp $(SSN) $(OPTDIR)
	cp images/* $(OPTDIR)/images
	cp data/* $(OPTDIR)/data
	cp ships/* $(OPTDIR)/ships
	cp sounds/* $(OPTDIR)/sounds
	cp OpenTorpedo.desktop $(PREFIX)/share/applications/
	cp OpenTorpedo.png $(PREFIX)/share/pixmaps/

deinstall:
	rm -rf $(PREFIX)/bin/$(SSN)
	rm -rf $(DATADIR)
	rm -rf $(PREFIX)/share/man/man6/OpenTorpedo.6.gz
	rm -f $(PREFIX)/share/applications/OpenTorpedo.desktop
	rm -f $(PREFIX)/share/pixmaps/OpenTorpedo.png

optdeinstall:
	rm -rf $(OPTDIR)
	rm -rf $(PREFIX)/share/applications/OpenTorpedo.desktop
	rm -rf $(PREFIX)/share/pixmaps/OpenTorpedo.png

tarball: clean
	cd .. && tar czf OpenTorpedo-$(VERSION).tar.gz OpenTorpedo --exclude=.svn

zipfile: clean
	cd .. && zip -r OpenTorpedo-$(VERSION)-source.zip OpenTorpedo --exclude \*.svn\*

