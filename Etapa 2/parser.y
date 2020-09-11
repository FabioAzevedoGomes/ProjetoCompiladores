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

// Um literal valido pode ser:
// INT, FLOAT, FALSE, TRUE, CHAR, STRING
literal: TK_LIT_INT | TK_LIT_FLOAT | TK_LIT_FALSE | TK_LIT_TRUE | TK_LIT_CHAR | TK_LIT_STRING;

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

// Um tipo const estatico pode ser:
tipo_const_estatico:    tipo |                          // Apenas o tipo
                        TK_PR_STATIC tipo |             // A palavra STATIC seguida do tipo
                        TK_PR_CONST tipo |              // A palavra CONST seguida do tipo
                        TK_PR_CONST TK_PR_STATIC tipo;  // Ambas as palavras CONST e STATIC, seguidas do tipo

// Um parametro de uma funcao e:
// Um tipo que pode ser const, seguido de seu nome ( Nao pode ser vetor )
parametro: tipo_const TK_IDENTIFICADOR;

// Uma lista de parametros de uma funcao pode ser:
lista_parametros:   parametro |                     // Um unico parametro
                    parametro ',' lista_parametros; // Um parametro seguido de uma lista de parametros, separados por virgulas

// A assinatura de uma funcao pode ser:
assinatura: '(' ')' |                   // Vazia, sendo caracterizada apenas pelos parenteses
            '(' lista_parametros ')';   // Um ou mais parametros entre parenteses

// Uma variavel local pode ser:
var_local:  tipo_const_estatico TK_IDENTIFICADOR |  // Um tipo CONST STATIC, um identificador simples (nao vetor)
            tipo_const_estatico TK_IDENTIFICADOR TK_OC_LE literal | // Um tipo CONST STATIC, um identificador simples, e '<=' seguido de um literal
            tipo_const_estatico TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR; // Um tipo CONST STATIC, um identificador simples, e '<=' seguido de um identificador

// Uma expressao pode ser:
expressao:  expressao_logica |      // Uma expressao logica
            expressao_aritmetica;   // Uma expressao aritmetica

// Uma expressao logica pode ser:
expressao_logica: TK_PR_PROTECTED;  // TODO Placeholder

// Uma expressao aritmetica pode ser:
expressao_aritmetica: TOKEN_ERRO;      // TODO Placeholder

//Uma atribuicao pode ser:
atribuicao: TK_IDENTIFICADOR '=' expressao |                    // Uma atribuicao a um identificador simples
            TK_IDENTIFICADOR '[' expressao ']' '=' expressao;   // Uma atribuicao a um vetor indexado por uma expressao

// Um comando de entrada ou saida pode ser:
comando_es: TK_PR_INPUT TK_IDENTIFICADOR |  // A palavra input, seguida de um identificador
            TK_PR_OUTPUT TK_IDENTIFICADOR | // A palavra output, seguida de um identificador
            TK_PR_OUTPUT literal;           // A palavra output, seguida de um literal

// Um argumento pode ser:
argumento:  TK_PR_PUBLIC | // TODO Placeholder
            expressao;      // Uma expressao

// Uma lista de argumentos pode ser:
lista_argumentos:   argumento |                     // Um unico argumento
                    argumento ',' lista_argumentos; // Um argumento seguido de uma lista de argumentos, separados por ,

// Uma chamada de funcao e:
chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')' | // O nome da funcao, seguida de uma lista de argumentos entre parenteses
                TK_IDENTIFICADOR '(' ')'; // O nome da funcao, seguido de parenteses (caso a lista de arugmentos seja vazia

// Um operador de shift pode ser:
// Um operador de shift left (<<) ou shift right (>>)
operador_shift: TK_OC_SL | TK_OC_SR;

// Um comando de shift pode ser:
comando_shift:  TK_IDENTIFICADOR operador_shift TK_LIT_INT |                 // Um identificador seguido de um operador de shift e um literal inteiro positivo
                TK_IDENTIFICADOR '[' expressao ']' operador_shift TK_LIT_INT;// Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo


// Um comando if pode ser:
comando_if: TK_PR_IF '(' expressao ')' bloco_comandos | // A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos
            TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos;    // A palavra if, com uma expressao entre parenteses e seguida de um bloco de comandos. Adicionalmente, um else seguido de um bloco de comandos no final

// Um comando for pode ser:
// A palavra for, seguida de, entre parenteses, uma atribuicao, expressao e atribuicao, separados por :, e por fim um bloco de comandos
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos;

// Um comando while pode ser:
// A palavra while, seguida de uma expressao entre parenteses, a palavra do e um bloco de comandos
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos;

// Um comando de controle de fluxo pode ser:
controle_fluxo: comando_if |
                comando_for |
                comando_while;

// Um comando simples pode ser:
comando_simples:    TK_PR_PRIVATE |                 // TODO Placeholder
                    controle_fluxo |                // Um comando de controle de fluxo (Sem ; no final)
                    TK_PR_RETURN expressao ';'|     // Um return, seguido de uma expressao
                    TK_PR_BREAK   ';'|              // Um break
                    TK_PR_CONTINUE ';'|             // Um continue
                    comando_shift ';'|              // Um comando de shift
                    chamada_funcao ';'|             // Uma chamada de funcao
                    comando_es ';'|                 // Um comando de e/s
                    atribuicao ';'|                 // Uma atribuicao
                    var_local ';'|                  // Uma variavel local
                    bloco_comandos ';';             // Um bloco de comandos

// Uma lista de comandos simples pode ser:
lista_comandos_simples: comando_simples |                       // Um unico comando simples
                        comando_simples lista_comandos_simples; // Um comando simples, seguido de uma lista de comandos simples,

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
