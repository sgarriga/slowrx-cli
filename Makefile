CC = gcc

CFLAGS    = -Wall -Wextra -std=gnu99 -pedantic -g 

OFLAGS = -O3

OBJECTS = common.o modespec.o vis.o fsk.o slowrx-cli.o wav.o image.o bmp.o

all: slowrx-cli

slowrx-cli: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lfftw3 -lm -lpthread

%.o: %.c common.h wav.h bmp.h
	$(CC) $(CFLAGS) $(OFLAGS) -c -o $@ $<

clean:
	rm -f slowrx $(OBJECTS)
