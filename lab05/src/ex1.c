#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE_LEN 255
#define DEBUG 0

struct command {
    char *alias;
    char **args;
};

char *ltrim(char *s) {
    if (!s)
        return NULL;
    if (!*s)
        return s;
    char *s_tmp = s;
    while (isspace(*s_tmp) || *s_tmp == '\n') {
        ++s_tmp;
    }
    memmove(s, s_tmp, s_tmp - s);
    return s;
}

char *rtrim(char *s) {
    char *ptr;
    if (!s)
        return NULL;
    if (!*s)
        return s;
    for (ptr = s + strlen(s) - 1; (ptr >= s) && (isspace(*ptr) || *ptr == '\n'); --ptr)
        ;
    ptr[1] = '\0';
    return s;
}

char *trim(char *s) {
    return ltrim(rtrim(s));
}

int cntchr(char *s, char ch, int acc) {
    return *s == '\0' ? acc : cntchr(s + 1, ch, acc + (*s == ch));
}

void getcommand(char *buf, struct command *command) {
    char *line = calloc(strlen(buf) + 1, sizeof(char));
    strcpy(line, buf);
    command->alias = trim(strtok(line, "="));
    char *args = trim(strtok(NULL, "=")) + 1;
    int argsno = cntchr(args, ' ', 0);
    char *arg = strtok(args, " ");
    command->args = calloc(argsno + 2, sizeof(char *));
    command->args[0] = arg;
    for (int i = 1; (arg = strtok(NULL, " ")) != NULL; i++) {
        command->args[i] = arg;
    }
    command->args[argsno + 1] = NULL;
}

char **getargs(struct command *commands, size_t size, char *alias) {
    for (int i = 0; i < size; i++) {
        if (!strcmp(alias, commands[i].alias)) {
            return commands[i].args;
        }
    }

    return NULL;
}

void appendargs(char ****command, size_t *command_size, char **arg) {
    int i = 0;
    while (arg[i]) {
        int j = i;
        while (arg[j] && *arg[j] != '|')
            j++;

        *command = (char ***)realloc(*command, (*command_size + 1) * sizeof(char **));

        // copy command to structure
        command[0][*command_size] = malloc((j - i + 1) * sizeof(char *));
        for (int k = i; k < j; k++) {
            command[0][*command_size][k - i] = calloc(strlen(arg[k]) + 1, sizeof(char));
            strcpy(command[0][*command_size][k - i], arg[k]);
        }
        command[0][*command_size][j - i] = NULL;

        *command_size = *command_size + 1;

        if (arg[j])  // *arg[j] == "|"
            i = j + 1;
        else
            i = j;
    }
}

void printchars(char ***command, size_t command_size) {
    for (int i = 0; i < command_size; i++) {
        for (int j = 0; command[i][j]; j++) {
            printf("%s ", command[i][j]);
        }
        printf("\n");
    }
}

void exec(struct command *commands, size_t size, char *buf) {
    char *alias = trim(strtok(buf, " "));
    char **args;
    char ***command = malloc(sizeof(char **));
    size_t command_size = 0;

    do {
        if (strcmp(alias, "|") != 0) {
            args = getargs(commands, size, alias);
            appendargs(&command, &command_size, args);
        }
    } while ((alias = trim(strtok(NULL, " "))) != NULL);

    int old_fd[2];
    int new_fd[2];
    pid_t *pids = calloc(command_size, sizeof(pid_t));

    for (int i = 0; i < command_size; i++) {
        if (i < command_size - 1)
            pipe(new_fd);

        if ((pids[i] = fork()) == 0) {  // child
            if (i > 0) {
                dup2(old_fd[0], STDIN_FILENO);
                close(old_fd[0]);
                close(old_fd[1]);
            }
            if (i < command_size - 1) {
                dup2(new_fd[1], STDOUT_FILENO);
                close(new_fd[0]);
                close(new_fd[1]);
            }

            execvp(command[i][0], command[i]);
            exit(0);
        } else {  // parent
            if (i > 0) {
                close(old_fd[0]);
                close(old_fd[1]);
            }
            if (i < command_size - 1) {
                old_fd[0] = new_fd[0];
                old_fd[1] = new_fd[1];
            }
        }
    }

    for (int i = 0; i < command_size; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

char **parsefile(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Unable to open file %s!\n", filename);
        exit(-2);
    }

    char *buf = NULL;
    size_t size = 0;
    struct command *commands = NULL;

    int count = 0;
    while ((getline(&buf, &size, file)) > 0) {
        if (strchr(buf, '=') != NULL) {
            // It contains '=' so it is a command alias
            struct command command;
            getcommand(buf, &command);
            struct command *tmp = (struct command *)realloc(commands, ++count * sizeof(struct command));
            tmp[count - 1] = command;
            commands = tmp;
        } else if (*buf != '\n') {
            // It does not contain '=' so it must be a command to execute
            exec(commands, count, buf);
        }
    }

    if (DEBUG) {
        printf("aliases\n");
        for (int i = 0; i < count; i++) {
            printf("%s\n", commands[i].alias);
            for (int j = 0; commands[i].args[j]; j++)
                printf("%s ", commands[i].args[j]);
            printf("\n");
        }
    }

    if (buf)
        free(buf);
    for (int i = 0; i < count; i++) {
        free(commands[i].args);
    }
    free(commands);
    fclose(file);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments! 1 required; %d provided!\n", argc - 1);
        exit(-1);
    }

    char *filename = argv[1];

    parsefile(filename);

    return 0;
}