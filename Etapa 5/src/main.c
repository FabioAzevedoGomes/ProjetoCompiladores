#include <stdio.h>
#include "ILOC.h"

// Parser functions
extern int yyparse(void);
extern int yylex_destroy(void);

// AST pointer and functions
void *arvore = NULL;
void exporta(void *arvore);
void libera(void *arvore);

// Applicaiton entrypoint
int main(int argc, char **argv)
{
    // Parse input file
    int ret = yyparse();

    // Generate ILOC code based on the generated AST
    generate_code((node_t *)arvore);

    // Output generated code
    //output(((node_t *)arvore)->iloc_start);

    // Free the AST memory
    libera(arvore);

    // Free parser memory
    arvore = NULL;
    yylex_destroy();

    return ret;
}
