#ifndef TOKEN_H
#define TOKEN_H

typedef enum
    {
        TOKEN_ID,
        TOKEN_EQUAL,
        TOKEN_NOT_EQUAL,
        TOKEN_ASSIGNMENT,
        TOKEN_EQUALITY,
        TOKEN_GREATER,
        TOKEN_LESS,
        TOKEN_GREATER_EQUAL,
        TOKEN_LESS_EQUAL,
        TOKEN_STRING,
        TOKEN_SEMI,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_INT,
        TOKEN_FLOAT,
        TOKEN_INT_DECLAR,
        TOKEN_FLOAT_DECLAR,
        TOKEN_SUM,
        TOKEN_DIF,
        TOKEN_MULT,
        TOKEN_DIV,
        TOKEN_IF,
        TOKEN_ENDIF,
        TOKEN_ELSE,
        TOKEN_WHILE,
        TOKEN_ENDWHILE,
        TOKEN_PRINT,
        TOKEN_EOF
    } TokenType;

typedef struct TOKEN_STRUCT
{
    TokenType type;

    char* value;
} token_T;

token_T* init_token(int type, char* value);
#endif