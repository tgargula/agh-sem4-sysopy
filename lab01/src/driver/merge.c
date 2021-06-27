#include "src/lib/include/lib_merge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {

#ifdef DyNaMiC
    void* handle = dlopen("../target/lib/lib_merge.so", RTLD_NOW);
    void (*merge)(char *, int) = (dlsym(handle, "merge"));
    void (*add_blocks)(struct array, int) = (dlsym(handle, "add_blocks"));
    void (*remove_block)(struct array, int) = (dlsym(handle, "remove_block"));
    void (*remove_row)(struct array, int, int) = (dlsym(handle, "remove_row"));
    void (*print_array)(struct array) = (dlsym(handle, "print_array"));
    struct array (*create_array)(int) = (dlsym(handle, "create_array"));
    void (*destruct_array)(struct array) = (dlsym(handle, "destruct_array"));
#endif

    if (system("if [ ! -d tmp ]; then mkdir tmp; fi"));

    int files = atoi(argv[1]);

    struct array array;
    struct tms start;
    clock_t start_t = times(&start);

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "create_table")) {
            int size = atoi(argv[++i]);
            array = create_array(size);
        }
        else if (!strcmp(argv[i], "merge_files")) {
            for (int j = 0; j < files; j++)
                merge(argv[i + j + 1], j);
            i += files;
        }
        else if (!strcmp(argv[i], "remove_row")) {
            int block = atoi(argv[++i]);
            int row = atoi(argv[++i]);
            remove_row(array, block, row);
        }
        else if (!strcmp(argv[i], "add_blocks"))
            add_blocks(array, files);
        else if (!strcmp(argv[i], "remove_block"))
            remove_block(array, atoi(argv[++i]));
        else if (!strcmp(argv[i], "count_lines"))
            printf("%d\n", array.blocks[atoi(argv[++i])].current_size);
        else if (!strcmp(argv[i], "print"))
            print_array(array);
    }

    struct tms end;
    clock_t end_t = times(&end);
    printf("real %f\n", (double)(end_t - start_t) / sysconf(_SC_CLK_TCK));
    printf("user %f\n", (double)(end.tms_utime - start.tms_utime) / sysconf(_SC_CLK_TCK));
    printf("sys  %f\n", (double)(end.tms_stime - start.tms_stime) / sysconf(_SC_CLK_TCK));

    destruct_array(array);
    if (system("rm -r tmp"));

#ifdef DyNaMiC
    dlclose(handle);
#endif
    
    return 0;
}