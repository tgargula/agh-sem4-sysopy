CC := gcc
CFLAGS := -march=native -Wall -g -O2 -I.

DEF := lib
RESOURCES := ./resources

DRV := ./src/drivers
SRC_LIB := ./src/lib/src
TARGET_LIB := ./target/lib
BIN := ./target/bin
OUT := ./target/out

DRIVERS := ex1 ex2 ex3 ex4 ex5

reports: target
	$(BIN)/ex1 $(RESOURCES)/ex1/short1.txt $(RESOURCES)/ex1/short2.txt >/dev/null
	$(BIN)/ex1 $(RESOURCES)/ex1/medium1.txt $(RESOURCES)/ex1/medium2.txt >/dev/null
	$(BIN)/ex1 $(RESOURCES)/ex1/big1.txt $(RESOURCES)/ex1/big2.txt >/dev/null
	$(BIN)/ex2 T $(RESOURCES)/ex2/short.txt >/dev/null
	$(BIN)/ex2 C $(RESOURCES)/ex2/medium.txt >/dev/null
	$(BIN)/ex2 r $(RESOURCES)/ex2/big.txt >/dev/null
	$(BIN)/ex3 $(RESOURCES)/ex3/short.txt $(RESOURCES)/ex3/out/short_a.txt $(RESOURCES)/ex3/out/short_b.txt $(RESOURCES)/ex3/out/short_c.txt >/dev/null
	$(BIN)/ex3 $(RESOURCES)/ex3/medium.txt $(RESOURCES)/ex3/out/medium_a.txt $(RESOURCES)/ex3/out/medium_b.txt $(RESOURCES)/ex3/out/medium_c.txt >/dev/null
	$(BIN)/ex3 $(RESOURCES)/ex3/big.txt $(RESOURCES)/ex3/out/big_a.txt $(RESOURCES)/ex3/out/big_b.txt $(RESOURCES)/ex3/out/big_c.txt >/dev/null
	$(BIN)/ex4 $(RESOURCES)/ex2/short.txt $(RESOURCES)/ex4/out/short.txt roll spin
	$(BIN)/ex4 $(RESOURCES)/ex2/medium.txt $(RESOURCES)/ex4/out/medium.txt C Python
	$(BIN)/ex4 $(RESOURCES)/ex2/big.txt $(RESOURCES)/ex4/out/big.txt r oo
	$(BIN)/ex5 $(RESOURCES)/ex5/short.txt $(RESOURCES)/ex5/out/short.txt
	$(BIN)/ex5 $(RESOURCES)/ex5/medium.txt $(RESOURCES)/ex5/out/medium.txt
	$(BIN)/ex5 $(RESOURCES)/ex5/big.txt $(RESOURCES)/ex5/out/big.txt

target: clean archive
	bash compile_drivers.sh $(CC) $(CFLAGS) : $(DRIVERS)

archive: lib_library.o lib_benchmark.o
	ar rcs $(TARGET_LIB)/lib_library.a $(OUT)/lib_library.o $(OUT)/lib_benchmark.o

lib_library.o:
	$(CC) $(CFLAGS) -c $(SRC_LIB)/lib_library.c -o $(OUT)/lib_library.o -D$(DEF) -lm

lib_benchmark.o:
	$(CC) $(CFLAGS) -c $(SRC_LIB)/lib_benchmark.c -o $(OUT)/lib_benchmark.o -lm

clean:
	rm $(TARGET_LIB)/* $(OUT)/* $(BIN)/* || true