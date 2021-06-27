#include "src/lib/include/lib_library.h"
#include "src/lib/include/lib_benchmark.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    char *filename1;
    char *filename2; 

    if (argc == 1) {
        filename1 = calloc(MAX_LINE_LEN, sizeof(char));
        filename2 = calloc(MAX_LINE_LEN, sizeof(char));
        if(scanf("%s", filename1));
        if(scanf("%s", filename2));
    } else {
        filename1 = argv[1];
        filename2 = argv[2];
    }

#ifdef benchmark
    struct btimes startTimes = measureTime();
#endif

    merge(filename1, filename2);

#ifdef benchmark
    printTime("pomiar_zad_1.txt", startTimes);
#endif

    if (argc == 1) {
        free(filename1);
        free(filename2);
    }

    return 0;
}