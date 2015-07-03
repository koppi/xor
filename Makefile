prefix?=/usr/local

CFLAGS=-O3

all: xor

indent:
	indent xor.c

install: xor
	strip xor
	install -m 0755 xor $(prefix)/bin

clean:
	rm -f xor *~

.PHONY: indent install
