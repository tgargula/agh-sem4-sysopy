#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "./utils.h"

struct image image;

void load_image(char *filename);
void save_image(char *filename);
void negate(int threads_no, void *(*start_routine)(void *));
void *start_routine_numbers(void *context);
void *start_routine_blocks(void *context);

int main(int argc, char **argv) {
    if (argc != 5) {
        errno = EPERM;
        ERROR_MSG("Incrrect number of arguments! Required 4; provided %d\n", argc - 1);
    }

    int threads_no = strtol(argv[1], NULL, 10);
    char *ifilename = argv[3];
    char *ofilename = argv[4];

    int option;

    if (!strcmp(argv[2], "numbers"))
        option = NUMBERS;
    else if (!strcmp(argv[2], "block"))
        option = BLOCK;
    else {
        errno = EINVAL;
        ERROR_MSG("Option not recognised!\n");
    }

    printf("File: %s\n", ifilename);
    printf("Number of threads: %d\n", threads_no);
    printf("Method: %s\n", argv[2]);

    load_image(ifilename);

    switch (option) {
        case NUMBERS:
            negate(threads_no, start_routine_numbers);
            break;
        case BLOCK:
            negate(threads_no, start_routine_blocks);
            break;
        default:
            errno = EINVAL;
            ERROR_MSG("Option not recognised!\n");
    }

    save_image(ofilename);
    printf("\n\n\n");

    return 0;
}

void load_image(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        ERROR_MSG("Cannot open file!\n");
    }

    if (fgets(image.type, LINE_LEN, file) == NULL) {
        ERROR_MSG("Cannot image type!\n");
    }

    char buf[MAX_LINE_LEN];
    if (fgets(buf, MAX_LINE_LEN, file) == NULL) {
        ERROR_MSG("Cannot read to buffer!\n");
    }

    char *tokbuf;
    tokbuf = strtok(buf, " ");
    image.width = strtol(tokbuf, NULL, 10);
    tokbuf = strtok(NULL, " ");
    image.height = strtol(tokbuf, NULL, 10);

    printf("Image size: %dx%d\n\n", image.width, image.height);


    if (fgets(buf, MAX_LINE_LEN, file) == NULL) {
        ERROR_MSG("Cannot read maximum pixel size!\n");
    }
    image.pixel_max = strtol(buf, NULL, 10);

    int i = 0;
    while (fgets(buf, MAX_LINE_LEN, file) != NULL) {
        if ((tokbuf = strtok(buf, " \t\r\n")) != NULL) {
            do {
                image.cells[i++] = strtol(tokbuf, NULL, 10);
            } while ((tokbuf = strtok(NULL, " \t\r\n")) != NULL);
        }
    }

    if (fclose(file) == -1) {
        ERROR_MSG("Cannot close file!\n");
    }
}

void save_image(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "w")) == NULL) {
        ERROR_MSG("Cannot open file!\n");
    }

    fprintf(file, "%c%c\n", image.type[0], image.type[1]);
    fprintf(file, "%d %d\n", image.width, image.height);
    fprintf(file, "%d\n", image.pixel_max);

    // Line width cannot be bigger than LINE_LEN
    int max_in_line = LINE_LEN / 4;

    for (int i = 0; i < image.width * image.height; i++) {
        fprintf(file, "%3hhu ", image.cells[i]);
        if ((i + 1) % max_in_line == 0) {
            fprintf(file, "\n");
        }
    }

    if (fclose(file) == -1) {
        ERROR_MSG("Cannot close file!\n");
    }
}

void negate(int threads_no, void *(*start_routine)(void *)) {
    pthread_t *threads = calloc(threads_no, sizeof(pthread_t));
    struct context *contexts = calloc(threads_no, sizeof(struct context));
    struct mtime_t times;
    mtime(&times, START, NULL);

    for (int i = 0; i < threads_no; i++) {
        contexts[i].threads_no = threads_no;
        contexts[i].index = i;
        pthread_create(&threads[i], NULL, start_routine, (void *)&contexts[i]);
    }

    for (int i = 0; i < threads_no; i++) {
        double *elapsed;
        pthread_join(threads[i], (void *)&elapsed);
        printf("Thread %d:\t%lf\n", i, *elapsed);
        free(elapsed);
    }

    mtime(&times, STOP, NULL);
    double acc;
    mtime(&times, GET, &acc);
    printf("Summary:\t%lf\n", acc);

    free(threads);
    free(contexts);
}

void *start_routine_numbers(void *arg) {
    struct context *context = (struct context *)arg;
    struct mtime_t times;
    mtime(&times, START, NULL);


    // This for loop is a naive approach – just to show that it is better
    // when the problem is parallelized 
    for (int i = 0; i < image.width * image.height; i++) {
        // It is essential that complementary values as: i, PIXEL_MAX - i
        // are processed by one thread.
        int val = image.cells[i] > image.pixel_max/2 ? image.pixel_max - image.cells[i] : image.cells[i];
        if (val % context->threads_no == context->index) {
            image.cells[i] = image.pixel_max - image.cells[i];
        }
    }

    mtime(&times, STOP, NULL);

    double *acc = malloc(sizeof(double));
    mtime(&times, GET, acc);
    pthread_exit((void *)acc);
}

// Due to its specification; number of threads should be smaller than image width.
// Otherwise newly created threads will not convert any cells.
void *start_routine_blocks(void *arg) {
    struct context *context = (struct context *)arg;
    struct mtime_t times;
    mtime(&times, START, NULL);

    int coeff = (int)ceil((double)image.width / context->threads_no);
    for (int i = context->index * coeff; i < (context->index + 1) * coeff && i < image.width; i++) {
        for (int j = i; j < image.width * image.height; j += image.width) {
            image.cells[j] = image.pixel_max - image.cells[j];
        }
    }

    mtime(&times, STOP, NULL);

    double *acc = malloc(sizeof(double));
    mtime(&times, GET, acc);
    pthread_exit((void *)acc);
}
