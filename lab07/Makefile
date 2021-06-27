CC = gcc
CFLAGS = -O2 -Wall -Wpedantic

compile: clean
	${CC} ${CFLAGS} src/SystemV/pizzeria.c -o target/SystemV/pizzeria
	${CC} ${CFLAGS} src/SystemV/supplier.c -o target/SystemV/supplier
	${CC} ${CFLAGS} src/SystemV/cook.c -o target/SystemV/cook
	${CC} ${CFLAGS} src/POSIX/pizzeria.c -o target/POSIX/pizzeria -lrt -lpthread
	${CC} ${CFLAGS} src/POSIX/supplier.c -o target/POSIX/supplier -lrt -lpthread
	${CC} ${CFLAGS} src/POSIX/cook.c -o target/POSIX/cook -lrt -lpthread

clean:
	rm target/POSIX/* target/SystemV/* || true
	