#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "CommandNode.h"
#include "Trace_Node.h"

// -----------------------------------------
// function REALLOC calls realloc
// For instance, example of print out:
// "File tracemem.c, line X, function F reallocated the memory segment at address A to a new size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* REALLOC(void* p, int newSize, char* file, int line) {
    p = realloc(p, newSize);
    fprintf(stdout,"File %s, line %d, function %s reallocated the memory segment at address %p to a new size %d\n", file, line, PRINT_TRACE(), p, newSize);
    return p;
}

// -------------------------------------------
// function MALLOC calls malloc
// For instance, example of print out:
// "File tracemem.c, line X, function F allocated new memory segment at address A to size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* MALLOC(int size, char* file, int line) {
    void* p = malloc(size);
    fprintf(stdout,"File %s, line %d, function %s allocated new memory segment at address %p to size %d\n", file, line, PRINT_TRACE(), p, size);
    return p;
}

// ----------------------------------------------
// function FREE calls free
// For instance, example of print out:
// "File tracemem.c, line X, function F deallocated the memory segment at address A"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void FREE(void* p,char* file,int line) {
    free(p);
    fprintf(stdout,"File %s, line %d, function %s deallocated the memory segment at address %p\n", file, line, PRINT_TRACE(), p);
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)

// -----------------------------------------
// Add an extra column to a 2d array of ints.
// Demonstrates how memory usage tracing of realloc is done
char** add_columns(char** array, int rows, int columns) {
    PUSH_TRACE("add_columns");

    for(int i = 0; i < rows; i++) {
        array[i] = (char*) realloc(array[i],sizeof(char) * (columns + 1));
    }

    POP_TRACE();
    return array;
}

// -----------------------------------------
// Add an extra column to a 2d array of ints.
// Demonstrates how memory usage tracing of realloc is done
char** add_row(char** array, int rows) {
    PUSH_TRACE("add_row");
    array = (char**) realloc(array, sizeof(char*) * (rows + 1));
    POP_TRACE();
    return array;
}

// ------------------------------------------
// Allocates memory for the char** array
char** allocate_array_mem(char** array, int rows, int columns) {
    PUSH_TRACE("allocate_array_mem");

    // Make array
    array = (char**) malloc(sizeof(char*) * rows);

    // Allocate array memory
    for(int i = 0; i < rows; i++) {
        array[i] = (char*) malloc(sizeof(char) * columns);
    }

    POP_TRACE();
    return array;
}

int main(int argc, char *argv[]) {
    if (argc > 2 || argc <= 1) {
        printf("Number of files inputted must be 1\n");
        exit(EXIT_FAILURE);
    }

    // Initialize variables for reading file
    FILE * fp;
    char * line = NULL;
    size_t length = 0;
    ssize_t read;

    // Open memtrace.out file and redirect stdout to memtrace.out
    int outFile = open("memtrace.out", O_RDWR | O_CREAT | O_APPEND);
    chmod("memtrace.out", S_IRWXU);
    dup2(outFile, STDOUT_FILENO);

    // Initialize a char** array of 10 rows and 10 columns
    char** fileCommands = NULL;
    int rows = 10, columns = 10;
    fileCommands = allocate_array_mem(fileCommands, rows, columns);
    int index = 0;

    // Initialize the linkedlist and necessary variables to help create the list
    CommandNode *head = NULL;
    CommandNode *currNode = NULL;
    CommandNode *prevNode = (CommandNode*) malloc(sizeof(CommandNode));

    // Open the file specified by the terminal
    fp = fopen(argv[1], "r");

    // Exit program if the file couldn't be opened
    if (fp == NULL) {
        printf("File can't be opened\n");
        exit(EXIT_FAILURE);
    }

    // Read each line of the file
    while ((read = getline(&line, &length, fp)) != -1) {

        // Check if command string is bigger than the size of the row
        if ((strlen(line) + 1) > columns) {
            columns = strlen(line);
            fileCommands = add_columns(fileCommands, rows, columns);
        }

        // Check if there is more rows needed
        if ((index + 1) == rows) {
            rows += 1;
            fileCommands = add_row(fileCommands, rows);
        }

        // Insert command read from file into char** array
        strcpy(fileCommands[index], line);

        // Create linkedlist
        currNode = (CommandNode*) malloc(sizeof(CommandNode));
        CreateCommandNode(currNode, fileCommands[index], index, NULL);
        InsertCommandAfter(prevNode, currNode);
        prevNode = currNode;
        if (index == 0) {
            head = currNode;
        }
        PrintNodes(head);

        index += 1;
    }

    // Deallocate the array
    for(int i = 0; i < rows; i++)
        free(fileCommands[i]);
    free(fileCommands);

    // Deallocate the linkedlist
    FreeNodes(head);
    free(head);
    free(currNode);
    free(prevNode);

    FreeTraceTop();

    // Close file handler
    fclose(fp);
    return 0;
}
