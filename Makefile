include config.mk

CFLAGS ?= -O1 -g -Wall -Wextra -std=c99 -pedantic -fsanitize=address,undefined
LDFLAGS ?= -fsanitize=address,undefined

CFLAGS_RELEASE = -O2 -Wall -Wextra -std=c99
LDFLAGS_RELEASE = -s

SRC = xor.c
OBJ = ${SRC:.c=.o}

all: ${TARGET}

release:
	$(MAKE) "CFLAGS=$(CFLAGS_RELEASE)" "LDFLAGS=$(LDFLAGS_RELEASE)" $(TARGET)

${OBJ}: config.mk

%.o: %.c
	@echo CC $@
	@${CC} -c ${CFLAGS} $<

${TARGET}: ${OBJ}
	@echo LD $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

install: all
	strip xor
	mkdir -p $(INSTALL_PREFIX)$(PREFIX)/bin
	install -m 0755 xor $(INSTALL_PREFIX)$(PREFIX)/bin

uninstall:
	rm $(INSTALL_PREFIX)$(PREFIX)/bin/xor

clean:
	rm -f xor $(OBJ) *~

.PHONY: all release install uninstall clean
