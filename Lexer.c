#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

void Calc(Node **root, char **token){
    int i;
    Node *curr;
    curr = *root;
    float final;
    curr = *root;
    for (i=0;token[i] != NULL;i++){
        printf("%s",token[i]);
    }
    /*switch (expression)
    {
    case /* constant-expression */
        /* code 
        break;
    
    default:
        break;
    }
    */
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
