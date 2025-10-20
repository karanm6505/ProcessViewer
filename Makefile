CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lncurses

all: mini_htop

mini_htop: main.o proc.o
	$(CC) $(CFLAGS) -o mini_htop main.o proc.o $(LDFLAGS)

main.o: main.c proc.h
	$(CC) $(CFLAGS) -c main.c

proc.o: proc.c proc.h
	$(CC) $(CFLAGS) -c proc.c

clean:
	rm -f *.o mini_htop

