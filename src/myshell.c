#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "../include/LineParser.h"
#include "../include/myshell.h"
#include "../include/LinkedList.h"
#include "../include/Utils.h"

#define MAX_INPUT_SIZE 2048

int debug = 0;
char *programName = "";

int main(int argc, char **argv)
{
    programName = argv[0] + 2; // argv[0] + 2 = program name without "./"

    if (argc > 1)
        isDebug(argv);

    char buffer[MAX_INPUT_SIZE] = "";
    while (1)
    {
        printDirectory();
        readLine(buffer, MAX_INPUT_SIZE, stdin);
        if (!isCommand(buffer, ""))
        {
            cmdLine *parsedLine = parseCmdLines(buffer);
            char *command = parsedLine->arguments[0];
            if (!isQuit(command)) // if user entered a command
            {
                if (isCommand(command, "cd")) // if the command is "cd"
                    changeCwd(parsedLine);
                else
                    execute(parsedLine);
            }
            else
                exit(EXIT_SUCCESS);

            freeCmdLines(parsedLine);
        }
    }
    return EXIT_SUCCESS;
}

int executeSingleCommand(cmdLine *pCmdLine)
{
    char *command = pCmdLine->arguments[0];
    execvp(command, pCmdLine->arguments);
    return 0; // execvp failed if code reaches this line
}

int execute(cmdLine *line)
{
    pid_t pid = fork();
    switch (pid)
    {
    case 0:
    {
        // runs on child proccess:
        int status = executeSingleCommand(line);
        if (status != 1)
        {
            printErrMsg(line->arguments[0]);
            _exit(EXIT_FAILURE);
        }
        break;
    }
    case -1:
        // fork failed
        printErrMsg("fork");
        return 0;
    default:
        // runs on parent proccess:
        if (line->blocking == 1) // if ampersand isn't added, wait for child to finish
            waitForChild(pid);
        if (debug)
        {
            fprintf(stderr, "Forked, parent proccess id: %d\n", getpid());
            fprintf(stderr, "Child proccess id: %d\n", pid);
            fprintf(stderr, "Executing command: %s\n", line->arguments[0]);
        }
    }
    return 1;
}

int changeCwd(cmdLine *line)
{
    if (line->argCount > 1)
    {
        int chdirResult = chdir(line->arguments[1]);
        if (chdirResult != 0)
        {
            printErrMsg("cd");
            return -1;
        }
    }
    return 0;
}