#include "src/lib/include/lib_library.h"
#include "src/lib/include/lib_benchmark.h"

int main(int argc, char **argv) {

#ifdef benchmark
    struct btimes startTimes = measureTime();
#endif

    filterNumbers(argv[1], argv[2], argv[3], argv[4]);

#ifdef benchmark
    printTime("pomiar_zad_3.txt", startTimes);
#endif

    return 0;
}