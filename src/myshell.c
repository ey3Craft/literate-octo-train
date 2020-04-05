#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/LineParser.h"
#include "../include/myshell.h"

#define MAX_INPUT_SIZE 2048

int debug = 0;

int main(int argc, char *argv[])
{
    isDebug(argc);

    printDirectory();

    char buffer[MAX_INPUT_SIZE] = "";
    while (1)
    {
        if(readLine(buffer, MAX_INPUT_SIZE, stdin) != NULL)
        {
            cmdLine *parsedLine = parseCmdLines(buffer);
            if(!isQuit(parsedLine->arguments[0])) // if user entered a command
            {
                if (execute(parsedLine))
                    perror("Error");
            }
            else
            {
                printf("Quiting...\n");
                exit(EXIT_SUCCESS);
            }
            freeCmdLines(parsedLine);
        }
    }
    return EXIT_SUCCESS;
}

int execute(cmdLine *pCmdLine)
{
    char command[MAX_INPUT_SIZE] = "/bin/";
    strcat(command, pCmdLine->arguments[0]);
    if (execv(command, pCmdLine->arguments)) // if execv() failed
        return 1;
    return 0;
}

void printDirectory()
{
    char cwd[100] = "";
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("> Current working direcotry: %s\n", cwd);
}

char* readLine(char *str, int n, FILE *stream)
{
	char *ans = fgets(str, n, stream);
	if (ans == NULL)
		return NULL;
	int newlineIndex = strcspn(str, "\n");
	str[newlineIndex] = 0;
	return str;
}

int isQuit(char* command)
{
    if (!strcmp(command, "quit") || !strcmp(command, "exit")) return 1;
    return 0;
}

void isDebug(int argc)
{
    if (argc > 1)
    {
        debug = 1;
        printf("Debug mode enabled\n");
    }
}

