#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include "CommandNode.h"

static void signalHandler(int iSig) {
    fprintf(stderr,"Exited with signal %d", iSig);
}

int main(int argc, char *argv[])
{
    // Checks if more than 1 or no file name was entered
    if (argc <= 1 || argc > 2) {
        printf("Invalid command input. Argument should only have 1 file\n");
        exit(EXIT_FAILURE);
    }

    // Initialize variables for reading file
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char newString[20][20];
    int i,j,rowNum;

    // Initialize the linkedlist and necessary variables to help create the list
    CommandNode *head = NULL;
    CommandNode *currNode = NULL;
    CommandNode *prevNode = (CommandNode*) malloc(sizeof(CommandNode));
    int indexNode = 0;

    // Open the file specified by the terminal
    fp = fopen(argv[1], "r");

    // Exit program if the file couldn't be opened
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    // Read each line of the file
    while ((read = getline(&line, &len, fp)) != -1) {

        // Insert the command line into a 2D array
        j=0; rowNum=0;
        for(i = 0; i <= (strlen(line)); i++)
        {
            // if space or NULL found, assign NULL into newString[ctr]
            if(line[i] == ' ' || line[i] == '\0')
            {
                newString[rowNum][j] = '\0';
                rowNum++;       //for next word
                j = 0;          //for next word, init index to 0
            }
            else
            {
                newString[rowNum][j] = line[i];
                j++;
            }
        }

        // Create a CommandNode and append it to the linked list
        currNode = (CommandNode*) malloc(sizeof(CommandNode));
        CreateCommandNode(currNode, newString, indexNode, NULL, rowNum);
        InsertCommandAfter(prevNode, currNode);
        prevNode = currNode;
        indexNode += 1;

        // Update head if the indexNode is 0
        if ((indexNode-1) == 0) {
            head = (CommandNode*) malloc(sizeof(CommandNode));
            head = currNode;
        }
    }

    // Close file handler
    fclose(fp);

    // Begin creating processes for each node in the linked list
    currNode = head;
    while (currNode != NULL) {
        pid_t pid = fork();

        // Print message if fork fails
        if (pid < 0) {
            fprintf(stderr, "fork error\n");
            return 1;
        }

//        char outFileName[10];
//        char errFileName[10];
//
//        sprintf(outFileName, "%d.out", (currNode->index) + 1);
//        sprintf(errFileName, "%d.err", (currNode->index) + 1);
//
//        int outFile = open(outFileName, O_WRONLY | O_CREAT | O_APPEND );
//        int errFile = open(errFileName, O_WRONLY | O_CREAT | O_APPEND );
//
//        dup2(outFile, STDOUT_FILENO);
//        dup2(errFile, STDERR_FILENO);

        // Create signal handlers
        signal(SIGKILL,  signalHandler);
        signal(SIGTERM,  signalHandler);

        // Initialize variables for timer
        struct timespec start, finish;
        double elapsed;

        // Begin timer and store start time into the current node
        clock_gettime(CLOCK_MONOTONIC, &start);
        currNode -> starttime = start.tv_sec;

        // Child process
        if (pid == 0) {
            currNode -> PID = getpid();
            currNode -> active = true;

            char *temp[20];
            int rowNum = 0;
            i = 0;

            // Add only the strings from a command into a 1D temporary array
            while (rowNum < 20) {
                if (*(currNode -> command[rowNum]) != '\0') {
                    temp[i] = currNode -> command[rowNum];
                    i += 1;
                }

                rowNum += 1;
            }

            i -= 1;

            // If the last character of the command is a newline, replace it with a \0
            char lastString[sizeof(temp[i])];
            strcpy(lastString, temp[i]);
            if (lastString[strlen(lastString) - 1] == '\n') {
                lastString[strlen(lastString) - 1] = '\0';
            }

            // Copy the commands into a new array, but insert the last part of the command with the string that
            // contains \0 as the last character
            char *commands[i+1];
            for (j = 0; j <= i; j++) {
                if (j == i) {
                    commands[j] = (char *) malloc(sizeof(lastString));
                    strcpy(commands[j], lastString);
                }
                else {
                    commands[j] = temp[j];
                }
            }

            // End the command array with NULL at the end
            commands[i+1] = NULL;

            printf("Starting command %d: child %d pid of parent %d\n", currNode->index, getpid(), getppid());

//            for (j = 0; j <= i; j++) {
//                printf("***%s***", commands[j]);
//            }
//            printf("\n");
//            printf("***%s***\n", commands[0]);

            // Execute the command
            execvp(commands[0], commands);

            // If anything went wrong, print
            fprintf(stderr, "Couldn't execute command\n");
            return 2;
        }

        // Parent process
        else {
            int status;

            if ((pid = waitpid(pid, &status, 0)) < 0) {
                fprintf(stderr, "Waitpid error\n");
                return 3;
            }

            clock_gettime(CLOCK_MONOTONIC, &finish);
            elapsed = (finish.tv_sec - (currNode -> starttime));

            if (elapsed < 2) {
                currNode -> active = false;
                fprintf(stderr, "Exit with exitcode = %d\n", WIFEXITED(status));
                fprintf(stderr, "Spawning too fast\n\n");
            }
            else {
                currNode = FindCommand(head, pid);
            }
        }

        currNode = GetNextCommand(currNode);
    }

    return 0;
}