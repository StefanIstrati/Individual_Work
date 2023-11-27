#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

union d_type {
    int INT;
    float FLOAT;
};

typedef enum {
    INTEGER,
    FLOAT
} DATA_TYPE;

typedef struct TreeNode {
    union d_type var;
    char *var_name;
    DATA_TYPE type;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef struct {
    double result;
    char *error;
} EvaluationResult;

typedef enum {
    NUMBER,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    LPAREN,  // Left parenthesis (
    RPAREN   // Right parenthesis )
} TokenType;

typedef struct {
    TokenType type;
    union {
        double number;
        char operator;
    } value;
} Token;

typedef struct {
    const char *input;
    size_t position;
    char current_char;
    Token current_token;
} Lexer;

void lexer_init(Lexer *lexer, const char *input);
Token lexer_get_next_token(Lexer *lexer);
double term(Lexer *lexer);
double factor(Lexer *lexer);
double expr(Lexer *lexer);



double factor(Lexer *lexer) {
    Token current_token = lexer->current_token;
    double result;

    if (current_token.type == NUMBER) {
        result = current_token.value.number;
        lexer->current_token = lexer_get_next_token(lexer);
    } else if (current_token.type == LPAREN) {
        lexer->current_token = lexer_get_next_token(lexer); // consume '('
        result = expr(lexer);
        
        // Check and consume the closing parenthesis ')'
        if (lexer->current_token.type == RPAREN) {
            lexer->current_token = lexer_get_next_token(lexer); // consume ')'
        } else {
            printf("Error: Missing closing parenthesis\n");
            exit(1);
        }
    } else {
        printf("Error: Unexpected token in factor\n");
        exit(1);
    }

    return result;
}

double term(Lexer *lexer) {
    double result = factor(lexer);

    while (lexer->current_token.type == MULTIPLY || lexer->current_token.type == DIVIDE) {
        Token op = lexer->current_token;
        if (op.type == MULTIPLY) {
            lexer->current_token = lexer_get_next_token(lexer);
            result *= factor(lexer);
        } else if (op.type == DIVIDE) {
            lexer->current_token = lexer_get_next_token(lexer);
            double divisor = factor(lexer);
            if (divisor != 0.0) {
                result /= divisor;
            } else {
                printf("Error: Division by zero\n");
                exit(1);
            }
        }
    }

    return result;
}

double expr(Lexer *lexer) {
    double result = term(lexer);

    while (lexer->current_token.type == ADD || lexer->current_token.type == SUBTRACT) {
        Token op = lexer->current_token;
        if (op.type == ADD) {
            lexer->current_token = lexer_get_next_token(lexer);
            result += term(lexer);
        } else if (op.type == SUBTRACT) {
            lexer->current_token = lexer_get_next_token(lexer);
            result -= term(lexer);
        }
    }

    return result;
}

void lexer_init(Lexer *lexer, const char *input) {
    lexer->input = input;
    lexer->position = 0;
    lexer->current_char = input[0];
    lexer->current_token.type = NUMBER;
    lexer->current_token.value.number = 0.0;
}

Token lexer_get_next_token(Lexer *lexer) {
    // Skip whitespace
    while (lexer->current_char != '\0' && lexer->current_char == ' ') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
    }

    if (lexer->current_char == '\0') {
        return (Token){.type = 0};
    }

    if (isdigit(lexer->current_char) || lexer->current_char == '.') {
        // Tokenize a number
        double number = 0.0;
        while (isdigit(lexer->current_char) || lexer->current_char == '.') {
            number = number * 10 + (lexer->current_char - '0');
            lexer->position++;
            lexer->current_char = lexer->input[lexer->position];
        }
        return (Token){.type = NUMBER, .value.number = number};
    } else if (lexer->current_char == '+') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
        return (Token){.type = ADD, .value.operator = '+'};
    } else if (lexer->current_char == '-') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
        return (Token){.type = SUBTRACT, .value.operator = '-'};
    } else if (lexer->current_char == '*') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
        return (Token){.type = MULTIPLY, .value.operator = '*'};
    } else if (lexer->current_char == '/') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
        return (Token){.type = DIVIDE, .value.operator = '/'};
    } else if (lexer->current_char == '(') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
        return (Token){.type = LPAREN, .value.operator = '('};
    } else if (lexer->current_char == ')') {
        lexer->position++;
        lexer->current_char = lexer->input[lexer->position];
        return (Token){.type = RPAREN, .value.operator = ')'};
    } else {
        printf("Error: Invalid character found: %c\n", lexer->current_char);
        exit(1);
    }
}

TreeNode *create_node(union d_type var, char *var_name, DATA_TYPE type) {
    TreeNode *new_node = malloc(sizeof(TreeNode));
    if (new_node == NULL) {
        exit(1);
    }
    new_node->var = var;
    new_node->var_name = strdup(var_name);
    new_node->type = type;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

TreeNode *insert_node(TreeNode *root, union d_type var, char *var_name, DATA_TYPE type) {
    if (root == NULL) {
        return create_node(var, var_name, type);
    }

    if (strcmp(var_name, root->var_name) < 0) {
        root->left = insert_node(root->left, var, var_name, type);
    } else if (strcmp(var_name, root->var_name) > 0) {
        root->right = insert_node(root->right, var, var_name, type);
    } else {
        printf("Variable %s already defined\n", var_name);
        exit(1);
    }

    return root;
}

TreeNode *find_variable(TreeNode *root, char *var_name) {
    if (root == NULL) {
        printf("Undefined variable %s\n", var_name);
        exit(1);
    }

    if (strcmp(var_name, root->var_name) == 0) {
        return root;
    } else if (strcmp(var_name, root->var_name) < 0) {
        return find_variable(root->left, var_name);
    } else {
        return find_variable(root->right, var_name);
    }
}

void Deallocate(TreeNode *root) {
    if (root != NULL) {
        Deallocate(root->left);
        Deallocate(root->right);
        free(root->var_name);
        free(root);
    }
}

void Var(TreeNode **root, char **token) {
    int i;
    if (strcmp(token[1], "int") == 0) {
        for (i = 2; token[i] != NULL; i++) {
            (*root) = insert_node(*root, (union d_type){.INT = 0}, token[i], INTEGER);
        }
    } else if (strcmp(token[1], "float") == 0) {
        for (i = 2; token[i] != NULL; i++) {
            (*root) = insert_node(*root, (union d_type){.FLOAT = 0}, token[i], FLOAT);
        }
    } else {
        printf("Undefined type of variables ");
    }
}

void Input(TreeNode **root, char **token) {
    int i;
    TreeNode *variable = find_variable(*root, token[1]);
    if (variable->type == INTEGER) {
        variable->var.INT = atoi(token[3]);
    } else if (variable->type == FLOAT) {
        variable->var.FLOAT = atof(token[3]);
    }
}
EvaluationResult evaluate_expression(Lexer *lexer) {
    EvaluationResult result;
    result.error = NULL;

    lexer->current_token = lexer_get_next_token(lexer);
    result.result = expr(lexer);

    // Check if there are any remaining tokens, which indicates an error in the input expression.
    if (lexer->current_token.type != 0) {
        result.error = "Invalid expression syntax";
    }

    return result;
}

void Calc(TreeNode *root, char **token) {
    int i;
    char *expr = NULL;

    // Calculate the length of the expression
    int exprLength = 0;
    for (i = 1; token[i] != NULL; i++) {
        exprLength += strlen(token[i]);
    }

    // Allocate memory for the expression
    expr = (char *)malloc(exprLength + 1); // +1 for the null terminator

    // Concatenate tokens to form the expression
    expr[0] = '\0'; // Ensure the string starts empty
    for (i = 1; token[i] != NULL; i++) {
        strcat(expr, token[i]);
    }

    // Print or process the expression as needed

    size_t len = strlen(expr);
    if (len > 0 && expr[len - 1] == '\n') {
        expr[len - 1] = '\0';
    }

    Lexer lexer;
    lexer_init(&lexer, expr);
    EvaluationResult result = evaluate_expression(&lexer);
    if (result.error) {
        printf("Error: %s\n", result.error);
    } else {
        printf("Result: %lf\n", result.result);
    }

    // Free the allocated memory
    free(expr);
}

int ver(char *index) {
    if (strcmp(index, "var") == 0) {
        return 1;
    } else if (strcmp(index, "input") == 0) {
        return 2;
    } else if (strcmp(index, "=") == 0) {
        return 3;
    } else {
        printf("\nUndefined function:%s", index);
        return 0;
    }
}

void token(char *line, char ***tokens) {
    char *token = strtok(line, " \t\n;:");
    int tokenCount = 0;

    *tokens = (char **)malloc(sizeof(char *) * 100);

    // Save each token in the array
    while (token != NULL && tokenCount < 100) {
        // Save the token in the array
        (*tokens)[tokenCount] = strdup(token);

        // Get the next token
        token = strtok(NULL, " \t\n;,");
        tokenCount++;

        // Reallocate memory for the next token
        *tokens = (char **)realloc(*tokens, (tokenCount + 1) * sizeof(char *));
    }

    // Null-terminate the array of tokens
    (*tokens)[tokenCount] = NULL;
}

int main() {
    FILE *file = fopen("calculator.txt", "r");
    TreeNode *root = NULL;
    char **tokens;
    char *line;
    int i = 0;
    char c, d;
    char index[20] = "";
    while (1) {
        i = 0;
        line = (char *)malloc(sizeof(char));
        while ((c = getc(file)) != ';') {
            if (c == EOF) {
                break;
            } else {
                line[i] = c;
                i++;
                line = (char *)realloc(line, (i + 1) * sizeof(char));
            }
        }
        if (c == EOF) {
            break;
        }
        line[i] = '\0';
        token(line, &tokens);
        free(line);
        int p = ver(tokens[0]);

        if (p == 1) {
            Var(&root, tokens);
        } else if (p == 2) {
            Input(&root, tokens);
        } else if (p == 3) {
            Calc(root, tokens);
        }

        for (i = 0; tokens[i] != NULL; i++) {
            free(tokens[i]);
        }
        free(tokens);
        getc(file);
    }

    // Print the variables in the binary tree
    printf("\nVariables in the binary tree:");
    // Implement an in-order traversal of the tree to print variables
    // Modify this part based on your actual needs.
    // This is just a simple example.
    Deallocate(root);

    return 0;
}
