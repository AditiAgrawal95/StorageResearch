CC=gcc
CFLAGS= -g -ggdb -I.
DEPS = dmgParser.h
OBJ = DMG.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make:	$(OBJ)
	$(CC) -o DMG $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o DMG
