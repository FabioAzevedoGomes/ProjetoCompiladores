%{
    #include <stdio.h>
    #include "ast.h" // Funcoes da arvore e tipos de dados
    #include "symbol_table.h" // Funcoes da tabela de simbolos

    /* Funcao de obtencao dos tokens */
    int yylex(void);
    
    /* Funcao que retorna a linha atual */
    int get_line_number();
    
    /* Funcao de tratamento de erro */
    int yyerror (char const *s);

    /* Referencia para a AST sendo montada */
    extern void *arvore;

    /* Pilha de tabelas de simbolos que sera usada na analise semantica*/
    symbol_table_stack_t* pilha_tabelas_simbolos;
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

// Declara os caracteres especiais como tendo o tipo valor_lexico
%token<valor_lexico> ',' 
';' 
':' 
'('
')' 
'[' 
']' 
'{'
'}' 
'+' 
'-' 
'|' 
'*' 
'/' 
'<' 
'>' 
'=' 
'!' 
'&' 
'%' 
'#' 
'^' 
'.' 
'$' 
'?'

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
operador_atrib
tipo
tipo_estatico
tipo_const
tipo_const_estatico
parametro
lista_identificadores_globais
%%

/**
 * Um programa pode ser:
 *
 * Vazio
 * Uma declaracao de funcao, seguida de mais programa
 * Uma definicao de variavel, seguida de mais programa
 */
programa:   /* empty */                 {$$ = NULL;}                              // Nao cria nodo para vazio
          | declaracao_funcao programa  {arvore = $1; insere_no_comando(&$1,$2);} // Insere o resto do programa como proximo da declaracao de funcao
          | var_global programa         {$$ = $2;}                                // Ignora variaveis nao inicializadas
;


// TIPOS E LITERAIS


// Um tipo valido pode ser:
tipo:   TK_PR_INT       {$$ = cria_nodo_lexico(NULL, PR, TIPO_INT);}    // A palavra int
      | TK_PR_FLOAT     {$$ = cria_nodo_lexico(NULL, PR, TIPO_FLOAT);}  // A palavra float
      | TK_PR_CHAR      {$$ = cria_nodo_lexico(NULL, PR, TIPO_CHAR);}   // A palavra char
      | TK_PR_BOOL      {$$ = cria_nodo_lexico(NULL, PR, TIPO_BOOL);}   // A palavra bool
      | TK_PR_STRING    {$$ = cria_nodo_lexico(NULL, PR, TIPO_STRING);} // A palavra string
; 

/**Um literal valido pode ser:
 *
 * Um literal aritmetico
 * Um literal logico
 * Um literal boolean true
 * Um literal boolean false
 * Um literal char
 * Um literal string
 */
literal:   TK_LIT_INT       {$$ = cria_nodo_lexico($1, LIT_INT,    TIPO_INT);}    // Cria um nodo para o lit int
         | TK_LIT_FLOAT     {$$ = cria_nodo_lexico($1, LIT_FLOAT,  TIPO_FLOAT);}  // Cria um nodo para o lit float
         | TK_LIT_TRUE      {$$ = cria_nodo_lexico($1, LIT_BOOL,   TIPO_BOOL);}   // Cria um nodo para o lit true
         | TK_LIT_FALSE     {$$ = cria_nodo_lexico($1, LIT_BOOL,   TIPO_BOOL);}   // Cria um nodo para o lit false
         | TK_LIT_CHAR      {$$ = cria_nodo_lexico($1, LIT_CHAR,   TIPO_CHAR);}   // Cria um nodo para o lit char
         | TK_LIT_STRING    {$$ = cria_nodo_lexico($1, LIT_STRING, TIPO_STRING);} // Cria um nodo para o lit string
;

// Um tipo estatico valido pode ser:
tipo_estatico:   TK_PR_STATIC tipo {$$ = $2;} // A palavra static, seguida de um tipo
               | tipo              {$$ = $1;} // Apenas um tipo
;

// Um tipo const pode ser:
tipo_const:   TK_PR_CONST tipo  {$$ = $2;} // A palavra const, seguida de um tipo
            | tipo              {$$ = $1;} // Apenas um tipo
;

// Um tipo const estatico pode ser:
tipo_const_estatico:   tipo                          {$$ = $1;} // Apenas um tipo
                     | TK_PR_STATIC tipo             {$$ = $2;} // A palavra static seguida de um tipo
                     | TK_PR_CONST tipo              {$$ = $2;} // A palavra const seguida de um tipo
                     | TK_PR_STATIC TK_PR_CONST tipo {$$ = $3;} // A palavra const e static, seguidas de um tipo
;


// DECLARACAO DE VARIAVEIS GLOBAIS


/**
 * Uma declaracao de variavel global e:
 *
 * Um tipo possivelmente estatico...
 * ... seguido de uma lista de identificadores...
 * ... e terminado por ponto e virgula (;)
 */ 
var_global: tipo_estatico lista_identificadores_globais ';' {$2->tipo = $1->tipo; 
                                                             libera_valor_lexico($3, DELIM);} // Libera a memoria usada no delimitador ponto e virgula (;)
;

/**
 * Uma lista de identificadores globais pode ser:
 *
 * Apenas um identificador
 * Um identificador, seguido de uma lista de identificadores, separados por virgula
 */
lista_identificadores_globais:   identificador_global                                   {/* Ignora variavel nao inicializada*/} 
                               | identificador_global ',' lista_identificadores_globais {libera_valor_lexico($2, DELIM);} // Libera a memoria usada no delimitador virgula (,)
; 

/**
 * Um identificador global pode ser:
 *
 * Um identificador simples
 * Um identificador indexado por um literal inteiro positivo
 */
identificador_global:   TK_IDENTIFICADOR                    {/*TODO Insere na tabela de simbolos global*/}     // Cria entrada na tabela de simbolos para o identificador
                      | TK_IDENTIFICADOR '[' TK_LIT_INT ']' {/*TODO Insere na tabela de simbolos global*/ 
                                                             libera_valor_lexico($2, DELIM);   // Libera a memoria usada para o delimitador colchete ([)
                                                             libera_valor_lexico($4, DELIM);}  // Libera a memoria usada para o delimitador colchete (])
;


// DECLARACAO DE FUNCOES


/**
 * Uma declaracao de funcao e:
 *
 * Um tipo estatico de retorno...
 * ...seguido pelo nome da funcao e sua assinatura ...
 * ... terminada por um bloco de comandos
 */ 
declaracao_funcao: tipo_estatico TK_IDENTIFICADOR assinatura bloco_comandos {$$ = cria_nodo_lexico($2, FUNC_LIST, $1->tipo); // Cria nodo com o id da funcao
                                                                             $$ = preenche_nodo(&$$,$4, NULL, NULL, NULL); // Insere o bloco de comandos como primeiro filho
                                                                             /*TODO Adicionar os argumentos na lista de args da tabela de simbolos*/} 
;

/**
 * A assinatura de uma funcao pode ser:
 *
 * Vazia, caracterizada apenas pelos parenteses
 * Um ou mais parametros entre parenteses
 */
assinatura:   '(' ')'                    {libera_valor_lexico($1, DELIM);  // Libera a memoria usada para o delimitador parenteses (
                                          libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador parenteses )
            | '(' lista_parametros ')'   {libera_valor_lexico($1, DELIM);  // Libera a memoria usada para o delimitador parenteses (
                                          libera_valor_lexico($3, DELIM);} // Libera a memoria usada para o delimitador parenteses )
;

/**
 * Uma lista de parametros de uma funcao pode ser:
 *
 * Um unico parametro
 * Um parametro, seguido de uma lista de parametros separados por virugla (,)
 */
lista_parametros:   parametro                     {/*TODO Insere na tabela de simbolos da funcao e args da funcao pai */}
                  | parametro ',' lista_parametros {libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador virgula (,)
;

/**
 * Um parametro de uma funcao e:
 *
 * Um tipo possivelmente const ... 
 * ... seguido de seu nome
 *
 * OBS.: Nao pode ser vetor
 */
parametro: tipo_const TK_IDENTIFICADOR {$$ = cria_nodo_lexico($2, ID, $1->tipo);} // Libera a memoria usada para o identificador 
;


// COMANDOS SIMPLES E BLOCOS DE COMANDOS


/**
 * Um bloco de comandos pode ser:
 *
 * Vazio, sendo caracterizado apenas pelas chaves
 * Uma lista de comandos simples entre chaves
 */
bloco_comandos:   '{' '}'                        {libera_valor_lexico($1, DELIM); // Libera a memoria usada para o delimitador {
                                                  libera_valor_lexico($2, DELIM); // Libera a memoria usada para o delimitador }
                                                  $$ = NULL;}                     // Nao contem nenhum comando
                | '{' lista_comandos_simples '}' {libera_valor_lexico($1, DELIM); // Libera a memoria usada para o delimitador {
                                                  libera_valor_lexico($3, DELIM); // Libera a memoria usada para o delimitador }
                                                  $$ = $2;                       // O primeiro da lista e o primeiro do bloco
                                                  push_st(&symbol_table, cria_tabela_simbolos());} // Coloca uma nova tabela de simbolos no topo da pilha
;

/** 
 * Uma lista de comandos simples pode ser:
 *
 * Um unico comando simples
 * Um comando simples, seguido de uma lista de comandos simples, separados por virgula (,)
 */
lista_comandos_simples:   comando_simples                         {$$ = $1;}                         // O comando e o primeiro
                        | comando_simples lista_comandos_simples  {$$ = insere_no_comando(&$1, $2);} // A lista de comandos e o comando segunte do comando primeiro
;

/**
 * Um comando simples pode ser: (todos terminados por ponto e virgula (;))
 *
 * Um comando de controle de fluxo
 * Um return, seguido de uma expressao 
 * Um break
 * Um continue
 * Um comando de shift
 * Uma chamada de funcao
 * Um comando de e/s
 * Uma atribuicao
 * Uma declaracao de variavel local
 * Um bloco de comandos
 */
comando_simples:   controle_fluxo ';'         {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
                 | TK_PR_RETURN expressao ';' {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_RETURN, PR, "return", get_line_number()); // Cria um nodo para o comando de RETURN
                                               $$ = preenche_nodo(&$$, $2, NULL, NULL, NULL);                                       // Insere a expressao como filho
                                               libera_valor_lexico($3, DELIM);}                                                     // Libera a memoria usada para o delimitador (;)
                 | TK_PR_BREAK   ';'          {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_BREAK_CONTINUE, PR, "break", get_line_number()); // Cria um nodo para o comando BREAK
                                               libera_valor_lexico($2, DELIM);}                                                            // Libera a memoria usada para o delimitador (;)
                 | TK_PR_CONTINUE ';'         {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_BREAK_CONTINUE, PR, "continue", get_line_number()); // Cria um nodo para o comando CONTINUE
                                               libera_valor_lexico($2, DELIM);}                                                               // Libera a memoria usada para o delimitador (;)
                 | comando_shift ';'          {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
                 | chamada_funcao ';'         {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
                 | comando_es ';'             {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
                 | atribuicao ';'             {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
                 | var_local ';'              {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
                 | bloco_comandos ';'         {$$ = $1; libera_valor_lexico($2, DELIM);} // Libera a memoria usada para o delimitador (;)
;


// CHAMADA DE FUNCAO


/** 
 * Uma chamada de funcao e:
 *
 * O nome da funcao, seguido de parenteses sem argumentos
 * O nome da funcao, seguido de parenteses e uma lista de argumentos
 */
chamada_funcao:   TK_IDENTIFICADOR '(' ')'                  {$$ = cria_nodo_lexico($1, FUNC_CALL, /*TODO Consulta a tabela de simbolos, descobre o tipo desse id*/); // Cria um nodo para a chamada de funcao
                                                             libera_valor_lexico($2, DELIM);       // Libera a memoria usada para o delimitador (
                                                             libera_valor_lexico($3, DELIM);}      // Libera a memoria usada para o delimitador )
                | TK_IDENTIFICADOR '(' lista_argumentos ')' {$$ = cria_nodo_lexico($1, FUNC_CALL, /*TODO Consulta a tabela de simbolos, descobre o tipo desse id*/);          // Cria um nodo para a chamada de funcao
                                                             $$ = preenche_nodo(&$$, $3, NULL, NULL, NULL); // Insere a lista de argumentos como filho
                                                             libera_valor_lexico($2, DELIM);                // Libera a memoria usada para o delimitador (;)
                                                             libera_valor_lexico($4, DELIM);}               // Libera a memoria usada para o delimitador (;)
;

/** 
 * Uma lista de argumentos pode ser:
 *
 * Um unico argumento
 * Um argumento, seguido de uma lista de argumentos, separados por virgula
 */
lista_argumentos:   argumento                      {$$ = $1;}                                      // Retorna o nodo criado para esse argumento
                  | argumento ',' lista_argumentos {$$ = preenche_nodo(&$1, $3, NULL, NULL, NULL); // Insere a lista de argumentos como filho do primeiro argumento
                                                    libera_valor_lexico($2, DELIM);}               // Libera a memoria usada para o delimitador (,)
;

/**
 * Um argumento pode ser:
 *
 * Uma expressao
 */
argumento: expressao {$$ = $1;} // Retorna o nodo criado
;

// COMANDOS SIMPLES

/**
 * Um vetor indexado e:
 *
 * Um identificador simples ...
 * ... seguido de uma expressao entre colchetes
 */
vetor_indexado: TK_IDENTIFICADOR '[' expressao ']' {$$ = cria_nodo_intermed(CARACTERE_ESPECIAL, VEC_IND, /*TODO Verifica tabela de simbolos, encontra o tipo desse id*/, "[]", $1->linha_ocorrencia); // Cria um nodo para a indexacao
                                                    $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID, /*TODO Verifica tabela de simbolos, encontra o tipo desse id*/), $3, NULL, NULL);                // Insere um nodo do identificador como filho
                                                    libera_valor_lexico($2, DELIM);   // Libera a memoria usada para o delimitador (;)
                                                    libera_valor_lexico($4, DELIM);}  // Libera a memoria usada para o delimitador (;)
;

/**
 * Uma declaracao de variavel local e:
 *
 * Um tipo possivelmente const static, ...
 * ... seguido de uma lista de identificadores
 */
var_local: tipo_const_estatico lista_identificadores_locais {$$ = $2;} //Retorna o nodo criado para a lista
;

/**
 * Uma lista de identificadores locais pode ser:
 *
 * Um identificador local
 * Um identificador local, seguido de uma lista de identificadores locais, separados por virgula
 */
lista_identificadores_locais:   identificador_local                                  {$$ = $1;} // Retorna o nodo criado para este identificador
                              | identificador_local ',' lista_identificadores_locais {$$ = preenche_nodo(&$1, $3, NULL, NULL, NULL); // Insere a lista de ids como filho do primeiro id
                                                                                      libera_valor_lexico($2, DELIM);}               // Libera a memoria usada para o delimitador (;)
;

/**
 * Um identificador local pode ser:
 *
 * Um identificador simples
 * Um identificador simples inicializado com um literal
 * Um identificador simples inicializado com outro identificador simples
 */
identificador_local:   TK_IDENTIFICADOR                           {$$ = NULL;                       // Ignora identificadores nao inicializados
                                                                   libera_valor_lexico($1, DELIM);} // Libera a memoria usada para o identificador
                     | TK_IDENTIFICADOR TK_OC_LE literal          {$$ = cria_nodo_lexico($2, VAR_INIT);                                 // Cria um nodo para a inicializacao
                                                                   $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID), $3, NULL, NULL);}  // Insere o id e o literal como filhos
                     | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {$$ = cria_nodo_lexico($2, VAR_INIT);                                                      // Cria um nodo para a inicializacao
                                                                   $$ = preenche_nodo(&$$, cria_nodo_lexico($1, ID), cria_nodo_lexico($3, ID), NULL, NULL);} // Insere os ids como filhos
;

//Um operador de atribuicao e:
operador_atrib: '=' {$$ = cria_nodo_lexico($1,VAR_ATRIB);} // Cria nodo para a operacao de atribuicao
;

/** Uma atribuicao pode ser:
 * 
 * Uma atribuicao a um identificador simples
 * Uma atribuicao a um identificador indexado
 */
atribuicao:   TK_IDENTIFICADOR operador_atrib expressao {$$ = preenche_nodo(&$2, cria_nodo_lexico($1, ID), $3, NULL, NULL);} // Insere o id e a expressao como filhos
            | vetor_indexado operador_atrib expressao   {$$ = preenche_nodo(&$2, $1, $3, NULL, NULL);} // Insere o vetor indexado e a expressao como filhos
;

/** Um comando de entrada ou saida pode ser:
 * 
 * A palavra input, seguida de um identificador
 * A palavra output, seguida de um identificador
 * A palavra output, seguida de um literal
 */
comando_es:   TK_PR_INPUT TK_IDENTIFICADOR  {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_ES,  "input", get_line_number()); // Cria nodo para o comando de input
                                             $$ = preenche_nodo(&$$, cria_nodo_lexico($2, ID), NULL, NULL, NULL);}            // Insere o identificador como filho
            | TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_ES, "output", get_line_number()); // Cria nodo para o comando de output
                                             $$ = preenche_nodo(&$$, cria_nodo_lexico($2, ID), NULL, NULL, NULL);}            // Insere o identificador como filho
            | TK_PR_OUTPUT literal          {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_ES, "output", get_line_number()); // Cria nodo para a palavra output
                                             $$ = preenche_nodo(&$$, $2, NULL, NULL, NULL);}                                  // Insere o nodo literal como filho
;

/**
 * Um operador de shift pode ser:
 *
 * O operador de shift left  <<
 * O operador de shift right >> 
 */
operador_shift:   TK_OC_SL {$$ = cria_nodo_lexico($1, CMD_SHIFT);} // Cria nodo para o operador de shift left 
                | TK_OC_SR {$$ = cria_nodo_lexico($1, CMD_SHIFT);} // Cria nodo para o operador de shift right
;

/**
* Um comando de shift pode ser:
*
* Um identificador, seguido de um operador de shift e um literal inteiro positivo
* Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo
*/
comando_shift:   TK_IDENTIFICADOR operador_shift TK_LIT_INT {$$ = preenche_nodo(&$2, cria_nodo_lexico($1, ID), cria_nodo_lexico($3, LIT_INT), NULL, NULL);} // Insere o id e literal int como filhos do shift
               | vetor_indexado operador_shift TK_LIT_INT   {$$ = preenche_nodo(&$2, $1, cria_nodo_lexico($3, LIT_INT), NULL, NULL);}                       // Insere o vetor indexado e o literal int como filhos do shift
;


// COMANDOS DE CONTROLE DE FLUXO


/** 
 * Um comando de controle de fluxo pode ser:
 *
 * Um comando if
 * Um comando fot
 * Um comando while
 */
controle_fluxo:   comando_if    {$$ = $1;} // Retorna o nodo do comando if
                | comando_for   {$$ = $1;} // Retorna o nodo do comando for
                | comando_while {$$ = $1;} // Retorna o nodo do comando while
;

/**
 * Um comando if pode ser:
 *
 * A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos
 * A palavra if, seguida de uma expressao entre parenteses e um bloco de comandos...
 * ... terminado com a palavra else, seguida de um bloco de comandos
 */
comando_if:   TK_PR_IF '(' expressao ')' bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_IF, "if", get_line_number());  // Cria um nodo para o comando if
                                                         $$ = preenche_nodo(&$$, $3, $5, NULL, NULL); // Insere a expressao e o primeiro comando do bloco como filhos
                                                         libera_valor_lexico($2, DELIM);  // Libera a memoria usada para o delimitador (
                                                         libera_valor_lexico($4, DELIM);} // Libera a memoria usada para o delimitador )
            | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_IF, "if", get_line_number()); // Cria um nodo para o comando if
                                                                                   $$ = preenche_nodo(&$$, $3, $5, $7, NULL); // Insere a expressao, o primeiro e o segundo bloco de comandos como filhos
                                                                                   libera_valor_lexico($2, DELIM);  // Libera a memoria usada para o delimitador (
                                                                                   libera_valor_lexico($4, DELIM);} // Libera a memoria usada para o delimitador )
;

/**
 * Um comando for e:
 *
 * A palavra for, seguida de, entre parenteses, ...
 * ... uma atribuicao, expressao e atribuicao, nessa ordem, separados por dois pontos (:)
 * ... terminado por um bloco de comandos
 */
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_FOR, "for", get_line_number()); // Cria um nodo para o comando for
                                                                                       $$ = preenche_nodo(&$$, $3, $5, $7, $9); // Insere a atriuicao inicial, a expressao de parada, a atribuicao de loop e o bloco de comandos como filhos
                                                                                       libera_valor_lexico($2, DELIM);  // Libera a memoria usada para o delimitador (
                                                                                       libera_valor_lexico($4, DELIM);  // Libera a memoria usada para o delimitador dois pontos :
                                                                                       libera_valor_lexico($6, DELIM);  // Libera a memoria usada para o delimitador dois pontos :
                                                                                       libera_valor_lexico($8, DELIM);} // Libera a memoria usada para o delimitador )
;

/**
 * Um comando while pode ser:
 *
 * A palavra while, seguida de uma expressao entre parenteses...
 * ... terminado pela palavra do, seguida de um bloco de comandos
 */
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos {$$ = cria_nodo_intermed(PALAVRA_RESERVADA, CMD_WHILE, "while", get_line_number()); // Cria um nodo para o comando while
                                                                      $$ = preenche_nodo(&$$, $3, $6, NULL, NULL); // Insere a expressao e o bloco de comandos como filhos
                                                                      libera_valor_lexico($2, DELIM);  // Libera a memoria usada para o delimitador (
                                                                      libera_valor_lexico($4, DELIM);} // Libera a memoria usada para o delimitador )
;

// EXPRESSOES


/**
 * Uma expressao pode ser:
 *
 * Uma expressao associada a um termo por um operador binario de baixa precedencia
 * Uma termo apenas
 */
expressao:   expressao operador_binario_baixa_prec termo {$$ = preenche_nodo(&$2, $1, $3, NULL, NULL);} // Insere a expressao e o termo como filhos do operador binario
           | termo                                       {$$ = $1;}                                     // Retorna o nodo do termo
;

/** Um termo pode ser:
 *
 * Um termo asoociado a um fator por um operador binario de alta precedencia
 * Um fator apenas
 */
termo:   termo operador_binario_alta_prec fator {$$ = preenche_nodo(&$2, $1, $3, NULL, NULL);} // Insere o termo e o fator como filhos do operador binario
       | fator                                  {$$ = $1;}                                     // Retorna o nodo do fator
;

/**
 * Um fator pode ser:
 *
 * Uma expressao entre parenteses
 * Um operando da linguagem
 */
fator:   '(' expressao ')' {$$ = $2;                         // Retorna o nodo da expressao
                            libera_valor_lexico($1, DELIM);  // Libera a memoria usada para o delimitador (
                            libera_valor_lexico($3, DELIM);} // Libera a memoria usada para o delimitador )
       | operando          {$$ = $1;}                        // Retorna o nodo do operando
;

/**
 * Um operando pode ser:
 *
 * Um identificador
 * Um vetor indexado
 * Um literal inteiro
 * Um literal float
 * Uma chamada de funcao
 * Um literal TRUE
 * Um literal FALSE
 * Um operador unario aplicado a um fator
 */
operando:   TK_IDENTIFICADOR       {$$ = cria_nodo_lexico($1, ID);}                 // Cria um nodo para o identificador
          | vetor_indexado         {$$ = $1;}                                       // Retorna o nodo do vetor indexado
          | TK_LIT_INT             {$$ = cria_nodo_lexico($1, LIT_INT);}            // Cria um nodo para o literal inteiro
          | TK_LIT_FLOAT           {$$ = cria_nodo_lexico($1, LIT_FLOAT);}          // Cria um nodo para o literal float
          | chamada_funcao         {$$ = $1;}                                       // Retorna o nodo da chamada de funcao
          | TK_LIT_TRUE            {$$ = cria_nodo_lexico($1, LIT_BOOL);}           // Cria um nodo para o literal true
          | TK_LIT_FALSE           {$$ = cria_nodo_lexico($1, LIT_BOOL);}           // Cria um nodo para o literal false
          | operador_unario fator  {$$ = preenche_nodo(&$1, $2, NULL, NULL, NULL);} // Insere o fator como filho do operador unario
; 

/**
 * Um comparador relacional pode ser:
 *
 * O comparador de maior ou igual (>=)
 * O comparador de menor ou igual (<=)
 * O comparador de igualdade (==)
 * O comparador de diferenca (!=)
 * O comparador de menor
 * O comparador de maior
 */
comparador_relacional:   TK_OC_GE {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o comparador maior ou igual
                       | TK_OC_LE {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o comparador menor ou igual
                       | TK_OC_EQ {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o comparador igual
                       | TK_OC_NE {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o comparador diferente
                       | '<'      {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o comparador menor
                       | '>'      {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o comparador maior
;

/* Um operador logico pode ser:
 *
 * O operador AND (&&)
 * O operador OR  (||)
 */
operador_logico:   TK_OC_AND {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o operador AND
                 | TK_OC_OR  {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o operador OR
;

/** Um operador binario de baixa precedencia pode ser:
 *
 * O operador de soma
 * O operador de subtracao
 * O operador bitwise or
 * O operador bitwise and
 * Um comparador relacional 
 * Um operador logico
 * A 'parte' interna de uma expressao condicional
 */
operador_binario_baixa_prec:   '+'                   {$$ = cria_nodo_lexico($1, BINOP);} // Cria um nodo para o operador de some
                             | '-'                   {$$ = cria_nodo_lexico($1, BINOP);} // Cria um nodo para o operador de subtracao
                             | '|'                   {$$ = cria_nodo_lexico($1, BINOP);} // Cria um nodo para o operador bitwise or
                             | '&'                   {$$ = cria_nodo_lexico($1, BINOP);} // Cria um nodo para o operador bitwise and
                             | comparador_relacional {$$ = $1;}                          // Retorna o nodo do comparador relacional 
                             | operador_logico       {$$ = $1;}                          // Retorna o nodo do operador logico
                             | '?' expressao ':'     {$$ = cria_nodo_intermed(CARACTERE_ESPECIAL, TERNOP, "?:", get_line_number()); // Cria um nood para o operador ternario
                                                      $$ = preenche_nodo(&$$,$2, NULL, NULL, NULL);     // Insere a expressao como filho
                                                      libera_valor_lexico($1, DELIM);  // Libera a memoria usada para o delimitador interrogacao (?)
                                                      libera_valor_lexico($3, DELIM);} // Libera a memoria usada para o delimitador dois pontos (:)
;

/**
 * Um operador binario de alta precedencia pode ser:
 *
 * O operador de multiplicacao
 * O operador de divisao
 * O operador de modulo
 * O operador de exponenciacao
 */
operador_binario_alta_prec:   '*' {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o operador de multiplicacao
                            | '/' {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o operador de divisao
                            | '%' {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o operador de modulo
                            | '^' {$$ = cria_nodo_lexico($1, BINOP);} // Cria nodo para o operador de exponenciacao
;

/** Um operador unario pode ser:
 *
 * O operador de positividade explicita
 * O operador de inversao de sinal
 * O operador de negacao
 * O operador de acesso a endereco
 * O operador de resolucao de ponteiros
 * O operador de avaliacao de expressao
 * O operador de acesso a tabela hash
 */
operador_unario:   '+' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de positividade explicita
                 | '-' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de inversao de sinal
                 | '!' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de negacao
                 | '&' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de acesso a endereco
                 | '*' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de resolucao de ponteiros
                 | '?' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de avaliacao de expressao
                 | '#' {$$ = cria_nodo_lexico($1, UNOP);} // Cria nodo para o operador de acesso a tabela hash
;

%%
    /* FUNCOES OPCIONAIS */

/* Funcao para casos de erro */
int yyerror(char const *s)
{
    int error_line = get_line_number(); // Linha onde ocorreu o erro
    
    /* Sinaliza na saida padrao que ocorreu um erro sintatico */
    fprintf(stderr,"%s\nOn line %d\n", s, error_line);
    
    /* Retorna 1 */
    return 1;
}
