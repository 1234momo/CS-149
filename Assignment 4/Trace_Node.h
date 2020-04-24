#ifndef TRACENODE_H
#define TRACENODE_H

// TRACE_NODE_STRUCT is a linked list of pointers to function identifiers
typedef struct TRACE_NODE_STRUCT {
    char* functionid;                         // ptr to function identifier
    struct TRACE_NODE_STRUCT* next;           // ptr to next frama
} TRACE_NODE;

// Remove the top of the stack
void POP_TRACE();

// Push p on the stack
void PUSH_TRACE(char* p);

/* function PRINT_TRACE prints out the sequence of function calls that are on the stack at this instance */
/* For example, it returns a string that looks like: funcA:funcB:funcC.  */
char* PRINT_TRACE();

void FreeTraceTop();

#endif