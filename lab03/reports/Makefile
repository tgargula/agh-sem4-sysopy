EXEC = nice -n 19 ../target/ex2

RESOURCES = ../resources
REPORT = report.txt

SHORT = $(RESOURCES)/short3.txt:$(RESOURCES)/short4.txt
MEDIUM = $(RESOURCES)/medium3.txt:$(RESOURCES)/medium4.txt
BIG = $(RESOURCES)/big1.txt:$(RESOURCES)/big2.txt

SHORT10 = $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT) $(SHORT)
MEDIUM10 = $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) $(MEDIUM) 
BIG10 = $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG) $(BIG)

SHORT100 = $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10) $(SHORT10)
MEDIUM100 = $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10) $(MEDIUM10)

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
	($(EXEC) $(SHORT)) >>$(REPORT)
	@echo >>$(REPORT)

oneMedium:
	@echo "Jedna para – średnie pliki:" >>$(REPORT)
	($(EXEC) $(MEDIUM)) >>$(REPORT)
	@echo >>$(REPORT)

oneBig:
	@echo "Jedna para – duże pliki:" >>$(REPORT)
	($(EXEC) $(BIG)) >>$(REPORT)
	@echo >>$(REPORT)

tenShort:
	@echo "Dziesięć par – krótkie pliki:" >>$(REPORT)
	($(EXEC) $(SHORT10)) >>$(REPORT)
	@echo >>$(REPORT)

tenMedium:
	@echo "Dziesięć par – średnie pliki:" >>$(REPORT)
	($(EXEC) $(MEDIUM10)) >>$(REPORT)
	@echo >>$(REPORT)

tenBig:
	@echo "Dziesięć par – duże pliki:" >>$(REPORT)
	($(EXEC) $(BIG10)) >>$(REPORT)
	@echo >>$(REPORT)

hundredShort:
	@echo "Sto par – krótkie pliki:" >>$(REPORT)
	($(EXEC) $(SHORT100)) >>$(REPORT)
	@echo >>$(REPORT)

hundredMedium:
	@echo "Sto par – średnie pliki:" >>$(REPORT)
	($(EXEC) $(MEDIUM100)) >>$(REPORT)
	@echo >>$(REPORT)

clean:
	rm tmp/* && rmdir tmp || true 