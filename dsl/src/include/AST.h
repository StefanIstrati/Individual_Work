#ifndef AST_H
#define AST_H

typedef enum NodeType {
    PROGRAM,
    NUMERIC_LITERAL,
    IDENTIFIER,
    BINARY_EXPR,
    CONDITIONAL_EXPR,
    WHILE_STATEMENT,
    IF_STATEMENT,
    PRINT_STATEMENT
} NodeType;

typedef struct Program {
    NodeType kind;
    struct Stmt** body;
} Program;

typedef struct Stmt {
    NodeType kind;
    union {
        struct Expr* expr;
        struct WhileStmt* whileStmt;
        struct IfStmt* ifStmt;
    };
} Stmt;

typedef struct Expr {
    union {
        struct BinaryExpr* binaryExpr;
        struct Identifier* identifier;
        struct NumericLiteral* numericLiteral;
        struct ConditionalExpr conditionalExpr;
    };
} Expr;

typedef struct BinaryExpr {
    NodeType kind;
    struct Expr* left;
    struct Expr* right;
    char* op;
} BinaryExpr;

typedef struct Identifier {
    NodeType kind;
    char* symbol;
} Identifier;

typedef struct NumericLiteral {
    NodeType kind;
    double value;
} NumericLiteral;

typedef struct ConditionalExpr {
    NodeType kind;
    struct Expr* left;
    struct Expr* right;
    char* op;
} ConditionalExpr;

typedef struct WhileStmt {
    NodeType kind;
    char* condition;
    struct Stmt** body;
} WhileStmt;

typedef struct IfStmt {
    NodeType kind;
    char* condition;
    struct Stmt** true_branch;
    struct Stmt** false_branch;
} IfStmt;

#endif
