#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h> 
#include <stdbool.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/interpretor.h"
#include "include/types.h"
#include "include/read.h"

int main(int argc, char* argv[]) {
    lexer_T* lexer = init_lexer(get_file_contents("example.txt"));
    parser_T* parser = init_parser(lexer);
    while (parser->current_token->type != TOKEN_EOF) {
         TreeNode* root = parseStatement(parser);
         evaluate(root);
         freeTreeNode(root);
    }
    
    freeSymbolTable();

    return 0;
}
