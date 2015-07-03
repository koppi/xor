prefix?=/usr/local

all: xor

install: xor
	install -m 0755 xor $(prefix)/bin

clean:
	rm -f xor *~

.PHONY: install
