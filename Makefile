# Makefile for fargo - Licensed under GPL v3.0 - See LICENSE for more info

CFLAGS = -c -std=c11 -Wall -Wextra -Wpedantic $(shell pkg-config vips --cflags)
LDFLAGS = $(shell pkg-config vips --libs)

SOURCES = src/main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = fargo


all: $(SOURCES) $(EXECUTABLE) doc

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

doc:
	pod2man -u --release="fargo" --name="fargo" --center=" " fargo.1.pod > fargo.1

install:
	install -m755 -D fargo $(DESTDIR)/usr/bin/fargo
	install -m644 -D fargo.1 $(DESTDIR)/usr/share/man/man1/fargo.1

remove:
	rm $(DESTDIR)/usr/bin/fargo
	rm $(DESTDIR)/usr/share/man/man1/fargo.1

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) fargo.1
	rm -f core.* vgcore.* callgrind.out.*
