prefix?=/usr/local

all: xor

indent:
	indent xor.c

install: xor
	install -m 0755 xor $(prefix)/bin

clean:
	rm -f xor *~

.PHONY: indent install
