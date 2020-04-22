#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "CommandNode.h"
#include "Trace_Node.h"

// -----------------------------------------
// function REALLOC calls realloc
// For instance, example of print out:
// "File tracemem.c, line X, function F reallocated the memory segment at address A to a new size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* REALLOC(void* p, int newSize, char* file, int line) {
    p = realloc(p, newSize);
    printf("File %s, line %d, function %s reallocated the memory segment at address %p to a new size %d\n", file, line, PRINT_TRACE, p, newSize);
    return p;
}

// -------------------------------------------
// function MALLOC calls malloc
// For instance, example of print out:
// "File tracemem.c, line X, function F allocated new memory segment at address A to size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* MALLOC(int size, char* file, int line) {
    void* p = malloc(size);
    printf("File %s, line %d, function %s allocated new memory segment at address %p to size %d\n", file, line, PRINT_TRACE, p, size);
    return p;
}

// ----------------------------------------------
// function FREE calls free
// For instance, example of print out:
// "File tracemem.c, line X, function F deallocated the memory segment at address A"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void FREE(void* p,char* file,int line) {
    free(p);
    printf("File %s, line %d, function %s deallocated the memory segment at address %p\n", file, line, PRINT_TRACE, p);
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)

// -----------------------------------------
// Add an extra column to a 2d array of ints.
// Demonstrates how memory usage tracing of realloc is done
void add_column(int** array,int rows,int columns) {
    PUSH_TRACE("add_column");
    int i;

    for(i = 0; i < rows; i++) {
        array[i] = (int*) realloc(array[i],sizeof(int) * (columns + 1));
        array[i][columns] = 10 * i + columns;
    }
    POP_TRACE();
    return;
}

// ------------------------------------------
// Example of how the memory trace is done
// Demonstrates how memory usage tracing of malloc and free is done
void make_extend_array() {
    PUSH_TRACE("make_extend_array");
    int i, j;
    int **array;

    // Make array
    array = (int**) malloc(sizeof(int*) * 4);  // 4 rows
    for(i = 0; i < 4; i++) {
        array[i] = (int*) malloc(sizeof(int) * 3);  // 3 columns
        for(j = 0; j < 3; j++)
            array[i][j] = 10 * i + j;
    }

    // Display array
    for(i = 0; i < 4; i++)
        for(j = 0; j < 3; j++)
            printf("array[%d][%d]=%d\n", i, j, array[i][j]);

    // Add a new column
    add_column(array,4,3);

    // Display the array again
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            printf("array[%d][%d]=%d\n", i, j, array[i][j]);

    // Deallocate the array
    for(i = 0; i < 4; i++)
        free((void*) array[i]);
    free((void*) array);

    POP_TRACE();
    return;
}

int main(int argc, char *argv[]) {
    if (argc > 2 || argc < 1) {
        printf("Number of files inputted must be 1");
        exit(EXIT_FAILURE);
    }

    // Initialize variables for reading file
    FILE * fp;
    char * line = NULL;
    size_t length = 0;
    ssize_t read;

    // Initialize char** array
    char** fileCommands = (char**) malloc(sizeof(char*) * 10);
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

        // Insert command read from file into char** array
        fileCommands[index] = line;

        // Create linkedlist
        currNode = (CommandNode*) malloc(sizeof(CommandNode));
        CreateCommandNode(currNode, line, index, NULL);
        InsertCommandAfter(prevNode, currNode);
        prevNode = currNode;
        if (index == 0) {
            head = currNode;
        }

        ++index;
    }

    // Close file handler
    fclose(fp);



    return 0;
}
