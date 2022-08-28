CC := gcc
CFLAGS := -std=c89 -pedantic -Wall -Wextra -g3 -O0
LDFLAGS :=

all: kiss_unpack

kiss_unpack: kiss_unpack.c cp932.c cp932table.c win32compat.c
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

#kiss_repack: kiss_repack.c
#	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

clean:
	rm -f kiss_unpack kiss_unpack.exe kiss_repack kiss_repack.exe *.o


.PHONY: clean
