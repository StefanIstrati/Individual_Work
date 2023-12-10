#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
#include "types.h"

void interpret(TreeNode* root);
double evaluate(TreeNode* root);
bool isRelationalOperator(TokenType type);
bool evaluateRelational(TreeNode* root);

#endif