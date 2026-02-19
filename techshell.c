// Name(s): Jayden N., Caden F.
// Description:

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

bool DEBUG = true;
#define MAX_ARGS 100
//Creating Struct
typedef struct {
    char *command;       //ls
    char *args[MAX_ARGS];//ls -l /home
    int argCount;
    
    int hasRedirectOut;  // >
    int hasRedirectIn;   // <
    int hasPipe;         // |
    
    char *redirectInFile;  // filename after >
    char *redirectOutFile; // filename after <
} ParsedInput;

// function for the display
void start();

// function to take the input/arguments
char* input();

// function to check what was taken in the input line
ParsedInput checkInput(char *val);

// function to execute what the check input finds
void execution(ParsedInput command);

int main(){
	char* newInput;
	ParsedInput command;

	for (;;){
		// display the prompt
		start();

		// get the user's input
		newInput = input();

		//parse the command line
		command = checkInput(newInput);

		// execute the command
		execution(command);
	}
}

void start(){
	// creates a variable for the path
	char homeLine[256];
	getcwd(homeLine, sizeof(homeLine));
	// prints the stuff before the $ and the $ (~$)
	printf("%s$ ", homeLine);
}

char* input(){
	// creates a character to store the command
	static char newCommand[MAX_ARGS];
	// reads the line
	fgets(newCommand, sizeof(newCommand), stdin);
	// stores the line
	return newCommand;
}
ParsedInput checkInput(char *val){

    // create a variable named result to store everything
    ParsedInput result;
    
    // Initialize everything
    result.argCount = 0;
    result.hasRedirectOut = 0;
    result.hasRedirectIn = 0;
    result.hasPipe = 0;
    result.redirectInFile = NULL;
    result.redirectOutFile = NULL;
    
    //Splits the input "val" using spaces as the seperator
    // if typed ls -l > file.txt should split as "ls" "-l" ">" "file.txt"
    char *token = strtok(val, " ");

    while (token != NULL){
    
        // Case 1 output redirect ">" 
        if (strcmp(token, ">") == 0){
            result.hasRedirectOut = 1;

            //since redierct grabs next word which should be filename since redirecting and sets it as a token
            token = strtok(NULL, " ");

            //redirects to the token (file)
            result.redirectOutFile = token;
            
        }
        // Case 2 input redirect "<"
        else if (strcmp(token, "<") == 0){
        // Marking that it exists
            result.hasRedirectIn = 1;
            token = strtok(NULL, " ");
            result.redirectInFile = token; 
        }
        // Case 3 Pipe "|"
        else if (strcmp(token, "|") == 0){
        // Showing it exists again
            result.hasPipe = 1;

        }
        //Case 4 Normal Args (a command or normal arg like ls -l)
        else{
        //stores the word in the args array
            result.args[result.argCount] = token;
            result.argCount++;
        }
        //next token
        token = strtok(NULL, " ");
    }
    //shows execvp or whichever function used when to stop
    result.args[result.argCount] = NULL;
    //sets first arg as command
    result.command = result.args[0];
    
    return result;
}


void execution(ParsedInput command){
	if (DEBUG == true){
		if(command.command != NULL){
        		printf("Command: %s\n", command.command);
		}
    		printf("Arguments:\n");
    		for(int i = 0; i < command.argCount; i++){
        		printf("  %s\n", command.args[i]);
    		}

    		printf("Arg Count: %d\n", command.argCount);
    		printf("Redirect Out: %d\n", command.hasRedirectOut);
   		printf("Redirect In: %d\n", command.hasRedirectIn);
    		printf("Pipe: %d\n", command.hasPipe);

    		if(command.redirectInFile != NULL){
        		printf("Redirect In File: %s\n", command.redirectInFile);
        		}
    		if(command.redirectOutFile != NULL){
        		printf("Redirect Out File: %s\n", command.redirectOutFile);
    		}
	} else {
		// checks if it is an available command
		while (true){
			if (strcmp(command.command, "cd")){
				// cd
				// checks if there are enough arguments
				// wants 2 arguments (cd and a path)
				if (command.argCount < 2){
					// goes to the home directory
					char directory[MAX_ARGS];
					getcwd(directory, sizeof(directory));

					break;
				} else if (command.argCount > 2){
					printf("Too many arguments were provided");
					break;
				} else {
					char moveTo[] = command.args[1];
					if (moveTo == ".."){
						// move up one level
		
					} else {
						// move to file if available
							// if file available, move to it
							// else, spit out an error
					}
				}
			} else if (strcmp(command.command, "exit")){
			
			} else {
				execvp(command.command, command.args);
			}
		}
		
}
