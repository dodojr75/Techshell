// Name(s): Jayden N., Caden F.
// Description:

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// A function that causes the prompt to
// display in the terminal
void start();

// A function that takes input from the user.
// It may return return the input to the calling statement or
// stor it at some memory location using a pointer.
char[] input();

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
void checkInput(char[] val)();

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
void execution(struct val);

int main(){
	char* input;
	struct ShellCommand command;

	for (;;){
		// display the prompt
		start();

		// get the user's input
		char[] newInput = input();

		//parse the command line
		command = checkInput(newInput);

		// execute the command
		execution(command);
	}
}

void start(){

}

char[] input(){

}

void checkInput(char[] val){

}

void execution(struct val){

}

