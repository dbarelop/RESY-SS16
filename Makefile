CC=gcc
MAKE=make -f
TARGET=logic
SOURCE=$(TARGET).c
CFLAGS= -Wall -pedantic -Werror -g
MOTORH= -I ./motorh
TRACKH= -I ./track

all:
	$(CC) $(MOTORH) $(TRACKH) $(SOURCE) -o $(TARGET) $(CFLAGS)

clean:
	rm $(TARGET)

