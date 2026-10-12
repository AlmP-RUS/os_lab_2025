CFLAGS = --all --pedantic
LDFLAGS = -L. -lsuperlibrary

CC = gcc

all : server client

server: libsuperlibrary.so

client: libsuperlibrary.so

libsuperlibrary.so: superlibrary.c superlibrary.h
	$(CC) --shared -o libsuperlibrary.so superlibrary.c superlibrary.h $(CFLAGS)

.PHONY:
clean:
	rm -f server client libsuperlibrary.so