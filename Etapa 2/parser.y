%{
#include <stdio.h>

int yylex(void);
int get_line_number();
int yyerror (char const *s);
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

//================================================================================================
//                                  TIPOS E LITERAIS:
//================================================================================================

// Um tipo valido pode ser:
// INT, FLOAT, CHAR, BOOL ou STRING
tipo: TK_PR_INT | TK_PR_FLOAT | TK_PR_CHAR | TK_PR_BOOL | TK_PR_STRING; 

// Um literal valido pode ser:
// Um literal aritmetico, um literal logico, CHAR ou STRING
literal: literal_aritmetico | literal_logico | TK_LIT_CHAR | TK_LIT_STRING;

// Um literal aritmetico pode ser:
// Int ou float
literal_aritmetico: TK_LIT_INT | TK_LIT_FLOAT;

// Um literal logico pode ser:
// True ou false
literal_logico: TK_LIT_TRUE | TK_LIT_FALSE;

// Um tipo estatico valido pode ser:
// A palavra STATIC seguida de um tipo, ou apenas o tipo
tipo_estatico: TK_PR_STATIC tipo | tipo;

// Um tipo const pode ser:
// A palavra CONST seguida de um tipo, ou apenas o tipo
tipo_const: TK_PR_CONST tipo | tipo;

// Um tipo const estatico pode ser:
tipo_const_estatico:    tipo |                          // Apenas o tipo
                        TK_PR_STATIC tipo |             // A palavra STATIC seguida do tipo
                        TK_PR_CONST tipo |              // A palavra CONST seguida do tipo
                        TK_PR_CONST TK_PR_STATIC tipo;  // Ambas as palavras CONST e STATIC, seguidas do tipo

//================================================================================================
//                                  DECLARACAO DE VARIAVEIS GLOBAIS:
//================================================================================================

// Uma declaracao de variavel global e: 
// Um tipo estatico, seguido de uma lista de identificadores, terminada por ;
var_global: tipo_estatico lista_identificadores_globais ';' ;

// Uma lista de identificadores globais pode ser:
lista_identificadores_globais:  identificador_global |                                     // Um identificador
                                identificador_global ',' lista_identificadores_globais ;    // Um identificador, seguido de uma lista de identificadores, separados por ,

// Um identificador global pode ser:
identificador_global:   TK_IDENTIFICADOR |                  // Um identificador simples
                        TK_IDENTIFICADOR '[' TK_LIT_INT ']';// Um vetor, com seu tamanho inteiro positivo entre colchetes a direita

//================================================================================================
//                                  DECLARACOES DE FUNCOES:
//================================================================================================

// Uma funcao e:
// Um tipo estatico de retorno, seguido pelo nome da funcao e sua assinatura, terminada por um bloco de comandos SEM ponto e virgula final
funcao: tipo_estatico TK_IDENTIFICADOR assinatura bloco_comandos;

// A assinatura de uma funcao pode ser:
assinatura: '(' ')' |                   // Vazia, sendo caracterizada apenas pelos parenteses
            '(' lista_parametros ')';   // Um ou mais parametros entre parenteses

// Uma lista de parametros de uma funcao pode ser:
lista_parametros:   parametro |                     // Um unico parametro
                    parametro ',' lista_parametros; // Um parametro seguido de uma lista de parametros, separados por virgulas

// Um parametro de uma funcao e:
// Um tipo que pode ser const, seguido de seu nome ( Nao pode ser vetor )
parametro: tipo_const TK_IDENTIFICADOR;


//================================================================================================
//                            COMANDOS SIMPLES E BLOCOS DE COMANDOS:
//================================================================================================

// Um bloco de comandos pode ser:
bloco_comandos: '{' '}'| // Vazio, sendo caracterizado apenas pelas chaves
                '{' lista_comandos_simples '}'; // Uma lista de comandos simples

// Uma lista de comandos simples pode ser:
lista_comandos_simples: comando_simples |                       // Um unico comando simples
                        comando_simples lista_comandos_simples; // Um comando simples, seguido de uma lista de comandos simples,

// Um comando simples pode ser:
comando_simples:    controle_fluxo |                // Um comando de controle de fluxo (Sem ; no final)
                    TK_PR_RETURN expressao ';'|     // Um return, seguido de uma expressao
                    TK_PR_BREAK   ';'|              // Um break
                    TK_PR_CONTINUE ';'|             // Um continue
                    comando_shift ';'|              // Um comando de shift
                    chamada_funcao ';'|             // Uma chamada de funcao
                    comando_es ';'|                 // Um comando de e/s
                    atribuicao ';'|                 // Uma atribuicao
                    var_local ';'|                  // Uma variavel local
                    bloco_comandos ';';             // Um bloco de comandos

//================================================================================================
//                                          CHAMADA DE FUNCAO:
//================================================================================================

// Uma chamada de funcao e:
chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')' | // O nome da funcao, seguida de uma lista de argumentos entre parenteses
                TK_IDENTIFICADOR '(' ')'; // O nome da funcao, seguido de parenteses (caso a lista de arugmentos seja vazia

// Uma lista de argumentos pode ser:
lista_argumentos:   expressao |                     // Um unico argumento
                    expressao ',' lista_argumentos; // Um argumento seguido de uma lista de argumentos, separados por ,

//================================================================================================
//                                              COMANDOS SIMPLES:
//================================================================================================

// Uma variavel local pode ser:
var_local:  tipo_const_estatico lista_identificadores_locais |  // Um tipo CONST STATIC, um identificador simples (nao vetor)
            tipo_const_estatico TK_IDENTIFICADOR TK_OC_LE literal | // Um tipo CONST STATIC, um identificador simples, e '<=' seguido de um literal
            tipo_const_estatico TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR; // Um tipo CONST STATIC, um identificador simples, e '<=' seguido de um identificador

lista_identificadores_locais:  TK_IDENTIFICADOR |                                     // Um identificador
                               TK_IDENTIFICADOR ',' lista_identificadores_locais ;    // Um identificador, seguido de uma lista de identificadores, separados por ,

// Uma atribuicao pode ser:
atribuicao: TK_IDENTIFICADOR '=' expressao |                    // Uma atribuicao a um identificador simples
            TK_IDENTIFICADOR '[' expressao ']' '=' expressao;   // Uma atribuicao a um vetor indexado por uma expressao

// Um comando de entrada ou saida pode ser:
comando_es: TK_PR_INPUT TK_IDENTIFICADOR |  // A palavra input, seguida de um identificador
            TK_PR_OUTPUT TK_IDENTIFICADOR | // A palavra output, seguida de um identificador
            TK_PR_OUTPUT literal;           // A palavra output, seguida de um literal

// Um operador de shift pode ser:
// Um operador de shift left (<<) ou shift right (>>)
operador_shift: TK_OC_SL | TK_OC_SR;

// Um comando de shift pode ser:
comando_shift:  TK_IDENTIFICADOR operador_shift TK_LIT_INT |                 // Um identificador seguido de um operador de shift e um literal inteiro positivo
                TK_IDENTIFICADOR '[' expressao ']' operador_shift TK_LIT_INT;// Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo

//================================================================================================
//                                         CONTROLE DE FLUXO:
//================================================================================================

// Um comando de controle de fluxo pode ser:
controle_fluxo: comando_if |
                comando_for |
                comando_while;

// Um comando if pode ser:
comando_if: TK_PR_IF '(' expressao ')' bloco_comandos | // A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos
            TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos;

// Um comando for pode ser:
// A palavra for, seguida de, entre parenteses, uma atribuicao, expressao e atribuicao, separados por :, e por fim um bloco de comandos
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos;

// Um comando while pode ser:
// A palavra while, seguida de uma expressao entre parenteses, a palavra do e um bloco de comandos
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos;

//================================================================================================
//                                      EXPRESSOES:
//================================================================================================

// Uma expressao pode ser:
// Uma expressao operada junto a um termo, atraves de operadores de baixa precedencia, ou apenas um termo
expressao: expressao operador_binario_baixa_prec termo | termo;

// Um termo pode ser:
// Um termo operado junto a um fator, atraves de operadores de alta precedencia, ou apenas um fator
termo: termo operador_binario_alta_prec fator | fator;

// Um fator pode ser:
// Uma expressao entre parenteses, ou apenas um operando
fator: '(' expressao ')' | operando;
           
// Um operando pode ser:
operando:  TK_IDENTIFICADOR |                   // Um identificador
           TK_IDENTIFICADOR '[' expressao ']' | // Um vetor indexado por uma expressao
           TK_LIT_INT |                         // Um literal inteiro
           TK_LIT_FLOAT |                       // Um literal float
           chamada_funcao |                     // Uma chamada de funcao
           TK_LIT_TRUE |                        // Um literal TRUE
           TK_LIT_FALSE |                       // Um literal FALSE
           operador_unario fator;               // Um operador unario aplicado a um fator


// Um comparador relacional pode ser:
// Maior ou igual, menor ou igual, igual, ou diferente
comparador_relacional: TK_OC_GE | TK_OC_LE | TK_OC_EQ | TK_OC_NE;

// Um operador logico pode ser:
// And ou Or
operador_logico: TK_OC_AND | TK_OC_OR;

// Um operador binario de baixa precedencia pode ser:
// Soma, subtracao, bitwise or, bitwise and, comparadores relacionais e operadores logicos. Tambem aceita a 'parte' interna do operador ternario
operador_binario_baixa_prec: '+' | '-' | '|' | '&' | comparador_relacional | operador_logico | '?' expressao ':';

// Um operador binario de alta precedencia pode ser:
// Multiplicacao, divisao, modulo e exponenciacao
operador_binario_alta_prec: '*' | '/' | '%' | '^';

// Um operador unario pode ser:
// Positivo, negativo, negacao, acesso a endereco, resolucao, avaliacao e acesso a hash
operador_unario: '+' | '-' | '!' | '&' | '*' | '?' | '#';

%%

// Funcao para casos de erro
int yyerror(char const *s)
{
    printf("%s : On line %d\n",s, get_line_number());
    return 1;
}
