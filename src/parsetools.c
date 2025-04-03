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
   char* saveptr;  // for strtok_r; see http://linux.die.net/man/3/strtok_r
   char* delimiters = " \t\n"; // whitespace
   int i = 0;

   list_to_populate[0] = strtok_r(line, delimiters, &saveptr);

   while(list_to_populate[i] != NULL && i < MAX_LINE_WORDS - 1)  {
       list_to_populate[++i] = strtok_r(NULL, delimiters, &saveptr);
   };
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