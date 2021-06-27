#include "src/lib/include/lib_library.h"
#include "src/lib/include/lib_benchmark.h"

int main(int argc, char **argv) {

#ifdef benchmark
    struct btimes startTimes = measureTime();
#endif 

    breakLines(argv[1], argv[2], 50);

#ifdef benchmark
    printTime("pomiar_zad_5.txt", startTimes);
#endif

    return 0;
}