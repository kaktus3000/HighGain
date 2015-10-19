CC=gcc
CFLAGS=-I. -O3 -ftree-vectorize -Wall -ffast-math -fPIC -mtune=native -march=native
LDFLAGS=-nostartfiles -nostdlib -shared

DEST = /usr/lib64/ladspa/
PLUG = HiGain.so

SOURCES = $(wildcard *.c)
OBJECTS	= $(SOURCES:.c=.o) 

all: HiGain.so

%.o: %.c
	@echo 'Building file: $<'
	$(CC) -c -o "$@" "$<" $(CFLAGS)

HiGain.so: $(OBJECTS)
	@echo 'Building target: $@'
	gcc $(LDFLAGS) -o $(PLUG) $(OBJECTS)

install: all
	@echo 'installing'
	install -d $(DEST)
	install -m 644 $(PLUG) $(DEST)

clean:
	@echo 'cleaning'
	rm -f $(OBJECTS) $(PLUG)


.PHONY: all clean
