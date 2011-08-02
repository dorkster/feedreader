PROJNAME=feedreader
ifdef NOGTK
SOURCES=src/main.c
CFLAGS=-DNOGTK
GTKCFLAGS=`pkg-config --cflags glib-2.0` `xml2-config --cflags` `curl-config --cflags`
GTKLDFLAGS=`pkg-config --libs glib-2.0` `xml2-config --libs` `curl-config --libs` -lpthread
else
SOURCES=src/main.c
GTKCFLAGS=`pkg-config --cflags gtk+-2.0` `xml2-config --cflags` `curl-config --cflags`
GTKLDFLAGS=`pkg-config --libs gtk+-2.0` `xml2-config --libs` `curl-config --libs` -lpthread
endif
ifndef DESTDIR
DESTDIR=/usr/local/bin
endif

CC=gcc
CFLAGS+=$(GTKCFLAGS)
LDFLAGS+=$(GTKLDFLAGS) 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/$(PROJNAME)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p bin
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall -O2 -mms-bitfields -std=c99 -o $@

.c.o:
	$(CC) $< $ $(CFLAGS) -c -g -Wall -O2 -mms-bitfields -std=c99 -o $@

install:
	cp $(EXECUTABLE) $(DESTDIR)
	chmod 755 $(DESTDIR)/$(PROJNAME)

uninstall:
	rm -i $(DESTDIR)/$(PROJNAME)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
