CC = cc
CFLAGS = -Wall -Wextra -O2 -g
CPPFLAGS = -Wall -Wextra -O2 -g -fpermissive
LDFLAGS =
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

CPP = g++
CPPFLAGS = -Wall -Wextra -02 -g

all: z80_test spectrum_test cpm_test cpmcon cpm_debug zxsdl

fm: fm.c
	$(CC) $(CFLAGS) -o fm fm.c $(LDFLAGS)

pnum: parse.num.c
	$(CC) $(CFLAGS) -o pnum parse.num.c $(LDFLAGS)

test: test.c
	$(CC) $(CFLAGS) -o test test.c $(LDFLAGS)

main: main.c
	$(CC) $(CFLAGS) -o main main.c z80.c $(LDFLAGS)

dis: dis.cpp
	$(CPP) $(CFLAGS) -o dis dis.cpp z80.dis.h $(LDFLAGS)

cli: cli.cpp
	$(CPP) $(CFLAGS) -o cli cli.cpp $(LDFLAGS)

io: io.c
	$(CC) $(CFLAGS) -o io io.c $(LDFLAGS)

str: str.cpp
	$(CPP) $(CFLAGS) -o str str.cpp $(LDFLAGS)

mep: me.cpp z80.c z80.h
	$(CPP) $(CFLAGS) -o mep me.cpp cli.h z80.c $(LDFLAGS)

me: me.c z80.c z80.h
	$(CC) $(CFLAGS) -o me me.c z80.c $(LDFLAGS)

z80_test: z80_test.c z80.c z80.h
	$(CC) $(CFLAGS) -o z80_test z80_test.c z80.c $(LDFLAGS)

spectrum_test: spectrum_test.c spectrum.c spectrum.h z80.c z80.h rom.h
	$(CC) $(CFLAGS) -o spectrum_test spectrum_test.c spectrum.c z80.c $(LDFLAGS)

cpm_test: cpm_test.c cpm.c cpm.h z80.c z80.h
	$(CC) $(CFLAGS) -o cpm_test cpm_test.c cpm.c z80.c $(LDFLAGS)

cpmcon: cpmcon.c cpm.c cpm.h z80.c z80.h
	$(CC) $(CFLAGS) -o cpmcon cpmcon.c cpm.c z80.c $(LDFLAGS)

cpm_debug: cpm_debug.c cpm.c cpm.h z80.c z80.h
	$(CC) $(CFLAGS) -o cpm_debug cpm_debug.c cpm.c z80.c $(LDFLAGS)

zxsdl: zxsdl.c spectrum.c spectrum.h z80.c z80.h rom.h tzx.c tzx.h
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -o zxsdl zxsdl.c spectrum.c z80.c tzx.c $(SDL_LIBS)

testone: z80_test spectrum_test cpm_test
	./z80_test
	./spectrum_test
	./cpm_test

fulltest: z80_test spectrum_test
	./z80_test
	./z80_test --zexdoc
	./z80_test --zexall
	./spectrum_test

clean:
	rm -rf z80_test spectrum_test cpm_test cpmcon zxsdl *.o *.dSYM

.PHONY: all test fulltest clean
