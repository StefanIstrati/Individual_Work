#include "include/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


lexer_T* init_lexer(char* contents)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->i = 0;
    lexer->c = contents[lexer->i];

    return lexer;
}

void lexer_advance(lexer_T* lexer)
{
    if (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        lexer->i += 1;
        lexer->c = lexer->contents[lexer->i];
    }
}

void lexer_retreat(lexer_T* lexer){
    lexer->i -=1;
    lexer->c = lexer->contents[lexer->i];
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while (lexer->c == ' ' || lexer->c == 10)
    {
        lexer_advance(lexer);
    }
}

token_T* lexer_get_next_token(lexer_T* lexer)
{
    char* sign = '\0';
    while (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        if (lexer->c == ' ' || lexer->c == 10)
            lexer_skip_whitespace(lexer);
        if (isdigit(lexer->c) || lexer->c == '.'){
            int point = 0;
            char* value = calloc(1, sizeof(char));
            value[0] = '\0';

            while(isdigit(lexer->c) || lexer->c == '.'){
                char* s = lexer_get_current_char_as_string(lexer);
                value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
                strcat(value, s);

                if (lexer->c == '.' && point != 0){
                    fprintf(stderr,"Too many points");
                }

                if (lexer->c == '.'){
                    point++;
                }
                lexer_advance(lexer);
            }
            if (point == 0){
                return init_token(TOKEN_INT,value);
            }
            return init_token(TOKEN_FLOAT,value);
        }

        if (isalnum(lexer->c))
            return lexer_collect_id(lexer);

        switch (lexer->c)
        {
            case '=': 
                sign = calloc(3, sizeof(char));
                sign[0] = lexer->c;
                sign[1] = '\0';
                sign[2] = '\0';
                lexer_advance(lexer);
                if (lexer->c == '='){
                    sign[1] = lexer->c;
                    return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALITY,sign));
                }else{
                    lexer_retreat(lexer);
                    return lexer_advance_with_token(lexer, init_token(TOKEN_EQUAL, lexer_get_current_char_as_string(lexer))); 
                }
                break;
            case '<':
                sign = calloc(3, sizeof(char));
                sign[0] = lexer->c;
                sign[1] = '\0';
                sign[2] = '\0';
                lexer_advance(lexer);
                if (lexer->c == '='){
                    sign[1] = lexer->c;
                    return lexer_advance_with_token(lexer, init_token(TOKEN_LESS_EQUAL,sign));
                }else{
                    lexer_retreat(lexer);
                    return lexer_advance_with_token(lexer, init_token(TOKEN_LESS, lexer_get_current_char_as_string(lexer)));                     
                }
                break;
            case '>': 
                sign = calloc(3, sizeof(char));
                sign[0] = lexer->c;
                sign[1] = '\0';
                sign[2] = '\0';
                lexer_advance(lexer);
                if (lexer->c == '='){
                    sign[1] = lexer->c; 
                    return lexer_advance_with_token(lexer, init_token(TOKEN_GREATER_EQUAL,sign));
                }else{
                    lexer_retreat(lexer);
                    return lexer_advance_with_token(lexer, init_token(TOKEN_GREATER, lexer_get_current_char_as_string(lexer)));                     
                }
                break;
            case '!':
                sign = calloc(3, sizeof(char));
                sign[0] = lexer->c;
                sign[1] = '\0';
                sign[2] = '\0';
                lexer_advance(lexer);
                if (lexer->c == '='){
                    sign[1] = lexer->c; 
                    return lexer_advance_with_token(lexer, init_token(TOKEN_NOT_EQUAL,sign));
                }else{
                    lexer_retreat(lexer);
                    fprintf(stderr,"Undefined token '!'");
                    exit(1);                   
                }
                break;
            case ';': return lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_char_as_string(lexer))); break;
            case '(': return lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN, lexer_get_current_char_as_string(lexer))); break;
            case ')': return lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN, lexer_get_current_char_as_string(lexer))); break;
            case '+': return lexer_advance_with_token(lexer, init_token(TOKEN_SUM, lexer_get_current_char_as_string(lexer))); break;
            case '-': return lexer_advance_with_token(lexer, init_token(TOKEN_DIF, lexer_get_current_char_as_string(lexer))); break;
            case '*': return lexer_advance_with_token(lexer, init_token(TOKEN_MULT, lexer_get_current_char_as_string(lexer))); break;
            case '/': return lexer_advance_with_token(lexer, init_token(TOKEN_DIV, lexer_get_current_char_as_string(lexer))); break;
        }
    }

    return init_token(TOKEN_EOF, "\0");
}



token_T* lexer_collect_id(lexer_T* lexer)
{
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (isalnum(lexer->c))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);

        lexer_advance(lexer);
    }
    if(strcmp(value,"int") == 0){
        return init_token(TOKEN_INT_DECLAR,value);
    }else if(strcmp(value,"float") == 0){
        return init_token(TOKEN_FLOAT_DECLAR,value);
    }else if(strcmp(value,"while") == 0){
        return init_token(TOKEN_WHILE,value);
    }else if(strcmp(value,"if") == 0){
        return init_token(TOKEN_IF,value);
    }else if(strcmp(value,"else") == 0){
        return init_token(TOKEN_ELSE,value);
    }else if(strcmp(value,"endwhile") == 0){
        return init_token(TOKEN_ENDWHILE,value);
    }else if(strcmp(value,"endif") == 0){
        return init_token(TOKEN_ENDIF,value);
    }else if (strcmp(value,"print") == 0){
        return init_token(TOKEN_PRINT,value);
    }

    return init_token(TOKEN_ID, value);
}

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);

    return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->c;
    str[1] = '\0';

    return str;
}