// Name(s): Jayden N., Caden F.
// Description:

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#define MAX_ARGS 100
//Creating Struct
typedef struct {
    char *command;       //ls
    char *args[MAX_ARGS];//ls -l /home
    int argCount;
    
    int hasRedirectOut;  // >
    int hasRedirectIn;   // <
    int hasPipe;         // |
    
    char *redirectFile;  // filename after >
} ParsedInput;
// A function that causes the prompt to
// display in the terminal
void start();

// A function that takes input from the user.
// It may return return the input to the calling statement or
// stor it at some memory location using a pointer.
char* input();

// A function that parses through the user input.
// Consider having this function return a struct that stores vital
// information about the parsed instruction such as:
// 	The command itself
// 	The arguments that come after the command
// 		Hint: When formating your data,
// 		look into execvp and how it takes in args.
// 	Information about if a redirect was detected such as >, <, or |
// 	Information about whether or not a new file
//          needs to be created and what that filename may be.
//
// Some halpful functions when doing this come from string.h and stdlib.h, such as
// strtok, strcmp, strcpy, calloc, malloc, realloc, free, and more
//
// Be sure to consider/test for situations when a backslash is used to escape the space char
// and when quotes are used to group together various tokens.
ParsedInput checkInput(char *val);
// A function that executes the command.
// This function might thake in a struct that represents the shell command.
//
// Be sure to consider each of the following:
// 	The execvp() function.
// 		This can execute commands that already exist, that is,
// 		you don't need to recreate the functionality of
// 		the commands on your computer, just the shell.
// 		Keep in mind that execvp takes over the current process.
// 	The fork() function.
// 		This can create a process for execvp to take over.
// 	cd is not a command like ls and mkdir
// 		cd is a toold provided by the shell,
// 		so you WILL need to recreate the functionality of cd.
// 	Be sure to handle standard output redirect and standard input redirects here
// 		That is, there symbols: > and <.
// 		Pipe isn't required but could be a nice addition.
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
	static char newCommand[264];
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
    result.redirectFile = NULL;
    
    //Splits the input "val" using spaces as the seperator
    // if typed ls -l > file.txt should split as "ls" "-l" ">" "file.txt"
    char *token = strtok(val, " ");

    while (token != NULL){
    
        // Case 1 output redirect ">" 
        if (strcmp(token, ">") == 0){
            result.hasRedirectOut = 1;
            result.argCount++;
            //since redierct grabs next word which should be filename since redirecting and sets it as a token
            token = strtok(NULL, " ");
            result.argCount++;
            //redirects to the token (file)
            result.redirectFile = token;
            
        }
        // Case 2 input redirect "<"
        else if (strcmp(token, "<") == 0){
        // Marking that it exists
            result.hasRedirectIn = 1;
            result.argCount++;
        }
        // Case 3 Pipe "|"
        else if (strcmp(token, "|") == 0){
        // Showing it exists again
            result.hasPipe = 1;
            result.argCount++;
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

    printf("%s\n", command.command);
    printf("%s\n", command.args[1]);
    printf("%d\n", command.argCount);
    printf("%d\n", command.hasRedirectOut);
    printf("%d\n", command.hasRedirectIn);
    printf("%d\n", command.hasPipe);
}
