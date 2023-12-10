#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct PARSER_STRUCT{
lexer_T* lexer;
token_T* current_token;
} parser_T;

typedef struct TreeNode {
    TokenType type;
    char* varName;  
    char* value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

parser_T* init_parser(lexer_T* lexer);

void parser_advance(parser_T* parser);

TreeNode* parse_expression(parser_T* parser);

TreeNode* parse_term(parser_T* parser);

TreeNode* parse_factor(parser_T* parser);

TreeNode* parse_statement(parser_T* parser);

TreeNode* parse_print(parser_T* parser);

TreeNode* parse_assignment(parser_T* parser);

TreeNode* parse_declaration(parser_T* parser, TokenType tokenType);

TreeNode* parse_while_loop(parser_T* parser);

TreeNode* parse_if_statement(parser_T* parser);

TreeNode* parse_conditional_expression(parser_T* parser);

TreeNode* create_node(TokenType type, TokenType varType, const char* varName, const char* value, TreeNode* left, TreeNode* right);

void free_tree_node(TreeNode* root);

void free_tree(TreeNode* root);
#endif