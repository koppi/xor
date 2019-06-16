include config.mk

SRC = xor.c
OBJ = ${SRC:.c=.o}

all: options ${TARGET}

options:
	@echo ${TARGET} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

${OBJ}: config.mk

%.o: %.c
	@echo CC $@
	@${CC} -c ${CFLAGS} $<

${TARGET}: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

install: all
	strip xor
	mkdir -p $(INSTALL_PREFIX)$(PREFIX)/bin
	install -m 0755 xor $(INSTALL_PREFIX)$(PREFIX)/bin

uninstall:
	rm $(INSTALL_PREFIX)$(PREFIX)/bin/xor

test:
	TRAVIS_DEBIAN_DISTRIBUTION=sid; wget -O- http://travis.debian.net/script.sh | sh -

clean:
	rm -f xor *~

.PHONY: all options clean install uninstall test
