CC=gcc

switch: raspberry_gpio.o switch.o
	$(CC) raspberry_gpio.o switch.o -o switch

led5: raspberry_gpio.o led5.o
	$(CC) -lrt -pthread raspberry_gpio.o led5.o -o led5

raspberry_gpio.o: raspberry_gpio.c
	$(CC) -c raspberry_gpio.c

switch.o: switch.c
	$(CC) -c switch.c

led5.o: led5.c
	$(CC) -c led5.c

clean:
	rm -f *.o switch led5

all: switch led5
