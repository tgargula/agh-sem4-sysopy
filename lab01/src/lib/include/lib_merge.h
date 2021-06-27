#ifndef _MERGE_H
#define _MERGE_H

struct block {
    char **rows;
    int size;
    int current_size;
};

struct array {
    struct block *blocks;
    int size;
};

struct sequence {
    char *file1;
    char *file2;
};

void merge(char sequence[], int seq_no);

void add_blocks(struct array array, int n);

void remove_block(struct array array, int block_index);

void remove_row(struct array array, int block_index, int row_index);

void print_array(struct array array);

struct array create_array(int n);

struct block read_block(char file[]);

void destruct_array(struct array array);

#endif