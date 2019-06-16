VERSION = 0.0.1

# Customize below to fit your system

TARGET = xor
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

INCS = -I. -I/usr/include
LIBS = -L/usr/lib -lc

CFLAGS = -static -Os -g -Wall -Werror ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS = -g ${LIBS}

CC = cc
