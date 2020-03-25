#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>
#include "CommandNode.h"

int main(int argc, char *argv[]) {
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

        // Replace \n to NULL
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = 0;
        }

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

    // Initialize variables for timer
    struct timespec start, finish;
    double elapsed;

    // Begin creating processes for each node in the linked list
    currNode = head;
    pid_t pid;
    while (currNode != NULL) {
        pid = fork();

        // Print message if fork fails
        if (pid < 0) {
            fprintf(stderr, "fork error\n");
            return 1;
        }

        char outFileName[10];
        char errFileName[10];

        sprintf(outFileName, "%d.out", (currNode->index) + 1);
        sprintf(errFileName, "%d.err", (currNode->index) + 1);

        int outFile = open(outFileName, O_RDWR | O_CREAT | O_APPEND);
        int errFile = open(errFileName, O_RDWR | O_CREAT | O_APPEND);

        chmod(outFileName, S_IRWXU);
        chmod(errFileName, S_IRWXU);

        dup2(outFile, STDOUT_FILENO);
        dup2(errFile, STDERR_FILENO);

        // Begin timer and store start time into the current node
        clock_gettime(CLOCK_MONOTONIC, &start);
        currNode -> starttime = start.tv_sec;

        // Child process
        if (pid == 0) {
            currNode -> active = true;

            char *commands[20];
            int index = 0;

            // Add only the strings from command into a 1D commands array
            while (rowNum < 20 && *(currNode -> command[index]) != '\0') {
                commands[index] = currNode -> command[index];
                index += 1;
            }

            // End the command array with NULL at the end
            commands[index] = NULL;

            fprintf(stdout, "Starting command %d: child %d pid of parent %d\n", currNode->index, getpid(), getppid());
            fflush(stdout);

            // Execute the command
            execvp(commands[0], commands);

            // If anything went wrong, print
            perror(commands[0]);
            exit(2);
        }

            // Parent process saves pid of child to proper node
        else {
            currNode -> PID = pid;
        }

        currNode = GetNextCommand(currNode);
    }

    int status;
    while ((pid = wait(&status)) >= 0) {

        CommandNode *finishedNode = FindCommand(head, pid);

        finishedNode -> active = false;

        // Calculate time taken for command to run
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - (finishedNode -> starttime));

        char outFileName[10];
        char errFileName[10];

        sprintf(outFileName, "%d.out", (finishedNode->index) + 1);
        sprintf(errFileName, "%d.err", (finishedNode->index) + 1);

        int outFile = open(outFileName, O_RDWR | O_CREAT | O_APPEND);
        int errFile = open(errFileName, O_RDWR | O_CREAT | O_APPEND);

        dup2(outFile, STDOUT_FILENO);
        dup2(errFile, STDERR_FILENO);

        if (elapsed > 2) {
            pid = fork();

            // Begin timer and store start time into the current node
            clock_gettime(CLOCK_MONOTONIC, &start);
            finishedNode -> starttime = start.tv_sec;

            if (pid < 0) {
                fprintf (stderr, "fork failed\n");
                return 3;
            }

            if (pid == 0) {
                finishedNode -> active = true;

                char *commands[20];
                int index = 0;

                // Add only the strings from command into a 1D commands array
                while (rowNum < 20 && *(finishedNode -> command[index]) != '\0') {
                    commands[index] = finishedNode -> command[index];
                    index += 1;
                }

                // End the command array with NULL at the end
                commands[index] = NULL;

                fprintf(stdout, "Starting command %d: child %d pid of parent %d\n", currNode->index, getpid(), getppid());
                fflush(stdout);

                // Execute the command
                execvp(commands[0], commands);

                // If anything went wrong, print
                perror(commands[0]);
                exit(2);
            }

                // Parent process saves pid of child to proper node
            else {
                finishedNode -> PID = pid;
            }
        }

            // If process executes less than 2 seconds
        else {
            if(pid > 0) {
                fprintf(stderr, "Spawning too fast\n");

                if (WIFEXITED(status)) {
                    fprintf(stderr, "Exit with exitcode = %d\n", WEXITSTATUS(status));
                }
                else if (WIFSIGNALED(status)) {
                    fprintf(stderr, "Killed with signal %d\n", WTERMSIG(status));
                }
            }
        }
    }

    return 0;
}