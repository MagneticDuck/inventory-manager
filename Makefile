TARGET = prog
LIBS = -L ~/.nix-profile/lib -lm -lncurses
CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I /nix/store/sxpjs9wdw3i845h4cpswdywqs0p86nn7-ncurses-6.0-dev/include

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h) 

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
