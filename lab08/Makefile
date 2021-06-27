CC = gcc
CFLAGS = -Wall -Wpedantic -O2
REPORT = times.txt

compile: clean
	$(CC) $(CFLAGS) src/negate.c -o target/negate -pthread -lm

run: compile
	bash run.sh | tee -a $(REPORT)

clean:
	rm -rf target/* || true
	rm -f times.txt || true
