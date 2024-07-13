CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wno-unused-parameter -Wno-unused-function -g -O0
LDFLAGS = 

DESTDIR ?= 
PREFIX ?= /usr

TARGET0 = cbuilder
OBJECT0 = $(SOURCE0:%.c=%.c.o)
SOURCE0 = cbuilder.c
TARGET1 = genmake
OBJECT1 = $(SOURCE1:%.c=%.c.o)
SOURCE1 = genmake.c

all: $(TARGET0) $(TARGET1)

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET0) $(TARGET1)

dist: distclean
	tar cvf ../$(DIRNAME)-latest.txz ../$(DIRNAME)

install: $(TARGET0) $(TARGET1)
	cp $(TARGET0) $(TARGET1) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(TARGET0) $(TARGET1)

$(TARGET0): $(OBJECT0)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)



$(TARGET1): $(OBJECT1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

