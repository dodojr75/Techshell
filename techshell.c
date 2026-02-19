// Names : Caden F. + Jayden N.

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

bool DEBUG = false;
#define MAX_ARGS 100

typedef struct {
    char *command;
    char *args[MAX_ARGS];
    int argCount;

    int hasRedirectOut;
    int hasRedirectIn;
    int hasPipe;

    char *redirectInFile;
    char *redirectOutFile;
} ParsedInput;

void start();
char* input();
ParsedInput checkInput(char *val);
void execution(ParsedInput command);

int main(){
    char* newInput;
    ParsedInput command;

    while (1){
        start();
        newInput = input();
        if (newInput == NULL) break;

        command = checkInput(newInput);
        execution(command);
    }
}
void start(){
    char homeLine[256];
    getcwd(homeLine, sizeof(homeLine));
    printf("%s$ ", homeLine);
}

char* input(){
    static char newCommand[1024];

    if (fgets(newCommand, sizeof(newCommand), stdin) == NULL){
        return NULL;
    }

    newCommand[strcspn(newCommand, "\n")] = '\0';

    return newCommand;
}
ParsedInput checkInput(char *val){

    ParsedInput result;
    result.argCount = 0;
    result.hasRedirectOut = 0;
    result.hasRedirectIn = 0;
    result.hasPipe = 0;
    result.redirectInFile = NULL;
    result.redirectOutFile = NULL;

    char *token = strtok(val, " ");

    while (token != NULL){

        if (strcmp(token, ">") == 0){
            result.hasRedirectOut = 1;
            token = strtok(NULL, " ");
            result.redirectOutFile = token;
        }
        else if (strcmp(token, "<") == 0){
            result.hasRedirectIn = 1;
            token = strtok(NULL, " ");
            result.redirectInFile = token;
        }
        else if (strcmp(token, "|") == 0){
            result.hasPipe = 1;
            result.args[result.argCount++] = token;
        }
        else{
            result.args[result.argCount++] = token;
        }

        token = strtok(NULL, " ");
    }

    result.args[result.argCount] = NULL;
    result.command = result.args[0];

    return result;
}
void execution(ParsedInput command){

    if (command.command == NULL)
        return;

    if (strcmp(command.command, "exit") == 0){
        exit(0);
    }
    if (strcmp(command.command, "cd") == 0){

        // If no argument: go to HOME
        if (command.argCount == 1){
            char *home = getenv("HOME");
            if (home == NULL){
                fprintf(stderr, "cd: HOME not set\n");
                return;
            }
            if (chdir(home) != 0){
                perror("cd failed");
            }
        }
        else{
            if (chdir(command.args[1]) != 0){
                perror("cd failed");
            }
        }

        return;  // IMPORTANT: do not fork
    }



    if (command.hasPipe){

        int pipefd[2];
        if (pipe(pipefd) < 0){
            perror("Pipe failed");
            return;
        }

        int pipeIndex = 0;
        while (command.args[pipeIndex] &&
               strcmp(command.args[pipeIndex], "|") != 0){
            pipeIndex++;
        }

        command.args[pipeIndex] = NULL;

        pid_t pid1 = fork();
        if (pid1 == 0){
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);

            execvp(command.args[0], command.args);
            perror("Left command failed");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 == 0){
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);

            execvp(command.args[pipeIndex + 1],
                   &command.args[pipeIndex + 1]);
            perror("Right command failed");
            exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        wait(NULL);
        wait(NULL);
        return;
    }

    pid_t pid = fork();

    if (pid == 0){

        if (command.hasRedirectOut){
            int fd = open(command.redirectOutFile,
                          O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

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
