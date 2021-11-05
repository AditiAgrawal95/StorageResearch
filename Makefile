CC=gcc
INCLUDES= -I/usr/include/libxml2 -lxml2 -lz
CFLAGS= -g -ggdb -Wno-format $(INCLUDES)
DEPS = dmgParser.h apfs.h
OBJ = DMG.o base64.o apfs.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make:	$(OBJ)
	$(CC) -o DMG $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o DMG decompressed* *.txt
