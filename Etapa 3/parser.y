%{
    #include <stdio.h>
    #include "ast.h" // Funcoes da arvore e tipos de dados

    /* Funcao de obtencao dos tokens */
    int yylex(void);
    
    /* Funcao que retorna a linha atual */
    int get_line_number();
    
    /* Funcao de tratamento de erro */
    int yyerror (char const *s);

    /* Referencia para a AST sendo montada */
    extern void *arvore;
%}

/* Union que representa o valor lexico do token*/
%union{
    node_t* nodo;                 // Nodo do token
    valor_lexico_t* valor_lexico; // Valor lexico do token 
}

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
%token<valor_lexico>TK_OC_LE
%token<valor_lexico>TK_OC_GE
%token<valor_lexico>TK_OC_EQ
%token<valor_lexico>TK_OC_NE
%token<valor_lexico>TK_OC_AND
%token<valor_lexico>TK_OC_OR
%token<valor_lexico>TK_OC_SL
%token<valor_lexico>TK_OC_SR
%token TK_OC_FORWARD_PIPE
%token TK_OC_BASH_PIPE
%token<valor_lexico>TK_LIT_INT
%token<valor_lexico>TK_LIT_FLOAT
%token<valor_lexico>TK_LIT_FALSE
%token<valor_lexico>TK_LIT_TRUE
%token<valor_lexico>TK_LIT_CHAR
%token<valor_lexico>TK_LIT_STRING
%token<valor_lexico>TK_IDENTIFICADOR
%token TOKEN_ERRO

// Simbolo inicial
%start programa

// Tipos dos nao terminais (Acredito que sejam todos do tipo nodo_t?)
%type <nodo> bloco_comandos 
lista_comandos_simples 
comando_es 
comando_simples 
var_local 
argumento 
lista_identificadores_locais 
identificador_local 
atribuicao 
operador_shift 
comando_shift 
vetor_indexado 
comando_if 
comando_for 
comando_while 
controle_fluxo 
termo 
expressao 
operador_logico 
comparador_relacional 
operador_binario_alta_prec 
operador_binario_baixa_prec 
chamada_funcao 
operando 
fator 
var_global 
declaracao_funcao 
programa 
operador_unario 
literal
lista_argumentos
%%

// Um programa pode ser:
programa:   /* empty */                 {$$ = NULL;} // Vazio
          | declaracao_funcao programa  {arvore = $1; insere_no_comando(&$1,$2);} // Uma declaracao de funcao, seguida de mais programa
          | var_global programa         {$$ = $2;/* Ignora var global */} // Uma definicao de variavel global, seguida de mais programa
;


// TIPOS E LITERAIS


// Um tipo valido pode ser:
tipo:   TK_PR_INT       {} // A palavra int
      | TK_PR_FLOAT     {} // A palavra float
      | TK_PR_CHAR      {} // A palavra char
      | TK_PR_BOOL      {} // A palavra bool
      | TK_PR_STRING    {} // A palavra string
; 

// Um literal valido pode ser:
literal:   TK_LIT_INT       {$$ = cria_nodo_lexico($1, LIT_INT);} // Um literal aritmetico
         | TK_LIT_FLOAT     {$$ = cria_nodo_lexico($1, LIT_FLOAT);} // Um literal logico
         | TK_LIT_TRUE      {$$ = cria_nodo_lexico($1, LIT_BOOL);} // Um literal boolean true
         | TK_LIT_FALSE     {$$ = cria_nodo_lexico($1, LIT_BOOL);} // Um literal boolean false
         | TK_LIT_CHAR      {$$ = cria_nodo_lexico($1, LIT_CHAR);} // Um literal char
         | TK_LIT_STRING    {$$ = cria_nodo_lexico($1, LIT_STRING);} // Um literal string
;

// Um tipo estatico valido pode ser:
tipo_estatico:   TK_PR_STATIC tipo {} // A palavra static, seguida de um tipo
               | tipo              {} // Apenas um tipo
;

// Um tipo const pode ser:
tipo_const:   TK_PR_CONST tipo  {} // A palavra const, seguida de um tipo
            | tipo              {} // Apenas um tipo
;

// Um tipo const estatico pode ser:
tipo_const_estatico:   tipo                          {} // Apenas um tipo
                     | TK_PR_STATIC tipo             {} // A palavra static seguida de um tipo
                     | TK_PR_CONST tipo              {} // A palavra const seguida de um tipo
                     | TK_PR_STATIC TK_PR_CONST tipo {} // A palavra const e static, seguidas de um tipo
;


// DECLARACAO DE VARIAVEIS GLOBAIS


/**
 * Uma declaracao de variavel global e:
 *
 * Um tipo possivelmente estatico...
 * ... seguido de uma lista de identificadores...
 * ... e terminado por ponto e virgula (;)
 */ 
var_global: tipo_estatico lista_identificadores_globais ';' {/* Ignora variavel nao inicializada*/} ;

/**
 * Uma lista de identificadores globais pode ser:
 *
 * Apenas um identificador
 *  OU
 * Um identificador, seguido de uma lista de identificadores, separados por virgula
 */
lista_identificadores_globais:   identificador_global                                   {/* Ignora variavel nao inicializada*/} 
                               | identificador_global ',' lista_identificadores_globais {/* Ignora variavel nao inicializada*/} 
; 

// Um identificador global pode ser:
identificador_global:   TK_IDENTIFICADOR                    {/* Ignora variavel nao inicializada*/}  // Um identificador simples
                      | TK_IDENTIFICADOR '[' TK_LIT_INT ']' {/* Ignora variavel nao inicializada*/} // Um vetor, com seu tamanho inteiro positivo entre colchetes a direita
;


// DECLARACAO DE FUNCOES


/**
 * Uma declaracao de funcao e:
 * Um tipo estatico de retorno...
 * ...seguido pelo nome da funcao e sua assinatura ...
 * ... terminada por um bloco de comandos
 */
declaracao_funcao: tipo_estatico TK_IDENTIFICADOR assinatura bloco_comandos {$$ = cria_nodo_lexico($2, FUNC_LIST); $$ = preenche_nodo(&$$,$4, NULL, NULL, NULL);} ;

// A assinatura de uma funcao pode ser:
assinatura:   '(' ')'                    {/* Ignora declaracao da funcao*/} // Vazia, sendo caracterizada apenas pelos parenteses
            | '(' lista_parametros ')'   {/* Ignora declaracao da funcao*/} // Um ou mais parametros entre parenteses
;

// Uma lista de parametros de uma funcao pode ser:
lista_parametros:   parametro                      {/* Ignora declaracao da funcao*/} // Um unico parametro
                  | parametro ',' lista_parametros {/* Ignora declaracao da funcao*/} // Um parametro seguido de uma lista de parametros, separados por virgula
;

/**
 * Um parametro de uma funcao e:
 *
 * Um tipo possivelmente const ... 
 * ... seguido de seu nome
 *
 * OBS.: Nao pode ser vetor
 */
parametro: tipo_const TK_IDENTIFICADOR {/* Ignora declaracao da funcao*/} ;


// COMANDOS SIMPLES E BLOCOS DE COMANDOS


// Um bloco de comandos pode ser:
bloco_comandos:   '{' '}'                        {$$ = NULL;/* Nao cria nenhum filho */} // Vazio, sendo caracterizado apenas pelas chaves
                | '{' lista_comandos_simples '}' {$$ = $2;} // Uma lista de comandos simples
;

// Uma lista de comandos simples pode ser:
lista_comandos_simples:   comando_simples                         {$$ = $1;} // Um unico comando simples
                        | comando_simples lista_comandos_simples  {$$ = insere_no_comando(&$1, $2);} // Um comando simples, seguido de uma lista de comandos simples
;

// Um comando simples pode ser:
comando_simples:   controle_fluxo ';'         {$$ = $1;} // Um comando de controle de fluxo, terminado por ponto e virgula (;)
                 | TK_PR_RETURN expressao ';' {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_RETURN, "return", get_line_number()); $$ = preenche_nodo(&$$, $2, NULL, NULL, NULL);} // Um return, seguido de uma expressao e terminado por ponto e virgula (;)
                 | TK_PR_BREAK   ';'          {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_BREAK_CONTINUE, "break", get_line_number());} // Um break, terminado por ponto e virgula (;)
                 | TK_PR_CONTINUE ';'         {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_BREAK_CONTINUE, "continue", get_line_number());} // Um continue, terminado por ponto e virgula (;)
                 | comando_shift ';'          {$$ = $1;} // Um comando de shift, terminado por ponto e virgula (;)
                 | chamada_funcao ';'         {$$ = $1;} // Uma chamada de funcao, terminada por ponto e virgula (;)
                 | comando_es ';'             {$$ = $1;} // Um comando de e/s, terminado por ponto e virgula (;)
                 | atribuicao ';'             {$$ = $1;} // Uma atribuicao, terminada por ponto e virgula (;)
                 | var_local ';'              {$$ = $1;} // Uma declaracao de variavel local, terminada por ponto e virgula (;)
                 | bloco_comandos ';'         {$$ = $1;} // Um bloco de comandos, terminado por ponto e virgula (;)
;


// CHAMADA DE FUNCAO


/** 
 * Uma chamada de funcao e:
 *
 * O nome da funcao, seguido de parenteses sem argumentos
 *  OU
 * O nome da funcao, seguido de parenteses e uma lista de argumentos
 */
chamada_funcao:   TK_IDENTIFICADOR '(' ')'                  {$$ = cria_nodo_lexico($1, FUNC_CALL);} 
                | TK_IDENTIFICADOR '(' lista_argumentos ')' {$$ = cria_nodo_lexico($1, FUNC_CALL); $$ = preenche_nodo(&$$, $3, NULL, NULL, NULL);} 
;

// Uma lista de argumentos pode ser:
lista_argumentos:   argumento                      {$$ = $1;} // Um unico argumento
                  | argumento ',' lista_argumentos {$$ = preenche_nodo(&$1, $3, NULL, NULL, NULL);} // Um argumento seguido de uma lista de argumentos, separados por virgula
;

// Um argumento pode ser:
argumento: expressao {$$ = $1;} ; // Uma expressao


// COMANDOS SIMPLES

/**
 * Um vetor indexado e:
 *
 * Um identificador simples ...
 * ... seguido de uma empressao entre colchetes
 */
vetor_indexado: TK_IDENTIFICADOR '[' expressao ']' {$$ = cria_nodo_intermed(CARACTERE_ESPECIAL, VEC_IND, "[]", $1->linha_ocorrencia); $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID), $3, NULL, NULL);};

/**
 * Uma declaracao de variavel local e:
 *
 * Um tipo possivelmente const static, ...
 * ... seguido de uma lista de identificadores
 */
var_local:   tipo_const_estatico lista_identificadores_locais {$$ = $2;} ;

/**
 * Uma lista de identificadores locais pode ser:
 *
 * Um identificador local
 *  OU
 * Um identificador local, seguido de uma lista de identificadores locais, separados por virgula
 */
lista_identificadores_locais:   identificador_local                                  {$$ = $1;}
                              | identificador_local ',' lista_identificadores_locais {$$ = preenche_nodo(&$1, $3, NULL, NULL, NULL);}
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
identificador_local:   TK_IDENTIFICADOR                           {$$ = NULL;/* Nao considera os identificadores nao inicializados */} 
                     | TK_IDENTIFICADOR TK_OC_LE literal          {$$ = cria_nodo_lexico($2, VAR_INIT); $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID), $3, NULL, NULL);} 
                     | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {$$ = cria_nodo_lexico($2, VAR_INIT); $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID), cria_nodo_lexico($1, ID), NULL, NULL);} 
;

// Uma atribuicao pode ser:
atribuicao:   TK_IDENTIFICADOR '=' expressao {$$ = cria_nodo_intermed(CARACTERE_ESPECIAL, VAR_ATRIB, "=", get_line_number()); $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID), $3, NULL, NULL);} // Uma atribuicao a um identificador simples
            | vetor_indexado '=' expressao   {$$ = cria_nodo_intermed(CARACTERE_ESPECIAL, VAR_ATRIB, "=", get_line_number()); $$ = preenche_nodo(&$$, $1, $3, NULL, NULL);} // Uma atribuicao a um vetor indexado por uma expressao // TODO Verificar memleak nos char especiais
;

// Um comando de entrada ou saida pode ser:
comando_es:   TK_PR_INPUT TK_IDENTIFICADOR  {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_ES,  "input", get_line_number()); $$ = preenche_nodo(&$$, cria_nodo_lexico($2, ID), NULL, NULL, NULL);} // A palavra input, seguida de um identificador
            | TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_ES, "output", get_line_number()); $$ = preenche_nodo(&$$, cria_nodo_lexico($2, ID), NULL, NULL, NULL);} // A palavra output, seguida de um identificador
            | TK_PR_OUTPUT literal          {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_ES, "output", get_line_number()); $$ = preenche_nodo(&$$, $2, NULL, NULL, NULL);} // A palavra output, seguida de um literal
;

// Um operador de shift pode ser:
operador_shift:   TK_OC_SL {$$ = cria_nodo_lexico($1, CMD_SHIFT);} // O operador de shift left (<<) 
                | TK_OC_SR {$$ = cria_nodo_lexico($1, CMD_SHIFT);} // O operador de shift right (>>) 
;

/**
* Um comando de shift pode ser:
*
* Um identificador, seguido de um operador de shift e um literal inteiro positivo
*  OU
* Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo
*/
comando_shift:   TK_IDENTIFICADOR operador_shift TK_LIT_INT {$$ = preenche_nodo(&$2, cria_nodo_lexico($1, ID), cria_nodo_lexico($3, LIT_INT), NULL, NULL);}
               | vetor_indexado operador_shift TK_LIT_INT   {$$ = preenche_nodo(&$2, $1, cria_nodo_lexico($3, LIT_INT), NULL, NULL);}
;


// COMANDOS DE CONTROLE DE FLUXO


// Um comando de controle de fluxo pode ser:
controle_fluxo:   comando_if    {$$ = $1;} // Um comando if
                | comando_for   {$$ = $1;} // Um comando for
                | comando_while {$$ = $1;} // Um comando while
;

/**
 * Um comando if pode ser:
 *
 * A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos
 *  OU
 * A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos...
 * ... terminado com a palavra else, seguida de um bloco de comandos
 */
comando_if:   TK_PR_IF '(' expressao ')' bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_IF, "if", get_line_number()); $$ = preenche_nodo(&$$, $3, $5, NULL, NULL);} 
            | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_IF, "if", get_line_number()); $$ = preenche_nodo(&$$, $3, $5, $7, NULL);} 
;

/**
 * Um comando for e:
 *
 * A palavra for, seguida de, entre parenteses, ...
 * ... uma atribuicao, expressao e atribuicao, nessa ordem, separados por dois pontos (:)
 * ... terminado por um bloco de comandos
 */
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_FOR, "for", get_line_number());
                                                                                       $$ = preenche_nodo(&$$, $3, $5, $7, $9);};

/**
 * Um comando while pode ser:
 *
 * A palavra while, seguida de uma expressao entre parenteses...
 * ... terminado pela palavra do, seguida de um bloco de comandos
 */
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_WHILE, "while", get_line_number()); 
                                                                      $$ = preenche_nodo(&$$, $3, $6, NULL, NULL);} ;


// EXPRESSOES


// Uma expressao pode ser:
expressao:   expressao operador_binario_baixa_prec termo {$$ = preenche_nodo(&$2, $1, $3, NULL, NULL);} // Uma expressao associada a um termo, atraves de um operador de baixa precedencia
           | termo                                       {$$ = $1;} // Apenas um termo
;

// Um termo pode ser:
termo:   termo operador_binario_alta_prec fator {$$ = preenche_nodo(&$2, $1, $3, NULL, NULL);} // Um termo associado a um fator, atraves de um operador de alta precedencia
       | fator                                  {$$ = $1;} // Apenas um fator
;

// Um fator pode ser:
fator:   '(' expressao ')' {$$ = $2;} // Uma expressao entre parenteses
       | operando          {$$ = $1;} // Um operando valido da linguagem
;

// Um operando pode ser:
operando:   TK_IDENTIFICADOR                   {$$ = cria_nodo_lexico($1, ID);} // Um identificador
          | vetor_indexado                     {$$ = $1;} // Um vetor indexado por uma expressao
          | TK_LIT_INT                         {$$ = cria_nodo_lexico($1, LIT_INT);} // Um literal inteiro
          | TK_LIT_FLOAT                       {$$ = cria_nodo_lexico($1, LIT_FLOAT);} // Um literal float
          | chamada_funcao                     {$$ = $1;}// Uma chamada de funcao
          | TK_LIT_TRUE                        {$$ = cria_nodo_lexico($1, LIT_BOOL);} // Um literal TRUE
          | TK_LIT_FALSE                       {$$ = cria_nodo_lexico($1, LIT_BOOL);} // Um literal FALSE
          | operador_unario fator              {$$ = preenche_nodo(&$1, $2, NULL, NULL, NULL);} // Um operador unario aplicado a um fator
; 

// Um comparador relacional pode ser:
comparador_relacional:   TK_OC_GE {$$ = cria_nodo_lexico($1, BINOP);} // O comparador de maior ou igual (>=)
                       | TK_OC_LE {$$ = cria_nodo_lexico($1, BINOP);} // O comparador de menor ou igual (<=)
                       | TK_OC_EQ {$$ = cria_nodo_lexico($1, BINOP);} // O comparador de igualdade (==)
                       | TK_OC_NE {$$ = cria_nodo_lexico($1, BINOP);} // O comparador de diferenca (!=)
                       | '<'      {$$ = cria_nodo_intermed(yylval.valor_lexico->tipo, BINOP, yylval.valor_lexico->valor.nome, yylval.valor_lexico->linha_ocorrencia);} // O comparador de menor
                       | '>'      {$$ = cria_nodo_intermed(yylval.valor_lexico->tipo, BINOP, yylval.valor_lexico->valor.nome, yylval.valor_lexico->linha_ocorrencia);} // O comparador de maior
;

// Um operador logico pode ser:
operador_logico:   TK_OC_AND {$$ = cria_nodo_lexico($1, BINOP);} // O operdor AND (&&)
                 | TK_OC_OR  {$$ = cria_nodo_lexico($1, BINOP);} // O operador OR (||)
;

// Um operador binario de baixa precedencia pode ser:
operador_binario_baixa_prec:   '+'                   {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador de soma
                             | '-'                   {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador de subtracao
                             | '|'                   {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador bitwise or
                             | '&'                   {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador bitwise and
                             | comparador_relacional {$$ = $1;} // Um comparador relacional 
                             | operador_logico       {$$ = $1;}     // Um operador logico
                             | '?' expressao ':'     {/*TODO Aqui essa exp deve ser inserida como segundo filho*/} // A 'parte' interna de uma expressao condicional
;

// Um operador binario de alta precedencia pode ser:
operador_binario_alta_prec:   '*' {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador de multiplicacao
                            | '/' {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador de divisao
                            | '%' {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador de modulore, 
                            | '^' {$$ = cria_nodo_lexico(yylval.valor_lexico, BINOP);} // O operador de exponenciacao
;

// Um operador unario pode ser:
operador_unario:   '+' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de positividade explicita
                 | '-' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de inversao de sinal
                 | '!' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de negacao
                 | '&' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de acesso a endereco
                 | '*' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de resolucao de ponteiros
                 | '?' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de avaliacao de expressao
                 | '#' {$$ = cria_nodo_lexico(yylval.valor_lexico, UNOP);} // O operador de acesso a tabela hash
;

%%
    /* FUNCOES OPCIONAIS */

/* Funcao para casos de erro */
int yyerror(char const *s)
{
    int error_line = get_line_number(); // Linha onde ocorreu o erro
    
    /* Sinaliza na saida padrao que ocorreu um erro sintatico */
    fprintf(stderr,"%s\nOn line %d\n", s, error_line);
    
    /* Libera toda a memoria da AST sendo criada */
    libera(arvore);

    /* Retorna 1 */
    return 1;
}
