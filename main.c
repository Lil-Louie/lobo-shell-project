#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "constants.h"
#include "parsetools.h"


int main() {

    // Buffer for reading one line of input
    char line[MAX_LINE_CHARS];
    // holds separated words based on whitespace
    char* line_words[MAX_LINE_WORDS + 1];
    // holds the commands seperated based on pipes
    char* cmds[MAX_LINE_WORDS +1];

    // Loop until user hits Ctrl-D (end of input)
    // or some other input error occurs
    while( fgets(line, MAX_LINE_CHARS, stdin) ) {
        int num_words = split_cmd_line(line, line_words);
        int num_cmds = split_pipes(line, cmds);

        if (num_cmds == 1) { // Single command, no pipe
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            } 
            if (pid == 0) { 
            execvp(line_words[0], line_words);
            perror("exec failed");
            exit(EXIT_FAILURE);
            }  
            int status;
            waitpid(pid, &status, 0);
            } else { // Handle piped commands
            execute_piped_commands(cmds, num_cmds);
        }
    }

    return 0;
}
