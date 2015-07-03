prefix?=/usr/local

CFLAGS=-O3

all: xor

indent:
	indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4  -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 xor.c

install: xor
	strip xor
	install -m 0755 xor $(prefix)/bin

clean:
	rm -f xor *~

.PHONY: indent install
