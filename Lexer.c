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

typedef struct Node {
    union d_type var;
    char *var_name;
    DATA_TYPE type;
    struct Node *next;
} Node;

typedef struct {
    double result;
    char* error;
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
    const char* input;
    size_t position;
    char current_char;
    Token current_token;
} Lexer;

void lexer_init(Lexer* lexer, const char* input);
Token lexer_get_next_token(Lexer* lexer);

double factor(Lexer* lexer);
double term(Lexer* lexer);
double expr(Lexer* lexer);

void lexer_init(Lexer* lexer, const char* input) {
    lexer->input = input;
    lexer->position = 0;
    lexer->current_char = input[0];
    lexer->current_token.type = NUMBER;
    lexer->current_token.value.number = 0.0;
}

Token lexer_get_next_token(Lexer* lexer) {
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

double factor(Lexer* lexer) {
    Token token = lexer->current_token;
    if (token.type == NUMBER) {
        lexer->current_token = lexer_get_next_token(lexer);
        return token.value.number;
    } else if (token.type == LPAREN) {
        lexer->current_token = lexer_get_next_token(lexer);
        double result = expr(lexer);
        if (lexer->current_token.type != RPAREN) {
            printf("Error: Unmatched parenthesis\n");
            exit(1);
        }
        lexer->current_token = lexer_get_next_token(lexer); // Consume the closing parenthesis
        return result;
    } else {
        printf("Error: Invalid factor\n");
        exit(1);
    }
}

double term(Lexer* lexer) {
    double result = factor(lexer);

    while (lexer->current_token.type == MULTIPLY || lexer->current_token.type == DIVIDE) {
        Token token = lexer->current_token;
        if (token.type == MULTIPLY) {
            lexer->current_token = lexer_get_next_token(lexer);
            result *= factor(lexer);
        } else if (token.type == DIVIDE) {
            lexer->current_token = lexer_get_next_token(lexer);
            double divisor = factor(lexer);
            if (divisor == 0) {
                printf("Error: Division by zero\n");
                exit(1);
            }
            result /= divisor;
        }
    }
    return result;
}

double expr(Lexer* lexer) {
    double result = term(lexer);

    while (lexer->current_token.type == ADD || lexer->current_token.type == SUBTRACT) {
        Token token = lexer->current_token;
        if (token.type == ADD) {
            lexer->current_token = lexer_get_next_token(lexer);
            result += term(lexer);
        } else if (token.type == SUBTRACT) {
            lexer->current_token = lexer_get_next_token(lexer);
            result -= term(lexer);
        }
    }
    return result;
}

EvaluationResult evaluate_expression(Lexer* lexer) {
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

void append_int(Node **root, char *name) {
    Node *new_node = malloc(sizeof(Node));
    Node *curr;
    if (new_node == NULL) {
        exit(1);
    }
    new_node->next = NULL;
    new_node->var.INT = 0;
    new_node->type = INTEGER;
    new_node->var_name = strdup(name); // Allocate memory for the name
    if (*root == NULL) {
        *root = new_node;
    } else {
        curr = *root;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new_node;
    }
}
void append_float(Node **root, char *name) {
    Node *new_node = malloc(sizeof(Node));
    Node *curr;
    if (new_node == NULL) {
        exit(1);
    }
    new_node->next = NULL;
    new_node->var.FLOAT = 0;
    new_node->type = FLOAT;
    new_node->var_name = strdup(name); // Allocate memory for the name
    if (*root == NULL) {
        *root = new_node;
    } else {
        curr = *root;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new_node;
    }
}

void Var(Node **root, char **token) {
    int i;
    if(strcmp(token[1], "int") == 0){
        for(i=2;token[i] != NULL; i++){
            append_int(root,token[i]);
        }
    }else if(strcmp(token[1], "float") == 0){
        for(i=2;token[i] != NULL; i++){
            append_float(root,token[i]);
        }
    }else{
        printf("Undefined type of variables ");
    }
}
 
void Input(Node **root, char **token) {
    int i;
    Node *curr;
    curr = *root;
    while (curr != NULL) {
        if (strcmp(curr->var_name, token[1]) == 0) {
            if (curr->type == INTEGER) {
                curr->var.INT = atoi(token[3]);
                break;
            } else if (curr->type == FLOAT) {  
                curr->var.FLOAT = atof(token[3]);
                break;
            } else {
                printf("Undefined variable %s", token[1]);
                break;
            }
        }
        curr = curr->next;
    }
    if (curr == NULL) {
        printf("Undefined variable %s", token[1]);
    }
}

void Calc(Node **root, char **token) {
    int i;
    Node *curr = *root;
    char *expr = NULL;

    // Calculate the length of the expression
    int exprLength = 0;
    for (i = 1; token[i] != NULL; i++) {
        exprLength += strlen(token[i]);
    }

    // Allocate memory for the expression
    expr = (char *)malloc(exprLength + 1);  // +1 for the null terminator

    // Concatenate tokens to form the expression
    expr[0] = '\0';  // Ensure the string starts empty
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

    // Implement the actual logic for evaluating the expression here
    /* switch (expression) {
        case /* constant-expression */
            /* code 
            break;

        default:
            break;
    } */
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

void Deallocate(Node *root) {
    while (root != NULL) {
        Node *temp = root;
        root = root->next;
        free(temp->var_name);  // Free the name before freeing the node
        free(temp);
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
    FILE *file=fopen( "calculator.txt","r");
    Node *root = NULL;
    char **tokens;
    char *line;
    int i = 0;
    char c,d;
    char index[20] = "";
    Node *curr;
    while(1){
        i=0;
        line = (char *)malloc(sizeof(char));
        while ((c = getc(file)) != ';') {
            if(c == EOF){
            break;
            }else{
                line[i] = c;
                i++;
                line = (char *)realloc(line, (i + 1) * sizeof(char));
            }
        }
        if(c == EOF){
            break;
        }
        line[i] = '\0';
        token(line,&tokens);
        free(line);
        int p = ver(tokens[0]);

        if (p == 1) {
            Var(&root,tokens);
        }else if(p == 2){
            Input(&root,tokens);
        }else if (p == 3){
            Calc(&root,tokens);
        }

        for(i=0;tokens[i] != NULL;i++){
            free(tokens[i]);
        }free(tokens);
        getc(file);
    }
    end:
    curr=root;
                for (curr = root; curr != NULL; curr = curr->next) {
                    if (curr->type == INTEGER){
                        printf("\n%s:%d INTEGER", curr->var_name, curr->var.INT);
                    }else{
                        printf("\n%s:%f FLOAT", curr->var_name, curr->var.FLOAT);
                    }
                }
                // Add similar handling for FLOAT case if needed
    Deallocate(root);
    return 0;
}
