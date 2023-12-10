#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "types.h"

typedef struct PARSER_STRUCT{
lexer_T* lexer;
token_T* current_token;
} parser_T;

parser_T* init_parser(lexer_T* lexer);

void parser_advance(parser_T* parser);

TreeNode* parseExpression(parser_T* parser);

TreeNode* parseTerm(parser_T* parser);

TreeNode* parseFactor(parser_T* parser);

TreeNode* parseStatement(parser_T* parser);

TreeNode* parsePrint(parser_T* parser);

TreeNode* parseAssignment(parser_T* parser);

TreeNode* parseDeclaration(parser_T* parser, TokenType tokenType);

TreeNode* parseWhileLoop(parser_T* parser);

TreeNode* parseIfStatement(parser_T* parser);

TreeNode* parseRelationalExpression(parser_T* parser);

#endif