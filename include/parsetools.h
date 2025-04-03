#ifndef PARSETOOLS_H
#define PARSETOOLS_H

// Parse a command line into a list of words,
//    separated by whitespace.
// Returns the number of words
//
int split_cmd_line(char* line, char** list_to_populate); 

//  Parse command line into a list of commands
//        
int split_pipes(char* line, char**  list_of_cmds);


void execute_piped_commands(char** cmds, int num_cmds);

#endif
