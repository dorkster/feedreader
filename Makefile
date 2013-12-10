PROJNAME=feedreader
SRCDIR=src
BUILDDIR=build
SOURCES=$(shell ls $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o, $(SOURCES))

CC=gcc
CFLAGS+=-Wall -O2 -mms-bitfields -std=c99
CFLAGS+=`pkg-config --cflags gtk+-2.0` `xml2-config --cflags` `curl-config --cflags`
LDFLAGS+=`pkg-config --libs gtk+-2.0` `xml2-config --libs` `curl-config --libs`

all: $(BUILDDIR) $(SOURCES) $(PROJNAME)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/

depend: .depend

.depend: $(SOURCES)
	@rm -f ./.depend
	@$(CC) $(CFLAGS) -MM $^ > ./.depend
	@sed -i ':x; /\\$$/ { N; s/\\\n//; tx }' ./.depend
	@sed -i 's/^/$(BUILDDIR)\//' ./.depend

-include .depend

$(PROJNAME): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(CFLAGS) -o $@

$(BUILDDIR)/%.o:
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -rf $(BUILDDIR)/ $(PROJNAME)
	@rm ./.depend

install:
	cp $(PROJNAME) /usr/bin/$(PROJNAME)
	cp feedreader.png /usr/share/pixmaps/feedreader.png

uninstall:
	rm -f /usr/bin/$(PROJNAME)
	rm -f /usr/share/pixmaps/feedreader.png

