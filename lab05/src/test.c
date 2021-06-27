#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void test1(char *producer_exe, char *consumer_exe) {
    char *consumer;
    char *producer;

    mkfifo("test1.fifo", 0666);
    asprintf(&consumer, "%s test1.fifo test1.out 3 &", consumer_exe);
    system(consumer);

    asprintf(&producer, "%s test1.fifo 1 data/resource1.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test1.fifo 3 data/resource2.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test1.fifo 6 data/resource3.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test1.fifo 9 data/resource5.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test1.fifo 7 data/resource4.txt 3 &", producer_exe);
    system(producer);
}

void test2(char *producer_exe, char *consumer_exe) {
    char *producer;
    char *consumer;

    mkfifo("test2.fifo", 0666);
    asprintf(&consumer, "%s test2.fifo test2.out 2 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test2.fifo test2.out 2 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test2.fifo test2.out 2 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test2.fifo test2.out 2 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test2.fifo test2.out 2 &", consumer_exe);
    system(consumer);

    asprintf(&producer, "%s test2.fifo 5 data/resource1.txt 2 &", producer_exe);
    system(producer);

}

void test3(char *producer_exe, char *consumer_exe) {
    char *producer;
    char *consumer;

    mkfifo("test3.fifo", 0666);
    asprintf(&consumer, "%s test3.fifo test3.out 3 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test3.fifo test3.out 3 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test3.fifo test3.out 3 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test3.fifo test3.out 3 &", consumer_exe);
    system(consumer);
    asprintf(&consumer, "%s test3.fifo test3.out 3 &", consumer_exe);
    system(consumer);

    asprintf(&producer, "%s test3.fifo 1 data/resource1.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test3.fifo 3 data/resource2.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test3.fifo 6 data/resource3.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test3.fifo 9 data/resource5.txt 3 &", producer_exe);
    system(producer);
    asprintf(&producer, "%s test3.fifo 7 data/resource4.txt 3 &", producer_exe);
    system(producer);

}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Illegal number of arguments. 2 required; %d provided\n", argc - 1);
        exit(-1);
    }

    char *consumer_exe = argv[1];
    char *producer_exe = argv[2];

    test1(producer_exe, consumer_exe);
    test2(producer_exe, consumer_exe);
    test3(producer_exe, consumer_exe);

    return 0;
}
