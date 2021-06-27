CC = gcc
CFLAGS = -Wall -g

compile: clean
	$(CC) $(CFLAGS) -c src/server.c -o target/server.o -D_DEFAULT_SOURCE
	$(CC) $(CFLAGS) -c src/client.c -o target/client.o -D_DEFAULT_SOURCE
	$(CC) $(CFLAGS) target/server.o -o target/server -lrt -lpthread
	$(CC) $(CFLAGS) target/client.o -o target/client -lrt -lpthread

clean:
	rm target/* || true