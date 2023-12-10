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



TreeNode* parseExpression(parser_T* parser) {
    TreeNode* left, * right;
    TokenType opType;

    left = parseTerm(parser);  // Parse the first term

    while (parser->current_token->type == TOKEN_SUM || parser->current_token->type == TOKEN_DIF) {
        opType = parser->current_token->type;
        parser_advance(parser);
        right = parseTerm(parser);  // Parse the next term

        left = createNode(opType, opType, "", "", left, right);
    }

    return left;
}

TreeNode* parseTerm(parser_T* parser) {
    TreeNode* left, * right;
    TokenType opType;

    left = parseFactor(parser);  // Parse the first factor

    while (parser->current_token->type == TOKEN_MULT || parser->current_token->type == TOKEN_DIV) {
        opType = parser->current_token->type;
        parser_advance(parser);
        right = parseFactor(parser);  // Parse the next factor

        left = createNode(opType, opType, "", "", left, right);
    }

    return left;
}

TreeNode* parseFactor(parser_T* parser) {
    TreeNode* node;

    if (parser->current_token->type == TOKEN_INT || parser->current_token->type == TOKEN_FLOAT || parser->current_token->type == TOKEN_ID) {
        char* value = strdup(parser->current_token->value);
        TokenType nodeType = parser->current_token->type;  
        node = createNode(nodeType, nodeType, value, value, NULL, NULL);
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_LPAREN) {
        int par = 0;
        parser_advance(parser);
        node = parseExpression(parser);
        
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


TreeNode* parseStatement(parser_T* parser) {
    TreeNode* statementNode = NULL;
    switch (parser->current_token->type) {
        case TOKEN_INT_DECLAR:
            statementNode = parseDeclaration(parser, TOKEN_INT);
            break;
        case TOKEN_FLOAT_DECLAR:
            statementNode = parseDeclaration(parser, TOKEN_FLOAT);
            break;
        case TOKEN_ID:
            statementNode = parseAssignment(parser);
            break;
        case TOKEN_WHILE:
            statementNode = parseWhileLoop(parser);
            break;
        case TOKEN_IF:
            statementNode = parseIfStatement(parser);
            break;
        case TOKEN_ENDWHILE:
            parser_advance(parser);
            break;
        case TOKEN_PRINT:
            statementNode = parsePrint(parser);
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

TreeNode* parsePrint(parser_T* parser) {
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
    TreeNode* PrintNode = createNode(TOKEN_PRINT, TOKEN_INT, varName, "", NULL, NULL);
    return PrintNode;
}


TreeNode* parseAssignment(parser_T* parser) {
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
    TreeNode* expr = parseExpression(parser);

    TreeNode* assignmentNode = createNode(TOKEN_ASSIGNMENT, TOKEN_EQUAL, varName, "", expr, NULL);
    return assignmentNode;
}

TreeNode* parseDeclaration(parser_T* parser, TokenType tokenType) {
    TreeNode* newNode = malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error in parseDeclaration\n");
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
        setVariable(varName, 0);
        newNode->left = NULL;
        newNode->right = NULL;
    } else {
        parser_advance(parser);
        TreeNode* leftNode = parseExpression(parser);
        if (newNode->type == TOKEN_INT_DECLAR && tokenType != leftNode->type) {
            fprintf(stderr, "Type Declaration Error\n");
            exit(EXIT_FAILURE);
        }
        newNode->left = leftNode;
        newNode->right = NULL;
        double value = evaluate(newNode->left);

        setVariable(varName, value);
        newNode->value = NULL;
    }

    if (parser->current_token->type != TOKEN_SEMI) {
        fprintf(stderr, "Error: Expected ';' at the end of variable declaration\n");
        exit(EXIT_FAILURE);
    }
    return newNode;
}

TreeNode* parseWhileLoop(parser_T* parser) {

    if (parser->current_token->type != TOKEN_WHILE) {
        fprintf(stderr, "Error: Expected 'while' keyword\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser);

    TreeNode* condition = parseRelationalExpression(parser);

    TreeNode* body = NULL;
    TreeNode** bodyPtr = &body; 
    parser_advance(parser);

    while (parser->current_token->type != TOKEN_ENDWHILE) {
        TreeNode* statement = parseStatement(parser);
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

    TreeNode* whileNode = createNode(TOKEN_WHILE, TOKEN_INT, "", "", condition, body);
    return whileNode;
}

TreeNode* parseIfStatement(parser_T* parser) {
    if (parser->current_token->type != TOKEN_IF) {
        fprintf(stderr, "Error: Expected 'if' keyword\n");
        exit(EXIT_FAILURE);
    }

    parser_advance(parser);

    TreeNode* condition = parseRelationalExpression(parser);

    TreeNode* ifBody = NULL;
    TreeNode** ifBodyPtr = &ifBody; 
    parser_advance(parser);

    while (parser->current_token->type != TOKEN_ELSE && parser->current_token->type != TOKEN_ENDIF) {
        TreeNode* statement = parseStatement(parser);
        *ifBodyPtr = statement; 
        ifBodyPtr = &(statement->right); 
    }

    TreeNode* elseBody = NULL;
    TreeNode** elseBodyPtr = &elseBody; 
    if (parser->current_token->type == TOKEN_ELSE) {
        parser_advance(parser);

        while (parser->current_token->type != TOKEN_ENDIF) {
            TreeNode* statement = parseStatement(parser);
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

    TreeNode* ifNode = createNode(TOKEN_IF, TOKEN_INT, NULL, NULL, condition, ifBody);
    TreeNode* elseNode = (elseBody != NULL) ? createNode(TOKEN_ELSE, TOKEN_INT, NULL, NULL, NULL, elseBody) : NULL;

    TreeNode* ifElseNode = createNode(TOKEN_IF, TOKEN_INT, NULL, NULL, ifNode, elseNode);

    return ifElseNode;
}

TreeNode* parseRelationalExpression(parser_T* parser) {
    TreeNode* left = parseExpression(parser);

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
        newNode->right = parseExpression(parser);

        return newNode;
    }

    return left;
}

