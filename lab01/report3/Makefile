EXEC1 = ../target/bin/merge1
EXEC2 = ../target/bin/merge2
EXEC3 = ../target/bin/merge3

RESOURCES = ../resources
REPORT = raport3b.txt

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

ONEEND = 1 create_table 1
TENEND = 10 create_table 10
HUNDREDEND = 100 create_table 100

ONE1 = $(EXEC1) $(ONEEND)
ONE2 = $(EXEC2) $(ONEEND)
ONE3 = $(EXEC3) $(ONEEND)
TEN1 = $(EXEC1) $(TENEND)
TEN2 = $(EXEC2) $(TENEND)
TEN3 = $(EXEC3) $(TENEND)
HUNDRED1 = $(EXEC1) $(HUNDREDEND)
HUNDRED2 = $(EXEC2) $(HUNDREDEND)
HUNDRED3 = $(EXEC3) $(HUNDREDEND)

all: clean
	make oneShort
	make oneMedium
	make oneBig
	make tenShort
	make tenMedium
	make tenBig
	make hundredShort
	make hundredMedium

static:
	@echo "Statyczna:" >>$(REPORT)

shared:
	@echo "Dzielona:" >>$(REPORT)

dynamic:
	@echo "Dynamiczna:" >>$(REPORT)

endl:
	@echo >>$(REPORT)

oneShort:
	@echo "Jedna para – krótkie pliki:" >>$(REPORT)
	make static oneShortStatic endl
	make shared oneShortShared endl
	make dynamic oneShortDynamic endl

oneShortStatic:
	($(ONE1) merge_files $(SHORT) $(ADD_REMOVE)) >>$(REPORT)

oneShortShared:
	($(ONE2) merge_files $(SHORT) $(ADD_REMOVE)) >>$(REPORT)

oneShortDynamic:
	($(ONE3) merge_files $(SHORT) $(ADD_REMOVE)) >>$(REPORT)

oneMedium:
	@echo "Jedna para – średnie pliki:" >>$(REPORT)
	make static oneMediumStatic endl
	make shared oneMediumShared endl
	make dynamic oneMediumDynamic endl
	
oneMediumStatic:
	($(ONE1) merge_files $(MEDIUM) $(ADD_REMOVE)) >>$(REPORT)

oneMediumShared:
	($(ONE2) merge_files $(MEDIUM) $(ADD_REMOVE)) >>$(REPORT)

oneMediumDynamic:
	($(ONE3) merge_files $(MEDIUM) $(ADD_REMOVE)) >>$(REPORT)

oneBig:
	@echo "Jedna para – duże pliki:" >>$(REPORT)
	make static oneBigStatic endl
	make shared oneBigShared endl
	make dynamic oneBigDynamic endl

oneBigStatic:
	($(ONE1) merge_files $(BIG) $(ADD_REMOVE)) >>$(REPORT)

oneBigShared:
	($(ONE2) merge_files $(BIG) $(ADD_REMOVE)) >>$(REPORT)

oneBigDynamic:
	($(ONE3) merge_files $(BIG) $(ADD_REMOVE)) >>$(REPORT)

tenShort:
	@echo "Dziesięć par – krótkie pliki:" >>$(REPORT)
	make static tenShortStatic endl
	make shared tenShortShared endl
	make dynamic tenShortDynamic endl

tenShortStatic:
	($(ONE1) merge_files $(SHORT10) $(ADD_REMOVE)) >>$(REPORT)

tenShortShared:
	($(ONE2) merge_files $(SHORT10) $(ADD_REMOVE)) >>$(REPORT)

tenShortDynamic:
	($(ONE3) merge_files $(SHORT10) $(ADD_REMOVE)) >>$(REPORT)

tenMedium:
	@echo "Dziesięć par – średnie pliki:" >>$(REPORT)
	make static tenMediumStatic endl
	make shared tenMediumShared endl
	make dynamic tenMediumDynamic endl

tenMediumStatic:
	($(ONE1) merge_files $(MEDIUM10) $(ADD_REMOVE)) >>$(REPORT)

tenMediumShared:
	($(ONE2) merge_files $(MEDIUM10) $(ADD_REMOVE)) >>$(REPORT)

tenMediumDynamic:
	($(ONE3) merge_files $(MEDIUM10) $(ADD_REMOVE)) >>$(REPORT)

tenBig:
	@echo "Dziesięć par – duże pliki:" >>$(REPORT)
	make static tenBigStatic endl
	make shared tenBigShared endl
	make dynamic tenBigDynamic endl

tenBigStatic:
	($(ONE1) merge_files $(BIG10) $(ADD_REMOVE)) >>$(REPORT)

tenBigShared:
	($(ONE2) merge_files $(BIG10) $(ADD_REMOVE)) >>$(REPORT)

tenBigDynamic:
	($(ONE3) merge_files $(BIG10) $(ADD_REMOVE)) >>$(REPORT)

hundredShort:
	@echo "Sto par – krótkie pliki:" >>$(REPORT)
	make static hundredShortStatic endl
	make shared hundredShortShared endl
	make dynamic hundredShortDynamic endl

hundredShortStatic:
	($(ONE1) merge_files $(SHORT100) $(ADD_REMOVE)) >>$(REPORT)

hundredShortShared:
	($(ONE2) merge_files $(SHORT100) $(ADD_REMOVE)) >>$(REPORT)

hundredShortDynamic:
	($(ONE3) merge_files $(SHORT100) $(ADD_REMOVE)) >>$(REPORT)

hundredMedium:
	@echo "Sto par – średnie pliki:" >>$(REPORT)
	make static hundredMediumStatic endl
	make shared hundredMediumShared endl
	make dynamic hundredMediumDynamic endl

hundredMediumStatic:
	($(ONE1) merge_files $(MEDIUM100) $(ADD_REMOVE)) >>$(REPORT)

hundredMediumShared:
	($(ONE2) merge_files $(MEDIUM100) $(ADD_REMOVE)) >>$(REPORT)

hundredMediumDynamic:
	($(ONE3) merge_files $(MEDIUM100) $(ADD_REMOVE)) >>$(REPORT)

clean:
	rm tmp/* && rmdir tmp || true