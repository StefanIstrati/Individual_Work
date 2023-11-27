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



typedef enum {
    VAR_DECLARATION,
    ASSIGNMENT,
    CALCULATION,
    IF_STATEMENT,
    WHILE_LOOP
} StatementType;

typedef struct {
    StatementType type;
    char **tokens;
} Statement;

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
    TreeNode *tree; 
} Lexer;

void lexer_init(Lexer *lexer, const char *input,TreeNode *tree);
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

void lexer_init(Lexer *lexer, const char *input, TreeNode *tree) {
    lexer->input = input;
    lexer->position = 0;
    lexer->current_char = input[0];
    lexer->current_token.type = NUMBER;
    lexer->current_token.value.number = 0.0;
    lexer->tree = tree;  
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
        char* str_num;
        int i=0;
        str_num = (char *)malloc(sizeof(char));
        while (isdigit(lexer->current_char) || lexer->current_char == '.') {
            str_num[i]=lexer->current_char;
            i++;
            str_num = (char *)realloc(str_num, (i + 1) * sizeof(char));
            lexer->position++;
            lexer->current_char = lexer->input[lexer->position];
        }
        number=atof(str_num);
        free(str_num);
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
        char* str_var = NULL;
        int i = 0;
        while (lexer->current_char != '.' && lexer->current_char != '+' && lexer->current_char != '-' && lexer->current_char != '*' && lexer->current_char != '/' && lexer->current_char != '(' && lexer->current_char != ')' && lexer->current_char != '\0') {
            str_var = (char *)realloc(str_var, (i + 1) * sizeof(char));
            str_var[i] = lexer->current_char;
            i++;
            lexer->position++;
            lexer->current_char = lexer->input[lexer->position];
        }
        str_var = (char *)realloc(str_var, (i + 1) * sizeof(char));
        str_var[i] = '\0';
        TreeNode* root = find_variable(lexer->tree,str_var);
        double variable;
        if (root->type == INTEGER){
            variable = root->var.INT + 0.0;
        }else if (root->type == FLOAT){
            variable = root->var.FLOAT;
        }
        free(str_var);
        return (Token){.type = NUMBER, .value.number = variable};
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
    lexer_init(&lexer, expr, root);
    EvaluationResult result = evaluate_expression(&lexer);
    if (result.error) {
        printf("Error: %s\n", result.error);
    } else {
        printf("Result: %g\n", result.result);
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
    while (token != NULL && tokenCount < 100) {
        (*tokens)[tokenCount] = strdup(token);
        token = strtok(NULL, " \t\n;,");
        tokenCount++;
        *tokens = (char **)realloc(*tokens, (tokenCount + 1) * sizeof(char *));
    }
    (*tokens)[tokenCount] = NULL;
}

Statement parse_statement(char *line) {
    Statement statement;
    token(line, &statement.tokens);

    // Determine the statement type
    if (strcmp(statement.tokens[0], "var") == 0) {
        statement.type = VAR_DECLARATION;
    } else if(strcmp(statement.tokens[0],"input") == 0){
        statement.type = ASSIGNMENT;
    }else if (strcmp(statement.tokens[0], "=") == 0) {
        statement.type = CALCULATION;
    } else if (strcmp(statement.tokens[0], "if") == 0) {
        statement.type = IF_STATEMENT;
    } else if (strcmp(statement.tokens[0], "while") == 0) {
        statement.type = WHILE_LOOP;
    } else {
        printf("Undefined");
    }

    return statement;
}

void execute_statement(TreeNode **root, Statement statement) {
    switch (statement.type) {
        case VAR_DECLARATION:
            Var(root, statement.tokens);
            break;
        case ASSIGNMENT:
            Input(root, statement.tokens);
            break;
        case CALCULATION:
            Calc(*root, statement.tokens);
            break;
        case IF_STATEMENT:
            // If();
            break;
        case WHILE_LOOP:
            // While();
            break;
        default:
            printf("Unsupported statement type\n");
            break;
    }
}

void While(){

}

void If(){

}

int main() {
    FILE *file = fopen("calculator.txt", "r");
    TreeNode *root = NULL;
    char *line;
    int a=0;
    while (1) {
        line = (char *)malloc(sizeof(char));
        int i = 0;
        char c;

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
        Statement statement = parse_statement(line);
        if (statement.type == WHILE_LOOP){
            a=1;
        }

        if (a == 1){
            execute_statement(&root, statement);
            goto label;
        }
        execute_statement(&root, statement);
        label:
        for (i = 0; statement.tokens[i] != NULL; i++) {
            free(statement.tokens[i]);
        }

        free(statement.tokens);
        free(line);
        getc(file);
    }
    // Print the variables in the binary tree
    printf("\nVariables in the binary tree:");

    Deallocate(root);
    fclose(file);   
    return 0;
}
