#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h> 
#include <stdbool.h>
#include "include/interpretor.h"

interpretor_T* variable = NULL;  

double evaluate_node(TreeNode* root) {
    if (root == NULL) {
        return 0.0; 
    }
    switch (root->type) {
        case TOKEN_ID:
        case TOKEN_INT_DECLAR:
        case TOKEN_FLOAT_DECLAR:
            return get_variable(root->varName);
        case TOKEN_PRINT:
            printf("%s : %g\n",root->varName,get_variable(root->varName));
            break;
        case TOKEN_INT:
        case TOKEN_FLOAT:
            return atof(root->value);
        case TOKEN_SUM:
            return evaluate_node(root->left) + evaluate_node(root->right);
        case TOKEN_DIF:
            return evaluate_node(root->left) - evaluate_node(root->right);
        case TOKEN_MULT:
            return evaluate_node(root->left) * evaluate_node(root->right);
        case TOKEN_ASSIGNMENT:
        {
            double result = evaluate_node(root->left);
            set_variable(root->varName, result);
            if (root->right != NULL)
                evaluate_node(root->right);
            return result;
        }
        case TOKEN_DIV:
            {
                double divisor = evaluate_node(root->right);
                if (divisor == 0.0) {
                    fprintf(stderr, "Error: Division by zero\n");
                    exit(EXIT_FAILURE); 
                }
                return evaluate_node(root->left) / divisor;
            }
        case TOKEN_WHILE:
            while (evaluate_conditional(root->left)) {
                evaluate_node(root->right);
            }
            return 0.0;
        case TOKEN_IF: {
            if (evaluate_conditional(root->left->left)) {
                evaluate_node(root->left->right);
            } else if (root->right != NULL) {
                evaluate_node(root->right->right);
            }
            return 0.0;  
        }
        case TOKEN_ELSE:
            return evaluate_node(root->right);
        case TOKEN_ENDIF:
            return 0.0;
        case TOKEN_LESS:
            return evaluate_node(root->left) < evaluate_node(root->right) ? 1.0 : 0.0;

        case TOKEN_GREATER:
            return evaluate_node(root->left) > evaluate_node(root->right) ? 1.0 : 0.0;

        case TOKEN_LESS_EQUAL:
            return evaluate_node(root->left) <= evaluate_node(root->right) ? 1.0 : 0.0;

        case TOKEN_GREATER_EQUAL:
            return evaluate_node(root->left) >= evaluate_node(root->right) ? 1.0 : 0.0;

        case TOKEN_EQUALITY:
            return evaluate_node(root->left) == evaluate_node(root->right) ? 1.0 : 0.0;

        case TOKEN_NOT_EQUAL:
            return evaluate_node(root->left) != evaluate_node(root->right) ? 1.0 : 0.0;
        default:
            fprintf(stderr, "Error: Invalid token type\n");
            exit(EXIT_FAILURE);
    }

    return 0.0; 
}

bool is_relational_operator(TokenType type) {
    return type == TOKEN_LESS || type == TOKEN_GREATER || type == TOKEN_LESS_EQUAL ||
           type == TOKEN_GREATER_EQUAL || type == TOKEN_EQUALITY || type == TOKEN_NOT_EQUAL;
}

bool evaluate_conditional(TreeNode* root) {
    if (root != NULL && is_relational_operator(root->type)) {
        double leftValue = evaluate_node(root->left);
        double rightValue = evaluate_node(root->right);

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

void set_variable(char* name, double value) {
    interpretor_T* entry = variable;
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    interpretor_T* newEntry = (interpretor_T*)malloc(sizeof(interpretor_T));
    newEntry->name = strdup(name);
    newEntry->value = value;
    newEntry->next = variable;
    variable = newEntry;
}

double get_variable(char* name) {
    interpretor_T* entry = variable;
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    fprintf(stderr, "Variable '%s' not found\n", name);
    exit(EXIT_FAILURE); 
}

void free_variable() {
    interpretor_T* entry = variable;
    while (entry != NULL) {
        interpretor_T* next = entry->next;
        free(entry->name);
        free(entry);
        entry = next;
    }
}
