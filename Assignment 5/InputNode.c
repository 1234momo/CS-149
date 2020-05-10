#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "InputNode.h"

// Create a new command node
// Usually nextCmd can be NULL and function InsertCommandAfter can be called to insert after head node.
void CreateInputNode(InputNode* thisNode, char* input, InputNode* nextCmd) {
    // Copy input into thisNodes's command
    thisNode->input = input;
    thisNode->nextNodePtr = nextCmd;
    return;
}

// Insert node newNode after thisNode
void InsertNodeBefore(InputNode* thisNode, InputNode* newNode) {
    newNode->nextNodePtr = thisNode;
}

// Free the linked list
void FreeNodes(InputNode* node) {
    InputNode* nextNode = GetNextNode(node);
    InputNode* currNode = node;

    // Traverse through the linkedlist
    while (currNode != NULL) {
        free(currNode);

        // Set nextNode and currNode
        if (nextNode != NULL) {
            InputNode* tempNode = nextNode;
            nextNode = GetNextNode(nextNode);
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
InputNode* GetNextNode(InputNode* thisNode) {
    return thisNode->nextNodePtr;
}