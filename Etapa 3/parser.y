%{
    #include <stdio.h>

    /* Funcao de obtencao dos tokens */
    int yylex(void);
    
    /* Funcao que retorna a linha atual */
    int get_line_number();
    
    /* Funcao de tratamento de erro */
    int yyerror (char const *s);
%}

/* Define o relatorio de erro como verboso */
%define parse.error verbose

/* Tokens */
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
programa:   /* empty */                 // Vazio
          | declaracao_funcao programa  // Uma declaracao de funcao, seguida de mais programa
          | var_global programa         // Uma definicao de variavel global, seguida de mais programa
;


// TIPOS E LITERAIS


// Um tipo valido pode ser:
tipo:   TK_PR_INT       // A palavra int
      | TK_PR_FLOAT     // A palavra float
      | TK_PR_CHAR      // A palavra char
      | TK_PR_BOOL      // A palavra bool
      | TK_PR_STRING    // A palavra string
; 

// Um literal valido pode ser:
literal:   TK_LIT_INT       // Um literal aritmetico
         | TK_LIT_FLOAT     // Um literal logico
         | TK_LIT_TRUE      // Um literal boolean true
         | TK_LIT_FALSE     // Um literal boolean false
         | TK_LIT_CHAR      // Um literal char
         | TK_LIT_STRING    // Um literal string
;

// Um tipo estatico valido pode ser:
tipo_estatico:   TK_PR_STATIC tipo // A palavra static, seguida de um tipo
               | tipo              // Apenas um tipo
;

// Um tipo const pode ser:
tipo_const:   TK_PR_CONST tipo  // A palavra const, seguida de um tipo
            | tipo              // Apenas um tipo
;

// Um tipo const estatico pode ser:
tipo_const_estatico:   tipo                          // Apenas um tipo
                     | TK_PR_STATIC tipo             // A palavra static seguida de um tipo
                     | TK_PR_CONST tipo              // A palavra const seguida de um tipo
                     | TK_PR_CONST TK_PR_STATIC tipo // A palavra const e static, seguidas de um tipo
;


// DECLARACAO DE VARIAVEIS GLOBAIS


/**
 * Uma declaracao de variavel global e:
 *
 * Um tipo possivelmente estatico...
 * ... seguido de uma lista de identificadores...
 * ... e terminado por ponto e virgula (;)
 */ 
var_global: tipo_estatico lista_identificadores_globais ';' ;

/**
 * Uma lista de identificadores globais pode ser:
 *
 * Apenas um identificador
 *  OU
 * Um identificador, seguido de uma lista de identificadores, separados por virgula
 */
lista_identificadores_globais:   identificador_global
                               | identificador_global ',' lista_identificadores_globais
; 

// Um identificador global pode ser:
identificador_global:   TK_IDENTIFICADOR |                  // Um identificador simples
                        TK_IDENTIFICADOR '[' TK_LIT_INT ']' // Um vetor, com seu tamanho inteiro positivo entre colchetes a direita
;


// DECLARACAO DE FUNCOES


/**
 * Uma declaracao de funcao e:
 * Um tipo estatico de retorno...
 * ...seguido pelo nome da funcao e sua assinatura ...
 * ... terminada por um bloco de comandos
 */
declaracao_funcao: tipo_estatico TK_IDENTIFICADOR assinatura bloco_comandos;

// A assinatura de uma funcao pode ser:
assinatura:   '(' ')'                    // Vazia, sendo caracterizada apenas pelos parenteses
            | '(' lista_parametros ')'   // Um ou mais parametros entre parenteses
;

// Uma lista de parametros de uma funcao pode ser:
lista_parametros:   parametro                      // Um unico parametro
                  | parametro ',' lista_parametros // Um parametro seguido de uma lista de parametros, separados por virgula
;

/**
 * Um parametro de uma funcao e:
 *
 * Um tipo possivelmente const ... 
 * ... seguido de seu nome
 *
 * OBS.: Nao pode ser vetor
 */
parametro: tipo_const TK_IDENTIFICADOR;


// COMANDOS SIMPLES E BLOCOS DE COMANDOS


// Um bloco de comandos pode ser:
bloco_comandos:   '{' '}'                        // Vazio, sendo caracterizado apenas pelas chaves
                | '{' lista_comandos_simples '}' // Uma lista de comandos simples
;

// Uma lista de comandos simples pode ser:
lista_comandos_simples:   comando_simples                         // Um unico comando simples
                        | comando_simples lista_comandos_simples; // Um comando simples, seguido de uma lista de comandos simples

// Um comando simples pode ser:
comando_simples:   controle_fluxo ';'         // Um comando de controle de fluxo, terminado por ponto e virgula (;)
                 | TK_PR_RETURN expressao ';' // Um return, seguido de uma expressao e terminado por ponto e virgula (;)
                 | TK_PR_BREAK   ';'          // Um break, terminado por ponto e virgula (;)
                 | TK_PR_CONTINUE ';'         // Um continue, terminado por ponto e virgula (;)
                 | comando_shift ';'          // Um comando de shift, terminado por ponto e virgula (;)
                 | chamada_funcao ';'         // Uma chamada de funcao, terminada por ponto e virgula (;)
                 | comando_es ';'             // Um comando de e/s, terminado por ponto e virgula (;)
                 | atribuicao ';'             // Uma atribuicao, terminada por ponto e virgula (;)
                 | var_local ';'              // Uma declaracao de variavel local, terminada por ponto e virgula (;)
                 | bloco_comandos ';'         // Um bloco de comandos, terminado por ponto e virgula (;)
;


// CHAMADA DE FUNCAO


/** 
 * Uma chamada de funcao e:
 *
 * O nome da funcao, seguido de parenteses sem argumentos
 *  OU
 * O nome da funcao, seguido de parenteses e uma lista de argumentos
 */
chamada_funcao:   TK_IDENTIFICADOR '(' ')'
                | TK_IDENTIFICADOR '(' lista_argumentos ')'
;

// Uma lista de argumentos pode ser:
lista_argumentos:   argumento                      // Um unico argumento
                  | argumento ',' lista_argumentos // Um argumento seguido de uma lista de argumentos, separados por virgula
;

// Um argumento pode ser:
argumento: expressao; // Uma expressao


// COMANDOS SIMPLES


/**
 * Uma declaracao de variavel local e:
 *
 * Um tipo possivelmente const static, ...
 * ... seguido de uma lista de identificadores
 */
var_local:   tipo_const_estatico lista_identificadores_locais;

/**
 * Uma lista de identificadores locais pode ser:
 *
 * Um identificador local
 *  OU
 * Um identificador local, seguido de uma lista de identificadores locais, separados por virgula
 */
lista_identificadores_locais:   identificador_local
                              | identificador_local ',' lista_identificadores_locais
;

/**
 * Um identificador local pode ser:
 *
 * Um identificador simples
 *  OU
 * Um identificador simples inicializado com um literal
 *  OU
 * Um identificador simples inicializado com outro identificador simples
 */
identificador_local:   TK_IDENTIFICADOR
                     | TK_IDENTIFICADOR TK_OC_LE literal
                     | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR
;

// Uma atribuicao pode ser:
atribuicao:   TK_IDENTIFICADOR '=' expressao                   // Uma atribuicao a um identificador simples
            | TK_IDENTIFICADOR '[' expressao ']' '=' expressao // Uma atribuicao a um vetor indexado por uma expressao
;

// Um comando de entrada ou saida pode ser:
comando_es:   TK_PR_INPUT TK_IDENTIFICADOR  // A palavra input, seguida de um identificador
            | TK_PR_OUTPUT TK_IDENTIFICADOR // A palavra output, seguida de um identificador
            | TK_PR_OUTPUT literal          // A palavra output, seguida de um literal
;

// Um operador de shift pode ser:
operador_shift:   TK_OC_SL // O operador de shift left (<<) 
                | TK_OC_SR // O operador de shift right (>>)
;

/**
* Um comando de shift pode ser:
*
* Um identificador, seguido de um operador de shift e um literal inteiro positivo
*  OU
* Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo
*/
comando_shift:   TK_IDENTIFICADOR operador_shift TK_LIT_INT
               | TK_IDENTIFICADOR '[' expressao ']' operador_shift TK_LIT_INT
;


// COMANDOS DE CONTROLE DE FLUXO


// Um comando de controle de fluxo pode ser:
controle_fluxo:   comando_if    // Um comando if
                | comando_for   // Um comando for
                | comando_while // Um comando while
;

/**
 * Um comando if pode ser:
 *
 * A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos
 *  OU
 * A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos...
 * ... terminado com a palavra else, seguida de um bloco de comandos
 */
comando_if:   TK_PR_IF '(' expressao ')' bloco_comandos
            | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos
;

/**
 * Um comando for e:
 *
 * A palavra for, seguida de, entre parenteses, ...
 * ... uma atribuicao, expressao e atribuicao, nessa ordem, separados por dois pontos (:)
 * ... terminado por um bloco de comandos
 */
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos;

/**
 * Um comando while pode ser:
 *
 * A palavra while, seguida de uma expressao entre parenteses...
 * ... terminado pela palavra do, seguida de um bloco de comandos
 */
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos;


// EXPRESSOES


// Uma expressao pode ser:
expressao:   expressao operador_binario_baixa_prec termo // Uma expressao associada a um termo, atraves de um operador de baixa precedencia
           | termo                                       // Apenas um termo
;

// Um termo pode ser:
termo:   termo operador_binario_alta_prec fator // Um termo associado a um fator, atraves de um operador de alta precedencia
       | fator                                  // Apenas um fator
;

// Um fator pode ser:
fator:   '(' expressao ')' // Uma expressao entre parenteses
       | operando          // Um operando valido da linguagem
;

// Um operando pode ser:
operando:   TK_IDENTIFICADOR                   // Um identificador
          | TK_IDENTIFICADOR '[' expressao ']' // Um vetor indexado por uma expressao
          | TK_LIT_INT                         // Um literal inteiro
          | TK_LIT_FLOAT                       // Um literal float
          | chamada_funcao                     // Uma chamada de funcao
          | TK_LIT_TRUE                        // Um literal TRUE
          | TK_LIT_FALSE                       // Um literal FALSE
          | operador_unario fator              // Um operador unario aplicado a um fator
;

// Um comparador relacional pode ser:
comparador_relacional:   TK_OC_GE // O comparador de maior ou igual (>=)
                       | TK_OC_LE // O comparador de menor ou igual (<=)
                       | TK_OC_EQ // O comparador de igualdade (==)
                       | TK_OC_NE // O comparador de diferenca (!=)
                       | '<'      // O comparador de menor
                       | '>'      // O comparador de maior
;

// Um operador logico pode ser:
operador_logico:   TK_OC_AND // O operdor AND (&&)
                 | TK_OC_OR  // O operador OR (||)
;

// Um operador binario de baixa precedencia pode ser:
operador_binario_baixa_prec:   '+'                   // O operador de soma
                             | '-'                   // O operador de subtracao
                             | '|'                   // O operador bitwise or
                             | '&'                   // O operador bitwise and
                             | comparador_relacional // Um comparador relacional 
                             | operador_logico       // Um operador logico
                             | '?' expressao ':'     // A 'parte' interna de uma expressao condicional
;

// Um operador binario de alta precedencia pode ser:
operador_binario_alta_prec:   '*' // O operador de multiplicacao
                            | '/' // O operador de divisao
                            | '%' // O operador de modulo
                            | '^' // O operador de exponenciacao
;

// Um operador unario pode ser:
operador_unario:   '+' // O operador de positividade explicita
                 | '-' // O operador de inversao de sinal
                 | '!' // O operador de negacao
                 | '&' // O operador de acesso a endereco
                 | '*' // O operador de resolucao de ponteiros
                 | '?' // O operador de avaliacao de expressao
                 | '#' // O operador de acesso a tabela hash
;

%%
    /* FUNCOES OPCIONAIS */

/* Funcao para casos de erro */
int yyerror(char const *s)
{
    int error_line = get_line_number(); // Linha onde ocorreu o erro
    
    /* Sinaliza na saida padrao que ocorreu um erro sintatico */
    fprintf(stderr,"%s\nOn line %d\n", s, error_line);
    
    /* Retorna a linha onde ocorreu o erro*/
    return error_line;
}
