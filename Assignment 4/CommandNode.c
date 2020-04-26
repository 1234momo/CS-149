#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CommandNode.h"

// Create a new command node
// Usually nextCmd can be NULL and function InsertCommandAfter can be called to insert after head node.
void CreateCommandNode(CommandNode* thisNode, char* cmd, int ind, CommandNode* nextCmd) {
    // Copy cmd into thisNodes's command
    thisNode->command = cmd;
    thisNode->index = ind;
    thisNode->nextCommandPtr = nextCmd;
    return;
}

// Insert node newNode after thisNode
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
    CommandNode* tmpNext = NULL;

    tmpNext = thisNode->nextCommandPtr;
    thisNode->nextCommandPtr = newNode;
    newNode->nextCommandPtr = tmpNext;
    return;
}

// Recursively print contents of linkedlist
void PrintNodes(CommandNode* node) {
    if (node == NULL) {
        return;
    }

    fprintf(stdout,"Index: %d\tCommand: %s\n", node->index, node->command);
    PrintNodes(GetNextCommand(node));
}

// Free the linked list
void FreeNodes(CommandNode* node) {
    CommandNode* nextNode = GetNextCommand(node);
    CommandNode* currNode = node;

    // Traverse through the linkedlist
    while (currNode != NULL) {
        free(currNode);

        // Set nextNode and currNode
        if (nextNode != NULL) {
            CommandNode* tempNode = nextNode;
            nextNode = GetNextCommand(nextNode);
            currNode = tempNode;
        }

        // Set currNode to NULL if nextNode is NULL
        // nextNode being NULL indicates that currNode is the last node
        else {
            currNode = NULL;
        }
    }
}

// Get next command node in linked list
CommandNode* GetNextCommand(CommandNode* thisNode) {
    return thisNode->nextCommandPtr;
}