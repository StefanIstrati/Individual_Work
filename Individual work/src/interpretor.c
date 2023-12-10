#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h> 
#include <stdbool.h>
#include "include/interpretor.h"
#include "include/types.h"

double evaluate(TreeNode* root) {
    if (root == NULL) {
        return 0.0; 
    }
    switch (root->type) {
        case TOKEN_ID:
        case TOKEN_INT_DECLAR:
        case TOKEN_FLOAT_DECLAR:
            return getVariable(root->varName);
        case TOKEN_PRINT:
            printf("%s : %g\n",root->varName,getVariable(root->varName));
            break;
        case TOKEN_INT:
        case TOKEN_FLOAT:
            return atof(root->value);
        case TOKEN_SUM:
            return evaluate(root->left) + evaluate(root->right);
        case TOKEN_DIF:
            return evaluate(root->left) - evaluate(root->right);
        case TOKEN_MULT:
            return evaluate(root->left) * evaluate(root->right);
        case TOKEN_ASSIGNMENT:
        {
            double result = evaluate(root->left);
            setVariable(root->varName, result);
            if (root->right != NULL)
                evaluate(root->right);
            return result;
        }
        case TOKEN_DIV:
            {
                double divisor = evaluate(root->right);
                if (divisor == 0.0) {
                    fprintf(stderr, "Error: Division by zero\n");
                    exit(EXIT_FAILURE); 
                }
                return evaluate(root->left) / divisor;
            }
        case TOKEN_WHILE:
            while (evaluateRelational(root->left)) {
                evaluate(root->right);
            }
            return 0.0;
        case TOKEN_IF: {
            if (evaluateRelational(root->left->left)) {
                evaluate(root->left->right);
            } else if (root->right != NULL) {
                evaluate(root->right->right);
            }
            return 0.0;  
        }
        case TOKEN_ELSE:
            return evaluate(root->right);
        case TOKEN_ENDIF:
            return 0.0;
        case TOKEN_LESS:
            return evaluate(root->left) < evaluate(root->right) ? 1.0 : 0.0;

        case TOKEN_GREATER:
            return evaluate(root->left) > evaluate(root->right) ? 1.0 : 0.0;

        case TOKEN_LESS_EQUAL:
            return evaluate(root->left) <= evaluate(root->right) ? 1.0 : 0.0;

        case TOKEN_GREATER_EQUAL:
            return evaluate(root->left) >= evaluate(root->right) ? 1.0 : 0.0;

        case TOKEN_EQUALITY:
            return evaluate(root->left) == evaluate(root->right) ? 1.0 : 0.0;

        case TOKEN_NOT_EQUAL:
            return evaluate(root->left) != evaluate(root->right) ? 1.0 : 0.0;
        default:
            fprintf(stderr, "Error: Invalid token type\n");
            exit(EXIT_FAILURE);
    }

    return 0.0; 
}

bool isRelationalOperator(TokenType type) {
    return type == TOKEN_LESS || type == TOKEN_GREATER || type == TOKEN_LESS_EQUAL ||
           type == TOKEN_GREATER_EQUAL || type == TOKEN_EQUALITY || type == TOKEN_NOT_EQUAL;
}

bool evaluateRelational(TreeNode* root) {
    if (root != NULL && isRelationalOperator(root->type)) {
        double leftValue = evaluate(root->left);
        double rightValue = evaluate(root->right);

        switch (root->type) {
            case TOKEN_LESS:         
                return leftValue < rightValue;
            case TOKEN_GREATER:      
                return leftValue > rightValue;
            case TOKEN_LESS_EQUAL:   
                return leftValue <= rightValue;
            case TOKEN_GREATER_EQUAL:
                return leftValue >= rightValue;
            case TOKEN_EQUALITY:  
                return leftValue == rightValue;
            case TOKEN_NOT_EQUAL:    
                return leftValue != rightValue;
            default:
                fprintf(stderr, "Error: Invalid relational operator\n");
                exit(EXIT_FAILURE);
        }
    }
    return false;  
}
