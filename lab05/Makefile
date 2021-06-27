CC = gcc
CFLAGS = -g

CONSUMER = ./target/consumer
PRODUCER = ./target/producer

target: clean ex1 ex2 ex3

ex1:
	$(CC) $(CFLAGS) src/ex1.c -o target/ex1

ex2:
	$(CC) $(CFLAGS) src/ex2.c -o target/ex2

ex3:
	$(CC) $(CFLAGS) src/producer.c -o $(PRODUCER)
	$(CC) $(CFLAGS) src/consumer.c -o $(CONSUMER)
	$(CC) $(CFLAGS) src/test.c -o ./target/test

test:
	./target/test $(CONSUMER) $(PRODUCER)

mkfifo: cleanfifo
	mkfifo pipe1.fifo
	mkfifo pipe2.fifo
	mkfifo pipe3.fifo

clean:
	rm -rf target/* || true

cleanfifo:
	rm *.fifo