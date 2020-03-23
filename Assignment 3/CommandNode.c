/**
 *CommandNode.c
 *Author: Bill Andreopoulos
 *The function bodies for a linked list of commands
 *CS149 assignment 3 usage only
**/

#include <string.h>
#include <stdio.h>
#include "CommandNode.h"

//create a new command node. usually nextCmd can be NULL and function InsertCommandAfter can be called to insert after head node.
void CreateCommandNode(CommandNode* thisNode, char cmd[20][20], int ind, CommandNode* nextCmd, int commandLen) {
    // Copy cmd into thisNodes's command
    for(int i = 0; i < commandLen; i++) {
        strcpy(thisNode->command[i], cmd[i]);
    }

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

//get next command node in linked list
CommandNode* GetNextCommand(CommandNode* thisNode) {
    return thisNode->nextCommandPtr;
}

//find a command based on the pid
CommandNode* FindCommand(CommandNode* cmd, int pid) {
    CommandNode* tmpNext = cmd;
    while (tmpNext != NULL) {
        if (tmpNext->PID == pid) { return tmpNext; }
        tmpNext = tmpNext->nextCommandPtr;
    }
    return NULL;
}