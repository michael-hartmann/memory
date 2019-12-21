CC = gcc
OPT = -O2
CFLAGS = -std=c99 -Wall -Wextra -Wno-unused-parameter -g `pkg-config --cflags gtk+-3.0`
LDFLAGS =  `pkg-config --libs gtk+-3.0` -lm
SRC = deck.c settings.c card.c memory.c
TARGET = memory

all:
	$(CC) $(CFLAGS) -O2 src/deck.c src/settings.c src/card.c src/memory.c $(LDFLAGS) -o $(TARGET)
