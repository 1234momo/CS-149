#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CommandNode.h"

//create a new command node. usually nextCmd can be NULL and function InsertCommandAfter can be called to insert after head node.
void CreateCommandNode(CommandNode* thisNode, char* cmd, int ind, CommandNode* nextCmd) {
    // Copy cmd into thisNodes's command
    thisNode->command = cmd;
    printf("%s\n", thisNode->command);
    thisNode->index = ind;
    thisNode->nextCommandPtr = nextCmd;
    return;
}

//insert node newNode after thisNode 
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
    CommandNode* tmpNext = NULL;

    tmpNext = thisNode->nextCommandPtr;
    thisNode->nextCommandPtr = newNode;
    newNode->nextCommandPtr = tmpNext;
    return;
}

// Print
void PrintNodes(CommandNode* node) {
    if (node == NULL) {
        return;
    }

    fprintf(stdout,"Index: %d\tCommand: %s\n", node->index, node->command);
    PrintNodes(GetNextCommand(node));
}

void FreeNodes(CommandNode* node) {
    CommandNode* nextNode = GetNextCommand(node);
    CommandNode* currNode = node;

    while (currNode != NULL) {
        free(currNode);

        if (nextNode != NULL) {
            CommandNode* tempNode = nextNode;
            nextNode = GetNextCommand(nextNode);
            currNode = tempNode;
        }
        else {
            currNode = NULL;
        }
    }
}

//get next command node in linked list
CommandNode* GetNextCommand(CommandNode* thisNode) {
    return thisNode->nextCommandPtr;
}