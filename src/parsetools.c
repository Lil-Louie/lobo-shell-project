#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include "constants.h"
#include <ctype.h> 
#include "parsetools.h"
#include <fcntl.h>
int const MAX_LINE = 1024;
int const MAX_ARGS = 10;

void syserror(const char *);


char* trim(char* str) {
    if (str == NULL) return NULL;

    // Skip leading whitespace
    while (isspace((unsigned char)*str)) str++;

    // If it's all spaces
    if (*str == '\0') return str;

    // Trim trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';  // Null-terminate

    return str;
}


// Parse a command line into a list of words,
// separated by whitespace.
// Returns the number of words
//
int split_cmd_line(char* line, char** list_to_populate) {
    int i = 0;
    char* ptr = line;
    int in_quotes = 0;

    while (*ptr != '\0') {
        // Skip leading spaces
        while (isspace((unsigned char)*ptr)) ptr++;

        if (*ptr == '\0') break;

        char* start = ptr;
        if (*ptr == '"') {
            in_quotes = 1;
            start = ++ptr; // skip opening quote
            while (*ptr && *ptr != '"') ptr++;
        } else {
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        }

        // Null-terminate the token
        if (*ptr != '\0') {
            *ptr = '\0';
            ptr++;
        }

        list_to_populate[i++] = start;

        if (in_quotes) in_quotes = 0;
    }

    list_to_populate[i] = NULL;
    return i;
}




// parse a command line into commmands
// seperated by whitespace
// Returns the number of commands

int split_pipes(char* line, char** list_of_cmds) {
    int i = 0;
    int in_quotes = 0; // Tracks if we're inside double quotes
    char* start = line;

    for (char* ptr = line; *ptr != '\0'; ptr++) {
        if (*ptr == '"') {
            in_quotes = !in_quotes; // Toggle in/out of quotes
        } else if (*ptr == '|' && !in_quotes) {
            *ptr = '\0';  // Null-terminate the command
            list_of_cmds[i++] = trim(start);
            start = ptr + 1;
        }
    }

    // Last command after the final pipe
    list_of_cmds[i++] = trim(start);
    list_of_cmds[i] = NULL;

    return i;
}





// Executes commands and forks the process
void execute_piped_commands(char** list_of_cmds, int num_cmds) {
    int pipes[num_cmds - 1][2];  
    pid_t pids[num_cmds];



    // Create the pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }
        //Debugging
        for (int i  = 0; i < 5; i++){
            fprintf(stdout, "Command #%d: %s\n", i, list_of_cmds[i]);
        }

    // Fork processes for each command
    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // child

            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < num_cmds - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            char* cmd_args[MAX_LINE_WORDS];
            split_cmd_line(list_of_cmds[i], cmd_args);


            //check for redirection
            for (int k = 0; cmd_args[k] != NULL; ++k) {
                printf(stdout, "redirection symbol: %s\nfile: %s", cmd_args[k], cmd_args[k+1]);
                if (strcmp(cmd_args[k], ">") == 0 || strcmp(cmd_args[k], ">>") == 0) {
                    int fd;
                    int flags = O_WRONLY | O_CREAT;
                    if (strcmp(cmd_args[k], ">") == 0)
                        flags |= O_TRUNC;
                    else
                        flags |= O_APPEND;

                    fd = open(cmd_args[k + 1], flags, 0644);
                    if (fd < 0) syserror("open for output redirection");
                    dup2(fd, STDOUT_FILENO);
                    close(fd);

                    cmd_args[k] = NULL;  // Remove redirection from args
                    break;
                } else if (strcmp(cmd_args[k], "<") == 0) {
                    int fd = open(cmd_args[k + 1], O_RDONLY);
                    if (fd < 0) syserror("open for input redirection");
                    dup2(fd, STDIN_FILENO);
                    close(fd);

                    cmd_args[k] = NULL;
                    break;
                }
            }




            fprintf(stderr, "Exec #%d: %s\n", i, cmd_args[0]);
            execvp(cmd_args[0], cmd_args);
            syserror("Could not exec command");
        }
    }

    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }
}


void syserror(const char *s)
{
    extern int errno;
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, " (%s)\n", strerror(errno));
    exit(1);
}