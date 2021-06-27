CC = gcc
CFLAGS = -g -Wall -Wpedantic

SRC = ./src
TARGET = ./target

target: clean ex1 ex2 ex3

ex1:
	$(CC) $(CFLAGS) $(SRC)/ex1.c -o $(TARGET)/ex1

ex2:
	$(CC) $(CFLAGS) $(SRC)/ex2.c -o $(TARGET)/ex2

ex3:
	$(CC) $(CFLAGS) $(SRC)/ex3/a/catcher.c -o $(TARGET)/ex3/a/catcher
	$(CC) $(CFLAGS) $(SRC)/ex3/a/sender.c -o $(TARGET)/ex3/a/sender
	$(CC) $(CFLAGS) $(SRC)/ex3/b/catcher.c -o $(TARGET)/ex3/b/catcher
	$(CC) $(CFLAGS) $(SRC)/ex3/b/sender.c -o $(TARGET)/ex3/b/sender

clean:
	rm $(TARGET)/*.o || true