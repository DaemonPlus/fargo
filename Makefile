# Makefile for fargo - Licensed under GPL v3.0 - See LICENSE for more info

# Beware of dark magic...
IMAGEMAGICKVERSION=$(shell pkg-config MagickWand --modversion | grep -oE '^\s*[0-9]+')
QDM=$(shell bc <<< "`pkg-config MagickWand --cflags | sed 's/.*-DMAGICKCORE_QUANTUM_DEPTH=\([0-9]*\).*/\1/;s/^/2^/'` - 1")

CFLAGS = -c -std=c11 -Wall -Wextra -Wpedantic $(shell pkg-config MagickWand --cflags) -DIMAGEMAGICKVERSION="$(IMAGEMAGICKVERSION)" -DQUANTUM_DEPTH_MAXSIZE="$(QDM)" -O2
LDFLAGS = $(shell pkg-config MagickWand --libs)

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
