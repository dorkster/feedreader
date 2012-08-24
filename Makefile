PROJNAME=feedreader

CC=gcc
CFLAGS+=-Wall -O2 -mms-bitfields -std=c99
CFLAGS+=`pkg-config --cflags gtk+-2.0` `xml2-config --cflags` `curl-config --cflags`
LDFLAGS+=`pkg-config --libs gtk+-2.0` `xml2-config --libs` `curl-config --libs`
OBJECTS=build/download.o build/feedlist.o build/gui.o build/parse.o build/util.o build/main.o
EXECUTABLE=$(PROJNAME)

all: build $(OBJECTS) $(EXECUTABLE)

build:
	mkdir -p build

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall -O2 -mms-bitfields -std=c99 -o $@

build/download.o : src/download.c src/download.h src/util.h
	$(CC) src/download.c -c $(CFLAGS) -o $@

build/feedlist.o : src/feedlist.c src/feedlist.h src/util.h
	$(CC) src/feedlist.c -c $(CFLAGS) -o $@

build/gui.o : src/gui.c src/download.h src/feedlist.h src/gui.h src/parse.h src/util.h
	$(CC) src/gui.c -c $(CFLAGS) -o $@

build/parse.o : src/parse.c src/parse.h src/feedlist.h src/util.h
	$(CC) src/parse.c -c $(CFLAGS) -o $@

build/util.o : src/util.c src/util.h
	$(CC) src/util.c -c $(CFLAGS) -o $@

build/main.o : src/main.c src/gui.h src/util.h
	$(CC) src/main.c -c $(CFLAGS) -o $@

install:
	cp $(EXECUTABLE) /usr/bin/$(EXECUTABLE)

uninstall:
	rm -f /usr/bin/$(EXECUTABLE)

clean:
	rm -rf build/ $(EXECUTABLE)
