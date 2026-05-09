include config.mk

CFLAGS ?= -O1 -g -Wall -Wextra -std=c99 -pedantic

ifeq ($(WINDOWS),1)
LDLIBS = -ladvapi32
override TARGET = xor.exe
else
ifneq (,$(findstring mingw,$(CC)))
LDLIBS = -ladvapi32
override TARGET = xor.exe
else
ifdef SystemRoot
LDLIBS = -ladvapi32
override TARGET = xor.exe
else
ifeq ($(OS),Windows_NT)
LDLIBS = -ladvapi32
override TARGET = xor.exe
else
LDLIBS =
endif
endif
endif
endif

LDFLAGS ?= $(LDLIBS)

CFLAGS_RELEASE = -O3 -Wall -Wextra -Werror -std=c99
LDFLAGS_RELEASE = -s $(LDLIBS)

SRC = xor.c
OBJ = ${SRC:.c=.o}

all: ${TARGET}

release:
	$(MAKE) "CFLAGS=$(CFLAGS_RELEASE)" "LDFLAGS=$(LDFLAGS_RELEASE)" WINDOWS=$(WINDOWS) $(TARGET)

${OBJ}: config.mk

%.o: %.c
	@echo CC $@
	@${CC} -c ${CFLAGS} $<

${TARGET}: ${OBJ}
	@echo LD $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

install: all
	mkdir -p $(INSTALL_PREFIX)$(PREFIX)/bin
	install -m 0755 $(TARGET) $(INSTALL_PREFIX)$(PREFIX)/bin/xor

uninstall:
	rm $(INSTALL_PREFIX)$(PREFIX)/bin/xor

clean:
	rm -f xor xor.exe $(OBJ) *~

.PHONY: all release install uninstall clean
