#include<stdio.h>
#include "include/lexer.h"


int main (int argc, char* argv[]){
    lexer_T* lexer = init_lexer(
        "var int a = a-777+333;\n"
        "while (a == 5){\n"
        "}\n"
        "while (a <= 5){\n"
        "}\n"
        "print(a);\n"
    );
    token_T* token = (void*)0;
    while ((token = lexer_get_next_token(lexer))->type != TOKEN_EOF){
            printf("token (%d,%s)\n",token->type, token->value);
    }
    return 0;
}