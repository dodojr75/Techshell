// Names : Caden F. + Jayden N.
// Description: Program that acts as a shell. It can move up and down levels, check file information, and give and take away permissions

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>    // helps with debuging the execution function
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// set true if you want to debug the execution function
bool DEBUG = false;
// permanent int of 100
#define MAX_ARGS 100

// struct used in place of the struct ShellCommand
typedef struct {
    // tracks the first argument, the command
    char *command;
    // tracks all of the arguments
    char *args[MAX_ARGS];
    // keeps count of the number of arguments
    int argCount;

    // tracks number of >
    int hasRedirectOut;
    // tracks number of <
    int hasRedirectIn;
    // tracks number of |
    int hasPipe;

    // the argument to the left of <
    char *redirectInFile;
    // the argument to the right of >
    char *redirectOutFile;
} ParsedInput;

// prototyping
void start();
char* input();
ParsedInput checkInput(char *val);
void execution(ParsedInput command);

int main(){
    // creates a pointer to a new input
    char* newInput;
    // creates the command that will be executed
    ParsedInput command;

    // creates an infinite loop until user manually exits
    for (;;){
        // prompts the user for an input
        start();
        // turns the input into a variable that is readable by the computer
        newInput = input();
        // checks if there are arguments
        if (newInput == NULL) break;

        // feeds the parsed input into the execution function
        command = checkInput(newInput);
        execution(command);
    }
}
void start(){
    // creates a variable to store what will be displayed
    char homeLine[256];
    // stores the directory
    getcwd(homeLine, sizeof(homeLine));
    // prints the display
    printf("%s$ ", homeLine);
}

char* input(){
    // a variable to store the input
    static char newCommand[1024];

    // checks if the input is null and stores the input in a variable
    if (fgets(newCommand, sizeof(newCommand), stdin) == NULL){
        return NULL;
    }

    // replaces the \n with \0
    newCommand[strcspn(newCommand, "\n")] = '\0';

    // returns the input in a variable
    return newCommand;
}
ParsedInput checkInput(char *val){

    ParsedInput result;
    // sets most of the defaults to 0/NULL
    // helps to count up from the ground
    result.argCount = 0;
    result.hasRedirectOut = 0;
    result.hasRedirectIn = 0;
    result.hasPipe = 0;
    result.redirectInFile = NULL;
    result.redirectOutFile = NULL;

    // tokenizes the first argument
    char *token = strtok(val, " ");

    // runs loop until the made token has no value
    while (token != NULL){

        // checks if the token is >
        if (strcmp(token, ">") == 0){
            result.hasRedirectOut = 1;
            // moves the token to the next argument
            token = strtok(NULL, " ");
            // stores the value to the right of the >
            result.redirectOutFile = token;
        }
        // checks if the token is <
        else if (strcmp(token, "<") == 0){
            result.hasRedirectIn = 1;
            // moves the token to the next argument
            token = strtok(NULL, " ");
            // stores the value to the left of the <
            result.redirectInFile = token;
        }
        // checks if the token is |
        else if (strcmp(token, "|") == 0){
            result.hasPipe = 1;
            result.args[result.argCount++] = token;
        }
        else{
            // adds the token to the argument list and increases the argument count
            result.args[result.argCount++] = token;
        }
        // moves to the next token
        token = strtok(NULL, " ");
    }
    // stores the value after the last argument as null
    result.args[result.argCount] = NULL;
    // sets the command to the first argument
    result.command = result.args[0];

    return result;
}
void execution(ParsedInput command){
    // checks if there is no first argument
    if (command.command == NULL){
        return;
    }

    // checks if the command is exit
    if (strcmp(command.command, "exit") == 0){
        // exits the program
        exit(0);
    }
    // checks if the command is cd
    if (strcmp(command.command, "cd") == 0){

        // checks if the only argument is cd
        if (command.argCount == 1){
            // stores the environment variable for the home directory
            char *home = getenv("HOME");
            // if there is no home directory, tell the user and then leave
            if (home == NULL){
                fprintf(stderr, "cd: HOME not set\n");
                return;
            }
            // if there is a home directory, go
            // if that fails, display to the user that it failed
            if (chdir(home) != 0){
                perror("cd failed");
            }
        }
        // if there is another argument besides cd
        else{
            // if there is, go
            // if that fails, display that it failed
            if (chdir(command.args[1]) != 0){
                perror("cd failed");
            }
        }

        return;
    }


    // checks if there was a pipe in the input line
    if (command.hasPipe){

        
        int pipefd[2];
        // tells the user if the pipe failed
        if (pipe(pipefd) < 0){
            perror("Pipe failed");
            return;
        }

        int pipeIndex = 0;
        // find the pipe in all the args
        while (command.args[pipeIndex] &&
               strcmp(command.args[pipeIndex], "|") != 0){
            pipeIndex++;
        }

        // when pipe is found change to null
        command.args[pipeIndex] = NULL;

        // first fork (left side of the pipe)
        pid_t pid1 = fork();
        if (pid1 == 0){
            // redirects stdout to pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);

            // execute left command
            execvp(command.args[0], command.args);
            // in case exec fails
            perror("Left command failed");
            exit(1);
        }

        // second fork (right side of pipe)
        pid_t pid2 = fork();
        if (pid2 == 0){
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);

            // execute right side command
            execvp(command.args[pipeIndex + 1],
                   &command.args[pipeIndex + 1]);
            perror("Right command failed");
            exit(1);
        }

        // Parent process's
        // Closes both pipes but doesn't use them
        close(pipefd[0]);
        close(pipefd[1]);
        // waits for children to finish
        wait(NULL);
        wait(NULL);
        return;
    }

    // making fork
    pid_t pid = fork();

    // Child
    if (pid == 0){

        // output redirect >
        if (command.hasRedirectOut){
            // opens file and returns a file descriptor
            int fd = open(command.redirectOutFile,
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);
            // makes output go from terminal to the file instead
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Input redirect <
        if (command.hasRedirectIn){
            int fd = open(command.redirectInFile, O_RDONLY);
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
