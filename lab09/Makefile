CC = gcc
CFLAGS = -g

compile: clean
	$(CC) $(CFLAGS) src/main.c -o target/main -pthread

run:
	@./target/main

clean:
	rm -f target/* || true