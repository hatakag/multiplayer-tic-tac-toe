CC=gcc
CFLAGS=-Wall -std=gnu99
LIBS=-pthread
SRC=src

all: server.out client.out test_client.out
server.out: server.o Tic-Tac-Toe_Lib.o
	$(CC) $(CFLAGS) -o server.out server.o Tic-Tac-Toe_Lib.o $(LIBS)
client.out: client.o
	$(CC) $(CFLAGS) -o client.out client.o Tic-Tac-Toe_Lib.o $(LIBS)
test_client.out: test_client.o
	$(CC) $(CFLAGS) -o test_client.out test_client.o $(LIBS)
server.o: $(SRC)/server.c
	$(CC) $(CFLAGS) -c $(SRC)/server.c
Tic-Tac-Toe_Lib.o: $(SRC)/Tic-Tac-Toe_Lib.c
	$(CC) $(CFLAGS) -c $(SRC)/Tic-Tac-Toe_Lib.c
client.o: $(SRC)/client.c
	$(CC) $(CFLAGS) -c $(SRC)/client.c
test_client.o: $(SRC)/test_client.c
	$(CC) $(CFLAGS) -c $(SRC)/test_client.c

.PHONY: clean
clean:
	rm -f *.o *.out
