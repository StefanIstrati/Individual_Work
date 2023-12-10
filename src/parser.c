#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/parser.h"
#include "include/interpretor.h"

parser_T* init_parser(lexer_T* lexer){
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
}

void parser_advance(parser_T* parser){
    parser->current_token = lexer_get_next_token(parser->lexer);
}



TreeNode* parse_expression(parser_T* parser) {
    TreeNode* left, * right;
    TokenType opType;

    left = parse_term(parser);  // Parse the first term

    while (parser->current_token->type == TOKEN_SUM || parser->current_token->type == TOKEN_DIF) {
        opType = parser->current_token->type;
        parser_advance(parser);
        right = parse_term(parser);  // Parse the next term

        left = create_node(opType, opType, "", "", left, right);
    }

    return left;
}

TreeNode* parse_term(parser_T* parser) {
    TreeNode* left, * right;
    TokenType opType;

    left = parse_factor(parser);  // Parse the first factor

    while (parser->current_token->type == TOKEN_MULT || parser->current_token->type == TOKEN_DIV) {
        opType = parser->current_token->type;
        parser_advance(parser);
        right = parse_factor(parser);  // Parse the next factor

        left = create_node(opType, opType, "", "", left, right);
    }

    return left;
}

TreeNode* parse_factor(parser_T* parser) {
    TreeNode* node;

    if (parser->current_token->type == TOKEN_INT || parser->current_token->type == TOKEN_FLOAT || parser->current_token->type == TOKEN_ID) {
        char* value = strdup(parser->current_token->value);
        TokenType nodeType = parser->current_token->type;  
        node = create_node(nodeType, nodeType, value, value, NULL, NULL);
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_LPAREN) {
        int par = 0;
        parser_advance(parser);
        node = parse_expression(parser);
        
        if (parser->current_token->type == TOKEN_RPAREN) {
            parser_advance(parser);
           return node;
        }

    } else {
        fprintf(stderr, "Error: Unexpected token in factor : %s\n",parser->current_token->value);
        exit(EXIT_FAILURE);
    }

    return node;
}


TreeNode* parse_statement(parser_T* parser) {
    TreeNode* statementNode = NULL;
    switch (parser->current_token->type) {
        case TOKEN_INT_DECLAR:
            statementNode = parse_declaration(parser, TOKEN_INT);
            break;
        case TOKEN_FLOAT_DECLAR:
            statementNode = parse_declaration(parser, TOKEN_FLOAT);
            break;
        case TOKEN_ID:
            statementNode = parse_assignment(parser);
            break;
        case TOKEN_WHILE:
            statementNode = parse_while_loop(parser);
            break;
        case TOKEN_IF:
            statementNode = parse_if_statement(parser);
            break;
        case TOKEN_ENDWHILE:
            parser_advance(parser);
            break;
        case TOKEN_PRINT:
            statementNode = parse_print(parser);
        case TOKEN_EOF:
            break;
        default:
            fprintf(stderr, "Error: Invalid or unexpected token in statement\n");
            exit(EXIT_FAILURE);
    }

    if (parser->current_token->type != TOKEN_SEMI) {
        fprintf(stderr, "Error: Expected ';' at the end of statement\n");
        exit(EXIT_FAILURE);
    }
    parser_advance(parser);
    return statementNode;
}

TreeNode* parse_print(parser_T* parser) {
    if (parser->current_token->type != TOKEN_PRINT) {
        fprintf(stderr, "Error: Expected 'print' keyword\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser);
    

    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after 'print'\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser);
    char* varName = strdup(parser->current_token->value);
    parser_advance(parser);
    if (parser->current_token->type != TOKEN_RPAREN) {

        fprintf(stderr, "Error: Expected ')' after expression inside 'print'\n");
        exit(EXIT_FAILURE);
    }
    parser_advance(parser);
    TreeNode* PrintNode = create_node(TOKEN_PRINT, TOKEN_INT, varName, "", NULL, NULL);
    return PrintNode;
}


TreeNode* parse_assignment(parser_T* parser) {
    if (parser->current_token->type != TOKEN_ID) {
        fprintf(stderr, "Error: Expected a variable identifier\n");
        exit(EXIT_FAILURE);
    }

    char* varName = strdup(parser->current_token->value);
    parser_advance(parser);

    if (parser->current_token->type != TOKEN_EQUAL) {
        fprintf(stderr, "Error: Expected '=' after variable identifier\n");
        exit(EXIT_FAILURE);
    }
    parser_advance(parser);
    TreeNode* expr = parse_expression(parser);

    TreeNode* assignmentNode = create_node(TOKEN_ASSIGNMENT, TOKEN_EQUAL, varName, "", expr, NULL);
    return assignmentNode;
}

TreeNode* parse_declaration(parser_T* parser, TokenType tokenType) {
    TreeNode* newNode = malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error in parse_declaration\n");
        exit(EXIT_FAILURE);
    }

    newNode->type = parser->current_token->type;

    parser_advance(parser);
    if (parser->current_token->type != TOKEN_ID) {
        fprintf(stderr, "Error: Expected variable name after type in declaration\n");
        exit(EXIT_FAILURE);
    }

    char* varName = strdup(parser->current_token->value);
    newNode->varName = varName;

    parser_advance(parser);

    if (parser->current_token->type != TOKEN_EQUAL) {
        set_variable(varName, 0);
        newNode->left = NULL;
        newNode->right = NULL;
    } else {
        parser_advance(parser);
        TreeNode* leftNode = parse_expression(parser);
        if (newNode->type == TOKEN_INT_DECLAR && tokenType != leftNode->type) {
            fprintf(stderr, "Type Declaration Error\n");
            exit(EXIT_FAILURE);
        }
        newNode->left = leftNode;
        newNode->right = NULL;
        double value = evaluate_node(newNode->left);

        set_variable(varName, value);
        newNode->value = NULL;
    }

    if (parser->current_token->type != TOKEN_SEMI) {
        fprintf(stderr, "Error: Expected ';' at the end of variable declaration\n");
        exit(EXIT_FAILURE);
    }
    return newNode;
}

TreeNode* parse_while_loop(parser_T* parser) {

    if (parser->current_token->type != TOKEN_WHILE) {
        fprintf(stderr, "Error: Expected 'while' keyword\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser);

    TreeNode* condition = parse_conditional_expression(parser);

    TreeNode* body = NULL;
    TreeNode** bodyPtr = &body; 
    parser_advance(parser);

    while (parser->current_token->type != TOKEN_ENDWHILE) {
        TreeNode* statement = parse_statement(parser);
        *bodyPtr = statement; 
        bodyPtr = &(statement->right);
    }

    if (parser->current_token->type != TOKEN_ENDWHILE) {
        fprintf(stderr, "Error: Expected 'endwhile'\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser); 

    if (parser->current_token->type != TOKEN_SEMI) {
        fprintf(stderr, "Error: Expected ';' after endwhile\n");
        exit(EXIT_FAILURE);
    }

    TreeNode* whileNode = create_node(TOKEN_WHILE, TOKEN_INT, "", "", condition, body);
    return whileNode;
}

TreeNode* parse_if_statement(parser_T* parser) {
    if (parser->current_token->type != TOKEN_IF) {
        fprintf(stderr, "Error: Expected 'if' keyword\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser);

    TreeNode* condition = parse_conditional_expression(parser);

    TreeNode* ifBody = NULL;
    TreeNode** ifBodyPtr = &ifBody; 
    parser_advance(parser);

    while (parser->current_token->type != TOKEN_ELSE && parser->current_token->type != TOKEN_ENDIF) {
        TreeNode* statement = parse_statement(parser);
        *ifBodyPtr = statement; 
        ifBodyPtr = &(statement->right); 
    }

    TreeNode* elseBody = NULL;
    TreeNode** elseBodyPtr = &elseBody; 
    if (parser->current_token->type == TOKEN_ELSE) {
        parser_advance(parser);

        while (parser->current_token->type != TOKEN_ENDIF) {
            TreeNode* statement = parse_statement(parser);
            *elseBodyPtr = statement; 
            elseBodyPtr = &(statement->right); 
        }
    }

    if (parser->current_token->type != TOKEN_ENDIF) {
        fprintf(stderr, "Error: Expected 'end' after if statement\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser); 

    if (parser->current_token->type != TOKEN_SEMI) {
        fprintf(stderr, "Error: Expected ';' after endif\n");
        exit(EXIT_FAILURE);
    }

    TreeNode* ifNode = create_node(TOKEN_IF, TOKEN_INT, NULL, NULL, condition, ifBody);
    TreeNode* elseNode = (elseBody != NULL) ? create_node(TOKEN_ELSE, TOKEN_INT, NULL, NULL, NULL, elseBody) : NULL;

    TreeNode* ifElseNode = create_node(TOKEN_IF, TOKEN_INT, NULL, NULL, ifNode, elseNode);

    return ifElseNode;
}

TreeNode* parse_conditional_expression(parser_T* parser) {
    TreeNode* left = parse_expression(parser);

    if (parser->current_token->type == TOKEN_LESS || parser->current_token->type == TOKEN_GREATER ||
        parser->current_token->type == TOKEN_LESS_EQUAL || parser->current_token->type == TOKEN_GREATER_EQUAL ||
        parser->current_token->type == TOKEN_EQUALITY || parser->current_token->type == TOKEN_NOT_EQUAL) {
        TokenType operatorType = parser->current_token->type;
        TreeNode* newNode = malloc(sizeof(TreeNode));
        newNode->type = operatorType;
        newNode->value = parser->current_token->value;
        newNode->varName = NULL;
        newNode->left = left;
        parser_advance(parser);
        newNode->right = parse_expression(parser);

        return newNode;
    }

    return left;
}

TreeNode* create_node(TokenType type, TokenType varType, const char* varName, const char* value, TreeNode* left, TreeNode* right) {
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

void free_tree_node(TreeNode* root) {
    if (root != NULL) {
        free_tree_node(root->left);
        free_tree_node(root->right);
        free(root->varName);
        free(root->value);
        free(root);
    }
}


void free_tree(TreeNode* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root->value);
        free(root);
    }
}

 