#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include "constants.h"
#include "parsetools.h"

int const MAX_LINE = 1024;
int const MAX_ARGS = 10;



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
}



// parse a command line into commmands
// seperated by whitespace
// Returns the number of commands

int split_pipes(char* line, char**  list_of_cmds){
   char* saveptr;  // for strtok_r; see http://linux.die.net/man/3/strtok_r
   char* pipes = "|"; // Pipes
   int i = 0;

   list_of_cmds[0] = strtok_r(line, pipes, &saveptr);
   while(list_of_cmds[i] != NULL && i < MAX_LINE_WORDS - 1)  {
        while (*list_of_cmds[i] == ' ') list_of_cmds[i]++;
        i++;
        list_of_cmds[i] = strtok_r(NULL, pipes, &saveptr);
   }

    fprintf(stdout, "Num of cmds (pipe):  %d\n", i);
    for (int j = 0; j < i; j++) {
        fprintf(stdout, "command %d: %s\n", j, list_of_cmds[j]);
    }

    return i;
}



// Executes commands and forks the process
void execute_piped_commands(char**  list_of_cmds, int num_cmds){
    int pipes[num_cmds - 1][2];
    pid_t pids[num_cmds];

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

        for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
         fprintf("The second child's pid is: %d\n", list_of_cmds[i]);


        }
}
