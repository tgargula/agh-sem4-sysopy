CC = gcc
CFLAGS = -Wall -Wpedantic -g -I..

DRIVER_DIR = ./src
LIB = ./src/lib/src
TARGET = ./target

target: clean ex1 ex2 ex3

ex1:
	$(CC) $(CFLAGS) $(DRIVER_DIR)/ex1.c -o $(TARGET)/ex1

ex2:
	$(CC) $(CFLAGS) $(DRIVER_DIR)/ex2.c -o $(TARGET)/ex2 -L$(shell echo $(LD_LIBRARY_PATH)) -l_merge

ex3:
	$(CC) $(CFLAGS) $(DRIVER_DIR)/ex3.c -o $(TARGET)/ex3

clean:
	rm -rf $(TARGET)/* || true