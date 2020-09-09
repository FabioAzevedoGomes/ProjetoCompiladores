%{
#include <stdio.h>
int yylex(void);
int get_line_number();
void yyerror (char const *s);
%}

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_PR_END
%token TK_PR_DEFAULT
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_OC_SL
%token TK_OC_SR
%token TK_OC_FORWARD_PIPE
%token TK_OC_BASH_PIPE
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_LIT_FALSE
%token TK_LIT_TRUE
%token TK_LIT_CHAR
%token TK_LIT_STRING
%token TK_IDENTIFICADOR
%token TOKEN_ERRO

// Simbolo inicial
%start programa

%%

// Um programa pode ser:
programa:   %empty |                // Vazio
            funcao programa |       // Uma funcao, seguida de mais programa
            var_global programa;    // Uma definicao de variavel global, seguida de mais programa

// Um tipo valido pode ser:
// INT, FLOAT, CHAR, BOOL ou STRING
tipo: TK_PR_INT | TK_PR_FLOAT | TK_PR_CHAR | TK_PR_BOOL | TK_PR_STRING; 

// Um tipo estatico valido pode ser:
// A palavra STATIC seguida de um tipo, ou apenas o tipo
tipo_estatico: TK_PR_STATIC tipo | tipo;

// Um identificador pode ser:
identificador:  TK_IDENTIFICADOR |                  // Um identificador simples
                TK_IDENTIFICADOR '[' TK_LIT_INT ']';// Um vetor, com seu tamanho inteiro positivo entre colchetes a direita

// Uma lista de identificadores pode ser:
lista_identificadores:  identificador |                         // Um identificador
                        identificador ',' lista_identificadores;// Um identificador, seguido de uma lista de identificadores, separados por ,

// Uma declaracao de variavel global e: 
// Um tipo estatico, seguido de uma lista de identificadores, terminada por ;
var_global: tipo_estatico lista_identificadores ';';

// Um tipo const pode ser:
// A palavra CONST seguida de um tipo, ou apenas o tipo
tipo_const: TK_PR_CONST tipo | tipo;

// Um parametro de uma funcao e:
// Um tipo que pode ser const, seguido de seu nome ( Nao pode ser vetor )
parametro: tipo_const TK_IDENTIFICADOR;

// Uma lista de parametros de uma funcao pode ser:
lista_parametros:   parametro |                     // Um unico parametro
                    parametro ',' lista_parametros; // Um parametro seguido de uma lista de parametros, separados por virgulas

// A assinatura de uma funcao pode ser:
assinatura: '(' ')' |                   // Vazia, sendo caracterizada apenas pelos parenteses
            '(' lista_parametros ')';   // Um ou mais parametros entre colchetes

// Um comando simples pode ser:
comando_simples:    TK_PR_PRIVATE |     // TODO Placeholder
                    bloco_comandos;     // Um bloco de comandos

// Uma lista de comandos simples pode ser:
lista_comandos_simples: comando_simples ';' |                       // Um unico comando simples, terminado com ;
                        comando_simples ';' lista_comandos_simples; // Um comando simples, seguido de uma lista de comandos simples, separados por ;

// Um bloco de comandos pode ser:
bloco_comandos: '{' '}'| // Vazio, sendo caracterizado apenas pelas chaves
                '{' lista_comandos_simples '}'; // Uma lista de comandos simples

// Uma funcao e:
// Um tipo estatico de retorno, seguido pelo nome da funcao e sua assinatura, terminada por um bloco de comandos SEM ponto e virgula final
funcao: tipo_estatico TK_IDENTIFICADOR assinatura bloco_comandos;

%%

// Funcao para casos de erro
void yyerror(char const *s)
{
    printf("%s : On line %d\n",s, get_line_number());
}
