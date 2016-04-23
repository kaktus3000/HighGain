CC=gcc
CFLAGS=-I. -O3 -ftree-vectorize -Wall -ffast-math -fPIC -mtune=native -march=native -Idsp
LDFLAGS=-nostartfiles -nostdlib -shared

DEST = /usr/lib64/lv2/HighGain.lv2
PLUG = HighGain_lv2.so

SOURCES = $(wildcard lv2/*.c) $(wildcard dsp/*.c)
OBJECTS	= $(SOURCES:.c=.o) 

all: HighGain.so

%.o: %.c
	@echo 'Building file: $<'
	$(CC) -c -o "$@" "$<" $(CFLAGS)

HighGain.so: $(OBJECTS)
	@echo 'Building target: $@'
	$(CC) $(LDFLAGS) -o $(PLUG) $(OBJECTS)

install: all
	@echo 'installing'
	install -d $(DEST)
	install -m 644 $(PLUG) $(DEST)
	install -m 644 lv2/manifest/*.ttl $(DEST)

clean:
	@echo 'cleaning'
	rm -f $(OBJECTS) $(PLUG)


.PHONY: all clean
