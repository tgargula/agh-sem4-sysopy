CC = gcc
CFLAGS = -g

target: clean v posix

v:
	$(CC) $(CFLAGS) src/SystemV/server.c -o target/SystemV/server
	$(CC) $(CFLAGS) src/SystemV/client.c -o target/SystemV/client

posix:
	$(CC) $(CFLAGS) src/POSIX/server.c -o target/POSIX/server -lrt
	$(CC) $(CFLAGS) src/POSIX/client.c -o target/POSIX/client -lrt

start: target
	gnome-terminal --geometry 101x26+0+0 --title="ipcs -q" -- ./ipcs.sh
	gnome-terminal --geometry 101x26-0+0 --title="Server" -- ./run.sh ./target/SystemV/server
	gnome-terminal --geometry 101x27+0-0 --title="Client 1" -- ./run.sh ./target/SystemV/client
	gnome-terminal --geometry 101x27-0-0 --title="Client 2" -- ./run.sh ./target/SystemV/client

pstart: target
	gnome-terminal --geometry 101x26+0+0 --title="Queues" -- ./mq.sh
	gnome-terminal --geometry 101x26-0+0 --title="Server" -- ./run.sh ./target/POSIX/server
	gnome-terminal --geometry 101x27+0-0 --title="Client 1" -- ./run.sh ./target/POSIX/client
	gnome-terminal --geometry 101x27-0-0 --title="Client 2" -- ./run.sh ./target/POSIX/client

clean:
	rm -rf target/* || true
	mkdir -p target/SystemV target/POSIX
