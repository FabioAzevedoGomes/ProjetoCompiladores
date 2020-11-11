#include <stdlib.h>

// Parser functions
extern int yyparse(void);
extern int yylex_destroy(void);

// AST pointer and functions
void *arvore = NULL;
void exporta(void *arvore);
void libera(void *arvore);
void export_code(void *arvore);
void generate_ASM(void *arvore);

int main(int argc, char **argv)
{
    // Parse input file
    int ret = yyparse();

    // If no problemas occurred
    if (ret == 0)
    {
        // Export the AST
        // exporta(arvore);

        // Export the ILOC code
        //export_code(arvore);

        // Convert from intermediate ILOC code to x86_64 assembly
        generate_ASM(arvore);

        // Free the AST memory
        libera(arvore);
    }

    // Free parser memory
    arvore = NULL;
    yylex_destroy();

    return ret;
}
