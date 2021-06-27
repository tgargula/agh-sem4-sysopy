EXEC = ../target/bin/merge1_t
EXEC_FULL = ../target/bin/merge1

RESOURCES = ../resources
REPORT = raport2.txt

SHORT = $(RESOURCES)/short3.txt:$(RESOURCES)/short4.txt
MEDIUM = $(RESOURCES)/medium3.txt:$(RESOURCES)/medium4.txt
BIG = $(RESOURCES)/big1.txt:$(RESOURCES)/big2.txt

SHORT10 = $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT)
MEDIUM10 = $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) 
BIG10 = $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG)

SHORT100 = $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10)
MEDIUM100 = $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10)

ADD_REMOVE = add_blocks remove_blocks
ADD_REMOVE10 = $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE) $(ADD_REMOVE)

ONE = $(EXEC) 1 create_table 1
FULL = $(EXEC_FULL) 1 create_table 1
TEN = $(EXEC) 10 create_table 10
FULL_TEN = $(EXEC_FULL) 10 create_table 10
HUNDRED = $(EXEC) 100 create_table 100
FULL_HUNDRED = $(EXEC_FULL) 100 create_table 100

all: clean
	make oneShort
	make oneMedium
	make oneBig
	make tenShort
	make tenMedium
	make tenBig
	make hundredShort
	make hundredMedium

oneShort:
	@echo "Jedna para – krótkie pliki:" >>$(REPORT)
	($(ONE) merge_files $(SHORT) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

oneMedium:
	@echo "Jedna para – średnie pliki:" >>$(REPORT)
	($(ONE) merge_files $(MEDIUM) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

oneBig:
	@echo "Jedna para – duże pliki:" >>$(REPORT)
	($(ONE) merge_files $(BIG) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

tenShort:
	@echo "Dziesięć par – krótkie pliki:" >>$(REPORT)
	($(TEN) merge_files $(SHORT10) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

tenMedium:
	@echo "Dziesięć par – średnie pliki:" >>$(REPORT)
	($(TEN) merge_files $(MEDIUM10) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

tenBig:
	@echo "Dziesięć par – duże pliki:" >>$(REPORT)
	($(TEN) merge_files $(BIG10) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

hundredShort:
	@echo "Sto par – krótkie pliki:" >>$(REPORT)
	($(HUNDRED) merge_files $(SHORT100) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

hundredMedium:
	@echo "Sto par – średnie pliki:" >>$(REPORT)
	($(HUNDRED) merge_files $(MEDIUM100) $(ADD_REMOVE)) >>$(REPORT)
	@echo "10 razy dodanie i usunięcie naprzemiennie:" >>$(REPORT)
	($(FULL) $(ADD_REMOVE10)) >>$(REPORT)
	@echo >>$(REPORT)

clean:
	rm tmp/* && rmdir tmp || true 