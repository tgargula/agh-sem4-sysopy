#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char **argv) {

    if (argc < 2 || argc == 3 || argc > 4) {
        fprintf(stderr, "Illegal number of arguments! 1 or 3 required; %d provided", argc - 1);
        exit(-1);
    }

    char *path = malloc(sizeof(char) * 255);
    int *size;

    if (argc == 2) {
        if (!strcmp(argv[1], "listdir")) {
            FILE *p = popen("ls -hal | grep ^-", "w");
            pclose(p);
        }
        else if (!strcmp(argv[1], "whoami")) {
            FILE *p = popen("cat /etc/passwd | grep $(whoami)", "w");
            pclose(p);
        }
        else {
            fprintf(stderr, "Illegal argument! There are two options: 'nadawca' or 'data'");
            exit(-2);
        }
    }

    if (argc == 4) {
        char *email = argv[1];
        char *title = argv[2];
        char *body = argv[3];
        char *command;
        asprintf(&command, "echo \"%s\" | mail -s \"%s\" %s", body, title, email);

        FILE *p = popen(command, "w");

        pclose(p);
    }

    return 0;
}