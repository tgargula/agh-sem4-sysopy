#include "src/lib/include/lib_library.h"
#include "src/lib/include/lib_benchmark.h"

int main(int argc, char **argv) {

    char sign = argv[1][0];
    char *filename = argv[2];

#ifdef benchmark
    struct btimes startTimes = measureTime();
#endif

    filterWithSign(filename, sign);

#ifdef benchmark
    printTime("pomiar_zad_2.txt", startTimes);
#endif

    return 0;
}