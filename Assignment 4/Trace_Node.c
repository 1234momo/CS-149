#include <stddef.h>
#include "Trace_Node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ptr to the top of the stack
static TRACE_NODE* TRACE_TOP = NULL;

/* --------------------------------*/
// Push p on the stack
void PUSH_TRACE(char* p)
{
    TRACE_NODE* tnode;
    static char glob[] = "global";

    if (TRACE_TOP == NULL) {

        // Initialize the stack with "global" identifier
        TRACE_TOP = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

        // Print msg if malloc fails
        if (TRACE_TOP == NULL) {
            printf("PUSH_TRACE: memory allocation error\n");
            exit(1);
        }

        TRACE_TOP->functionid = glob;
        TRACE_TOP->next = NULL;
    }

    // Create the node for p
    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

    // Print msg if malloc fails
    if (tnode == NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }

    tnode->functionid = p;
    tnode->next = TRACE_TOP;    // insert tnode as the first in the list
    TRACE_TOP = tnode;          // point TRACE_TOP to the first node
}

/* --------------------------------*/
// Remove the top of the stack
void POP_TRACE() {
    TRACE_NODE* tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);
}

/* ---------------------------------------------- */
/* function PRINT_TRACE prints out the sequence of function calls that are on the stack at this instance */
/* For example, it returns a string that looks like: funcA:funcB:funcC.  */
char* PRINT_TRACE() {
    int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
    int i, length, j;
    TRACE_NODE* tnode;
    static char buf[100];

    // Stack not initialized yet, so we are still in the 'global' area
    if (TRACE_TOP == NULL) {
        strcpy(buf,"global");
        return buf;
    }

    /* peek at the depth top entries on the stack, but do not
       go over 100 chars and do not go over the bottom of the
       stack */
    sprintf(buf,"%s", TRACE_TOP->functionid);
    length = strlen(buf);   // length of the string so far

    for(i=1, tnode = TRACE_TOP->next; tnode != NULL && i < depth; i++, tnode = tnode->next) {
        // Length of what we want to add
        j = strlen(tnode->functionid);

        // Total length is ok
        if (length + j + 1 < 100) {
            sprintf(buf + length, ":%s", tnode->functionid);
            length += j + 1;
        }
        else
            break;
    }
    return buf;
}

void FreeTraceTop() {
    free(TRACE_TOP);
}