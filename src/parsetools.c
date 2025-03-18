#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "parsetools.h"



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




// parse a command line into commmands
// seperated by whitespace
// Returns the number of commands

int split_pipes(char* line, char**  line_words){
    char* saveptr;  // for strtok_r; see http://linux.die.net/man/3/strtok_r
   char* delimiters = " \t\n"; // whitespace
   int i = 0;

   list_to_populate[0] = strtok_r(line, delimiters, &saveptr);

   while(list_to_populate[i] != NULL && i < MAX_LINE_WORDS - 1)  {
       list_to_populate[++i] = strtok_r(NULL, delimiters, &saveptr);

       return something;
}





// Executes commands and forks the process
void execute_piped_commands(char**  line_words, int num_cmds);

   return i;
}
