#ifndef INPUTNODE_H
#define INPUTNODE_H

typedef struct input_struct {
    char* input;
    struct input_struct* nextNodePtr;
} InputNode;


void CreateInputNode(InputNode* thisNode, char* input, InputNode* nextCmd);
void InsertNodeBefore(InputNode* thisNode, InputNode* newNode);
InputNode* GetNextNode(InputNode* thisNode);
void FreeNodes(InputNode* node);

#endif