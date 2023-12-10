#include "include/lexer.h"
#include "include/parser.h"
#include "include/interpretor.h"
#include "include/read.h"

int main(int argc, char* argv[]) {
    lexer_T* lexer = init_lexer(get_file_contents("example.txt"));
    parser_T* parser = init_parser(lexer);
    while (parser->current_token->type != TOKEN_EOF) {
         TreeNode* root = parse_statement(parser);
         evaluate_node(root);
         free_tree_node(root);
    }
    
    free_variable();

    return 0;
}
