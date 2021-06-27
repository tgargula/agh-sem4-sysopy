CC = gcc
DIR = $(shell pwd)
CFLAGS = -march=native -Os -Wall -I$(DIR)

LIB_C = $(DIR)/src/lib/src/lib_merge.c
LIB_O = $(DIR)/target/lib/lib_merge.o
LIB_A = $(DIR)/target/lib/lib_merge.a
LIB_SO = $(DIR)/target/lib/lib_merge.so

DRV_C = $(DIR)/src/driver/merge.c
DRV_T_C = $(DIR)/src/driver/merge_t.c
DRV_STATIC_O = $(DIR)/target/out/merge.o
DRV_STATIC_T_O = $(DIR)/target/out/merge_t.o
DRV_STATIC_EX = $(DIR)/target/bin/merge1
DRV_SHARED_EX = $(DIR)/target/bin/merge2
DRV_DYN_EX = $(DIR)/target/bin/merge3
DRV_STATIC_EX_T = $(DIR)/target/bin/merge1_t

REP_DIR = $(DIR)/reports

all: clean static shared static_t dynamic

static: static.o
	$(CC) $(CFLAGS) -static -o $(DRV_STATIC_EX) $(DRV_STATIC_O) -L$(DIR)/target/lib -l_merge

static.o: lib_merge.a
	$(CC) $(CFLAGS) -c $(DRV_C) -o $(DRV_STATIC_O)

shared: shared.so
	$(CC) $(DRV_C) -I$(DIR) -L$(DIR)/target/lib -Wl,-rpath=$(DIR)/target/lib -l_merge -o $(DRV_SHARED_EX)

shared.so:
	$(CC) -fPIC -I$(DIR) -shared $(LIB_C) -o $(LIB_SO)

static_t: static_t.o
	$(CC) $(CFLAGS) -static -o $(DRV_STATIC_EX_T) $(DRV_STATIC_T_O) -L$(DIR)/target/lib -l_merge

static_t.o:
	$(CC) $(CFLAGS) -c $(DRV_T_C) -o $(DRV_STATIC_T_O)

lib_merge.a: lib_merge.o
	ar rcs $(LIB_A) $(LIB_O)

lib_merge.o:
	$(CC) $(CFLAGS) -c $(LIB_C) -o $(LIB_O)

dynamic:
	$(CC) $(CFLAGS) $(DRV_C) -ldl -o $(DRV_DYN_EX) -DDyNaMiC

clean:
	rm target/lib/* target/out/* target/bin/* || true