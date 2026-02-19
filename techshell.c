// Name(s): Jayden N., Caden F.
// Description:

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

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
	} 
	else {
		//Check if no command entered
		if (command.command == NULL) {
			return;
		}
		if (strcmp(command.command, "cd") == 0){
			// cd
			// checks if there are enough arguments
			// wants 2 arguments (cd and a path)
			if (command.argCount < 2){
				// creates current directory
				char directory[MAX_ARGS];
				getcwd(directory, sizeof(directory));
				// tokenizes each section between the /
				char *token = strtok(home, "/");
				int i = 0;
				char *newDirectory[MAX_ARGS];
				// places first 2 tokens in the new array
				while (i < 2){
					newDirectory[i] = token;
					token = strtok(Null, "/");
					i += 1;
				}
				int j = 0;
				char newLine[MAX_ARGS];
				// combines the two strings with / for new directory
				while (j < i){
					strcat(newLine, "/");
					strcat(newLine, newDirectory[j]);
					j += 1;
				}
				// changes directory to new directory
				chdir(newLine);
			} 
			else if (command.argCount > 2){
				printf("Too many arguments were provided");
			} 
			else {
				char *moveTo = command.args[1];
				if (moveTo == ".."){
					// move up one level
					char oldDirectory[MAX_ARGS];
					getcwd(oldDirectory, sizeof(oldDirectory));
					char *token = strtok(oldDirectory, "/");
					int i = 0;
					char *oldString[MAX_ARGS];
					while (token != NULL){
						oldString[i] = token;
						token = strtok(NULL, "/");
						i += 1;
					}
					i -= 1;
					char *newDirectory[MAX_ARGS];
					int j = 0;
					while (j < i){
						newDirectory[j] = oldString[j];
						j += 1;
					}
					char newLine[MAX_ARGS];
					int k = 0;
					while (k < j){
						strcat(newLine, "/");
						strcat(newLine, newDirectory[k]);
						k += 1;
					}
					chdir(newLine);

				} 
				else {
					char current[MAX_ARGS];
					getcwd(current, sizeof(current));
					char *token = strtok(current, "/");
					int i = 0;
					char *oldDirectory[MAX_ARGS];
					while (token != NULL){
						oldDirectory[i] = token;
						token = strtok(NULL, "/");
						i += 1;
					}
					i -= 1;
					char *newDirectory[MAX_ARGS];
					int j = 0;
					while (j < i){
						newDirectory[j] = oldDirectory[j];
						j += 1;
					}
					char newLine[MAX_ARGS];
					int k = 0;
					while (k < j){
						strcat(newLine, "/");
						strcat(newLine, newDirectory[k]);
						k += 1;
					}
					strcat(newLine, "/");
					strcat(newLine, command.args[1]);
					chdir(newLine);
				}
			}
		} 
		
		else if (strcmp(command.command, "exit") == 0){
		    exit(0);
		} 
		else {
			pid_t pid = fork();
			//making fork and checking if it failed or not
			if (pid < 0){
			    perror("Fork failed");
			    return;
			}
			
			// Child
			if (pid == 0){
			
			    // output redirect >
			    
			    if (command.hasRedirectOut){
			        //opens the file and returns a file descriptor 
			        int fd = open(command.redirectOutFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			        
			        if (fd < 0){
			            perror("Open Failed");
			            exit(1);
			        
			        }
			        //Makes output go from terminal to the file instead 
			        dup2(fd, STDOUT_FILENO);
			        close(fd);
			    }
			    
			    // Input redirect <
			    if (command.hasRedirectIn){
			    
			        int fd = open(command.redirectInFile, O_RDONLY);
			        
			        if (fd < 0){
			            perror("Failed to open");
			            exit(1);
			        }
			        
			        dup2(fd, STDIN_FILENO);
			        close(fd);
			    }
			    execvp(command.command, command.args);
			    
			    perror("Command failed");
			    exit(1);
			}
			else{
			    wait(NULL);
			}
		}		
}
