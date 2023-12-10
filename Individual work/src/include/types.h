#ifndef TYPES_H
#define TYPES_H

#include "token.h"

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct TreeNode {
    TokenType type;
    char* varName;  
    char* value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct SymbolTableEntry {
    char* name;
    double value;
    struct SymbolTableEntry* next;
} SymbolTableEntry;

extern SymbolTableEntry* symbolTable;  
extern Token currentToken;

TreeNode* createNode(TokenType type, TokenType varType, const char* varName, const char* value, TreeNode* left, TreeNode* right);

void setVariable(char* name, double value);

double getVariable(char* name);

void freeSymbolTable();

void freeTreeNode(TreeNode* root);

void freeToken(Token* token);

void freeTree(TreeNode* root);

void printAllVariables();

#endif
