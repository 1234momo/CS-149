#include <stdbool.h>

typedef struct command_struct {
    char* command;
    int index;
    struct command_struct* nextCommandPtr;
} CommandNode;


void CreateCommandNode(CommandNode* thisNode, char* cmd, int ind, CommandNode* nextCmd);
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode);
void PrintNodes(CommandNode* node);
CommandNode* GetNextCommand(CommandNode* thisNode);
