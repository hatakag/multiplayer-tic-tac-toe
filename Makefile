CC=gcc
CFLAGS=-Wall -std=gnu99
#LIBS=-pthread
SRC=src

all: server.out client.out
server.out: server.o Tic-Tac-Toe_Lib.o
	$(CC) $(CFLAGS) $(LIBS) -o server.out server.o Tic-Tac-Toe_Lib.o
client.out: client.o
	$(CC) $(CFLAGS) $(LIBS) -o client.out client.o
server.o: $(SRC)/server.c
	$(CC) $(CFLAGS) -c $(SRC)/server.c
Tic-Tac-Toe_Lib.o: $(SRC)/Tic-Tac-Toe_Lib.c
	$(CC) $(CFLAGS) -c $(SRC)/Tic-Tac-Toe_Lib.c
client.o: $(SRC)/client.c
	$(CC) $(CFLAGS) -c $(SRC)/client.c

.PHONY: clean
clean:
	rm -f *.o *.out
