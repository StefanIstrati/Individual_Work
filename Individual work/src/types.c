#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h> 
#include <stdbool.h>
#include "include/types.h"

SymbolTableEntry* symbolTable = NULL;  

TreeNode* createNode(TokenType type, TokenType varType, const char* varName, const char* value, TreeNode* left, TreeNode* right) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    newNode->type = type;
    newNode->varName = (varName != NULL) ? strdup(varName) : NULL;
    newNode->value = (value != NULL) ? strdup(value) : NULL;
    newNode->left = left;
    newNode->right = right;


    return newNode;
}

void setVariable(char* name, double value) {
    SymbolTableEntry* entry = symbolTable;
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    SymbolTableEntry* newEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    newEntry->name = strdup(name);
    newEntry->value = value;
    newEntry->next = symbolTable;
    symbolTable = newEntry;
}

double getVariable(char* name) {
    SymbolTableEntry* entry = symbolTable;
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    fprintf(stderr, "Variable '%s' not found\n", name);
    exit(EXIT_FAILURE); 
}

void freeSymbolTable() {
    SymbolTableEntry* entry = symbolTable;
    while (entry != NULL) {
        SymbolTableEntry* next = entry->next;
        free(entry->name);
        free(entry);
        entry = next;
    }
}

void freeTreeNode(TreeNode* root) {
    if (root != NULL) {
        freeTreeNode(root->left);
        freeTreeNode(root->right);
        free(root->varName);
        free(root->value);
        free(root);
    }
}

void freeToken(Token* token) {
    free(token->value);
}

void freeTree(TreeNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root->value);
        free(root);
    }
}

