#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/lib/include/lib_merge.h"

void destruct_block(struct block block) {
    if (block.rows != NULL) {
        for (int i = 0; i < block.size; i++)
            if (block.rows[i] != NULL)
                free(block.rows[i]);
        free(block.rows);
    }
}

void destruct_array(struct array array) {
    for (int i = 0; i < array.size; i++)
        destruct_block(array.blocks[i]);
    free(array.blocks);
}

int countlines(char file[]) {
    FILE *f = NULL;
    f = fopen(file, "r");
    int lines = 0;

    while (!feof(f))
        if (fgetc(f) == '\n')
            lines++;
    
    fclose(f);

    return lines;
}

struct sequence divide_files_pair(char sequence[]) {
    struct sequence seq;
    int i, j;
    for (i = 0; sequence[i] != ':'; i++);
    for (j = 0; sequence[i + j + 1]; j++);
    seq.file1 = calloc(i + 1, sizeof(char));
    seq.file2 = calloc(j + 1, sizeof(char));
    for (i = 0; sequence[i] != ':'; i++)
        seq.file1[i] = sequence[i];
    for (j = 0; sequence[i + j + 1]; j++)
        seq.file2[j] = sequence[i + j + 1];
    seq.file1[i] = seq.file2[j] = '\0';
    return seq;
}

void add_blocks(struct array array, int n) {
    for (int i = 0; i < n; i++) {
        char *number = calloc(12, sizeof(*number));
        sprintf(number, "%d", i);
        char file[12] = "tmp/";
        strcat(file, number);
        array.blocks[i] = read_block(file);
        free(number);
    }
}

void merge(char sequence[], int seq_no) {
    char *number = calloc(8, sizeof(*number));
    sprintf(number, "%d", seq_no);
    char file[12] = "tmp/";
    strcat(file, number);
    struct sequence seq = divide_files_pair(sequence);
    FILE *f1 = fopen(seq.file1, "r");
    FILE *f2 = fopen(seq.file2, "r");
    FILE *fout = fopen(file, "w");

    if (f1 == NULL || f2 == NULL)
        fprintf(stderr, "Wystąpił problem z plikami!\n");

    char *line = NULL;
    short stop = 0;
    size_t len = 0;
    while (!stop) {
        stop = 1;
        if (getline(&line, &len, f1) != -1) {
            fprintf(fout, "%s", line);
            stop = 0;
        }
        if (getline(&line, &len, f2) != -1) {
            fprintf(fout, "%s", line);
            stop = 0;
        }
    }

    free(number);
    free(seq.file1);
    free(seq.file2);
    free(line);
    fclose(f1);
    fclose(f2);
    fclose(fout);
}

struct block read_block(char file[]) {
    struct block block;
    block.size = countlines(file);
    block.current_size = block.size;
    block.rows = calloc(block.size, sizeof(char *));

    FILE *f = fopen(file, "r");
    char *line = NULL;
    size_t len = 0;
    int read = 0;

    for (int i = 0; (read = getline(&line, &len, f)) != -1 && i < block.size; i++) {
        block.rows[i] = calloc(read, sizeof(char));
        for (int j = 0; line[j]; j++)
            block.rows[i][j] = line[j];
    }

    free(line);
    fclose(f);

    return block;
}

struct array create_array(int n) {
    struct array array;
    array.size = n;
    array.blocks = calloc(array.size, sizeof(*array.blocks));
    
    return array;
}

void remove_block(struct array array, int block_index) {
    destruct_block(array.blocks[block_index]);
    array.blocks[block_index].rows = NULL;
}

void remove_row(struct array array, int block_index, int row_index) {
    array.blocks[block_index].current_size--;
    free(array.blocks[block_index].rows[row_index]);
    array.blocks[block_index].rows[row_index] = NULL;
}

void print_array(struct array array) {
    for (int i = 0; i < array.size; i++) {
        if (array.blocks[i].rows != NULL) {
            for (int j = 0; j < array.blocks[i].size; j++) {
                char *row = array.blocks[i].rows[j];
                if (row != NULL)
                    printf("%s", row);
            }
            printf("\n");
        }
    }
}