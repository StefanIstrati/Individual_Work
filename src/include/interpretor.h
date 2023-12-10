#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include "parser.h"

typedef struct INTERPRETOR_STRUCT {
    char* name;
    double value;
    struct INTERPRETOR_STRUCT* next;
} interpretor_T;

extern interpretor_T* variable;  

void set_variable(char* name, double value);

double get_variable(char* name);

void free_variable();

double evaluate_node(TreeNode* root);

bool is_relational_operator(TokenType type);

bool evaluate_conditional(TreeNode* root);

#endif