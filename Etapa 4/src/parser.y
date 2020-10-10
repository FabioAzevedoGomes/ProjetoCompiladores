%{
    #include <stdio.h>
    #include "symbol_table_management.h" // Methods for managing the Symbol Table
    #include "tree.h"                    // Methods for managing the AST

    // Function for getting tokens from input 
    int yylex(void);
    
    // Function for getting current line
    int get_line_number();
    
    // Function for handling syntactic errors
    int yyerror (char const *s);

    // Extern reference to the AST
    extern void *arvore;

    // Pointer to a structure containing semantic error details
    error_t *error_data = NULL;
%}

// Union for token lexical value
%union{
    node_t* node;            // Token node in the AST
    lexical_value_t* lexval; // Token lexical value
    st_entry_t* symbol;      // Token symbol in the Symbol Table
    LanguageType type;       // Token type according to language definition
}

// Define verbose syntax error reports
%define parse.error verbose

// Tokens
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
%token<lexval>TK_OC_LE
%token<lexval>TK_OC_GE
%token<lexval>TK_OC_EQ
%token<lexval>TK_OC_NE
%token<lexval>TK_OC_AND
%token<lexval>TK_OC_OR
%token<lexval>TK_OC_SL
%token<lexval>TK_OC_SR
%token TK_OC_FORWARD_PIPE
%token TK_OC_BASH_PIPE
%token<lexval>TK_LIT_INT
%token<lexval>TK_LIT_FLOAT
%token<lexval>TK_LIT_FALSE
%token<lexval>TK_LIT_TRUE
%token<lexval>TK_LIT_CHAR
%token<lexval>TK_LIT_STRING
%token<lexval>TK_IDENTIFICADOR
%token TOKEN_ERRO

// Special characters are identified by their lexical value
%token<lexval> ',' 
%token<lexval> ';' 
%token<lexval> ':' 
%token<lexval> '('
%token<lexval> ')' 
%token<lexval> '[' 
%token<lexval> ']' 
%token<lexval> '{'
%token<lexval> '}' 
%token<lexval> '+' 
%token<lexval> '-' 
%token<lexval> '|' 
%token<lexval> '*' 
%token<lexval> '/' 
%token<lexval> '<' 
%token<lexval> '>' 
%token<lexval> '=' 
%token<lexval> '!' 
%token<lexval> '&' 
%token<lexval> '%' 
%token<lexval> '#' 
%token<lexval> '^' 
%token<lexval> '.' 
%token<lexval> '$' 
%token<lexval> '?'

// Initial grammar symbol
%start programa

%type <symbol> lista_identificadores_globais
%type <symbol> identificador_global
%type <symbol> lista_parametros
%type <symbol> parametro
%type <symbol> assinatura

// Non-Terminal types
%type <node> bloco_comandos 
%type <node> lista_comandos_simples 
%type <node> comando_es 
%type <node> comando_simples 
%type <node> var_local 
%type <node> argumento 
%type <node> lista_identificadores_locais 
%type <node> identificador_local 
%type <node> atribuicao 
%type <node> operador_shift 
%type <node> comando_shift 
%type <node> vetor_indexado 
%type <node> comando_if 
%type <node> comando_for 
%type <node> comando_while 
%type <node> controle_fluxo 
%type <node> termo 
%type <node> expressao 
%type <node> operador_logico 
%type <node> comparador_relacional 
%type <node> operador_binario_alta_prec 
%type <node> operador_binario_baixa_prec 
%type <node> chamada_funcao 
%type <node> operando 
%type <node> fator  
%type <node> declaracao_funcao 
%type <node> programa 
%type <node> operador_unario 
%type <node> literal
%type <node> lista_argumentos

%type<type> tipo
%type<type> tipo_estatico
%type<type> tipo_const
%type<type> tipo_const_estatico

%%

/**
 * Um programa pode ser:
 *
 * Vazio
 * Uma declaracao de funcao, seguida de mais programa
 * Uma definicao de variavel, seguida de mais programa
 */
programa:   /* empty */                 {$$ = NULL;}                                    // Nao cria nodo para vazio
          | declaracao_funcao programa  {$$ = create_command_chain($1,$2); arvore = $$;} // Insere o resto do programa como proximo da declaracao de funcao
          | var_global programa         {$$ = $2;}                                      // Ignora variaveis nao inicializadas
;


// TIPOS E LITERAIS


// Um tipo valido pode ser:
tipo:   TK_PR_INT       {$$ = TYPE_INT;}    // A palavra int
      | TK_PR_FLOAT     {$$ = TYPE_FLOAT;}  // A palavra float
      | TK_PR_CHAR      {$$ = TYPE_CHAR;}   // A palavra char
      | TK_PR_BOOL      {$$ = TYPE_BOOL;}   // A palavra bool
      | TK_PR_STRING    {$$ = TYPE_STRING;} // A palavra string
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
literal:   TK_LIT_INT       {$$ = create_lexical_node($1, TYPE_INT,    CMD_OPERAND);} // Cria um nodo para o lit int
         | TK_LIT_FLOAT     {$$ = create_lexical_node($1, TYPE_FLOAT,  CMD_OPERAND);} // Cria um nodo para o lit float
         | TK_LIT_TRUE      {$$ = create_lexical_node($1, TYPE_BOOL,   CMD_OPERAND);} // Cria um nodo para o lit true
         | TK_LIT_FALSE     {$$ = create_lexical_node($1, TYPE_BOOL,   CMD_OPERAND);} // Cria um nodo para o lit false
         | TK_LIT_CHAR      {$$ = create_lexical_node($1, TYPE_CHAR,   CMD_OPERAND);} // Cria um nodo para o lit char
         | TK_LIT_STRING    {$$ = create_lexical_node($1, TYPE_STRING, CMD_OPERAND);} // Cria um nodo para o lit string
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
var_global: tipo_estatico lista_identificadores_globais ';' {declare_symbol_list($2, $1);
                                                             free_lexical_value($3, TYPE_NA);} // Libera a memoria usada no delimitador ponto e virgula (;)
;

/**
 * Uma lista de identificadores globais pode ser:
 *
 * Apenas um identificador
 * Um identificador, seguido de uma lista de identificadores, separados por virgula
 */
lista_identificadores_globais:   identificador_global                                   {$$ = $1;} // O simbolo e a propria lista
                               | identificador_global ',' lista_identificadores_globais {make_symbol_list($1, $3);
                                                                                         free_lexical_value($2, TYPE_NA);} // Libera a memoria usada no delimitador virgula (,)
; 

/**
 * Um identificador global pode ser:
 *
 * Um identificador simples
 * Um identificador indexado por um literal inteiro positivo
 */
identificador_global:   TK_IDENTIFICADOR                    {$$ = make_symbol_entry($1, 1, KIND_IDENTIFIER);}
                      | TK_IDENTIFICADOR '[' TK_LIT_INT ']' {$$ = make_symbol_entry($1, $3->value.integer, KIND_IDENTIFIER);
                                                             free_lexical_value($2, TYPE_NA);   // Libera a memoria usada para o delimitador colchete ([)
                                                             free_lexical_value($3, TYPE_NA);   // Libera a memoria usada para o inteiro usado no tamanho
                                                             free_lexical_value($4, TYPE_NA);}  // Libera a memoria usada para o delimitador colchete (])
;


// DECLARACAO DE FUNCOES


/**
 * Uma declaracao de funcao e:
 *
 * Um tipo estatico de retorno...
 * ...seguido pelo nome da funcao e sua assinatura ...
 * ... terminada por um bloco de comandos
 */ 
// TODO Insere na tabela de simbolos global E na tabela de simbolos do proprio escopo da funcao
declaracao_funcao: tipo_estatico TK_IDENTIFICADOR assinatura bloco_comandos {$$ = create_function_declaration(create_lexical_node($2, $1, CMD_FUNCTION_DECLARATION), $4, $1);} 
;

/**
 * A assinatura de uma funcao pode ser:
 *
 * Vazia, caracterizada apenas pelos parenteses
 * Um ou mais parametros entre parenteses
 */
// TODO Insere a lista de parametros na declaracao da funcao (Talvez seja aqui que se inicia um escopo novo para funcoes)
assinatura:   '(' ')'                    {$$ = NULL;
                                          free_lexical_value($1, TYPE_NA);  // Libera a memoria usada para o delimitador parenteses (
                                          free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador parenteses )
            | '(' lista_parametros ')'   {$$ = $2;
                                          free_lexical_value($1, TYPE_NA);  // Libera a memoria usada para o delimitador parenteses (
                                          free_lexical_value($3, TYPE_NA);} // Libera a memoria usada para o delimitador parenteses )
;

/**
 * Uma lista de parametros de uma funcao pode ser:
 *
 * Um unico parametro
 * Um parametro, seguido de uma lista de parametros separados por virugla (,)
 */
lista_parametros:   parametro                      {$$ = $1;}
                  | parametro ',' lista_parametros {free_lexical_value($2, TYPE_NA);} // TODO Insere simbolo na lista de parametros de funcao ? n sei se aqui
;

/**
 * Um parametro de uma funcao e:
 *
 * Um tipo possivelmente const ... 
 * ... seguido de seu nome
 *
 * OBS.: Nao pode ser vetor
 */
parametro: tipo_const TK_IDENTIFICADOR { } // TODO Insere simbolo na lista de parametros de funcao
;


// COMANDOS SIMPLES E BLOCOS DE COMANDOS


/**
 * Um bloco de comandos pode ser:
 *
 * Vazio, sendo caracterizado apenas pelas chaves
 * Uma lista de comandos simples entre chaves
 */
// TODO Exit scope here
bloco_comandos:   '{' '}'                        {free_lexical_value($1, TYPE_NA); // Libera a memoria usada para o delimitador {
                                                  free_lexical_value($2, TYPE_NA); // Libera a memoria usada para o delimitador }
                                                  $$ = NULL;}                      // Nao contem nenhum comando
                | '{' lista_comandos_simples '}' {free_lexical_value($1, TYPE_NA); // Libera a memoria usada para o delimitador {
                                                  free_lexical_value($3, TYPE_NA); // Libera a memoria usada para o delimitador }
                                                  $$ = $2;                         // O primeiro da lista e o primeiro do bloco
                                                  }
;

/** 
 * Uma lista de comandos simples pode ser:
 *
 * Um unico comando simples (O primeiro de um bloco a ser detectado)
 * Um comando simples, seguido de uma lista de comandos simples, separados por virgula (,)
 */
lista_comandos_simples:   comando_simples                         {$$ = $1;}                           // O comando e o primeiro
                        | comando_simples lista_comandos_simples  {$$ = create_command_chain($1, $2);} // A lista de comandos e o comando segunte do comando primeiro
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
comando_simples:   controle_fluxo ';'         {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
                 | TK_PR_RETURN expressao ';' {$$ = create_return($2); free_lexical_value($3, TYPE_NA);}                   // Libera a memoria usada para o delimitador (;)
                 | TK_PR_BREAK   ';'          {$$ = create_break(get_line_number()); free_lexical_value($2, TYPE_NA);}     // Libera a memoria usada para o delimitador (;)
                 | TK_PR_CONTINUE ';'         {$$ = create_continue(get_line_number()); free_lexical_value($2, TYPE_NA);}  // Libera a memoria usada para o delimitador (;)
                 | comando_shift ';'          {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
                 | chamada_funcao ';'         {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
                 | comando_es ';'             {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
                 | atribuicao ';'             {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
                 | var_local ';'              {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
                 | bloco_comandos ';'         {$$ = $1; free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
;


// CHAMADA DE FUNCAO


/** 
 * Uma chamada de funcao e:
 *
 * O nome da funcao, seguido de parenteses sem argumentos
 * O nome da funcao, seguido de parenteses e uma lista de argumentos
 */
chamada_funcao:   TK_IDENTIFICADOR '(' ')'                  {$$ = create_function_call(create_lexical_node($1, TYPE_TBA, CMD_OPERAND), NULL);
                                                             free_lexical_value($2, TYPE_NA);       // Libera a memoria usada para o delimitador (
                                                             free_lexical_value($3, TYPE_NA);}      // Libera a memoria usada para o delimitador )
                | TK_IDENTIFICADOR '(' lista_argumentos ')' {$$ = create_function_call(create_lexical_node($1, TYPE_TBA, CMD_OPERAND), $3);
                                                             free_lexical_value($2, TYPE_NA);                // Libera a memoria usada para o delimitador (;)
                                                             free_lexical_value($4, TYPE_NA);}               // Libera a memoria usada para o delimitador (;)
;

/** 
 * Uma lista de argumentos pode ser:
 *
 * Um unico argumento
 * Um argumento, seguido de uma lista de argumentos, separados por virgula
 */
lista_argumentos:   argumento                      {$$ = $1;}                          // Retorna o nodo criado para esse argumento
                  | argumento ',' lista_argumentos {$$ = insert_child($1, $3);         // Insere a lista de argumentos como filho do primeiro argumento
                                                    free_lexical_value($2, TYPE_INT);} // Libera a memoria usada para o delimitador (,)
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
vetor_indexado: TK_IDENTIFICADOR '[' expressao ']' {$$ = create_vector_access(create_lexical_node($1, TYPE_TBA, CMD_OPERAND), $3);
                                                    free_lexical_value($2, TYPE_NA);   // Libera a memoria usada para o delimitador (;)
                                                    free_lexical_value($4, TYPE_NA);}  // Libera a memoria usada para o delimitador (;)
;

/**
 * Uma declaracao de variavel local e:
 *
 * Um tipo possivelmente const static, ...
 * ... seguido de uma lista de identificadores
 */
var_local: tipo_const_estatico lista_identificadores_locais {$$ = $2;} // TODO Verifica e atualiza a tabela de simbolos com as novas variaveis declaradas 
;

/**
 * Uma lista de identificadores locais pode ser:
 *
 * Um identificador local
 * Um identificador local, seguido de uma lista de identificadores locais, separados por virgula
 */
lista_identificadores_locais:   identificador_local                                  {$$ = $1;} // TODO Insere na lista de inserção da tabela de simbolos ? talvez nao, n sei
                              | identificador_local ',' lista_identificadores_locais {$$ = insert_child($1, $3);        // Insere na lista de variaveis sendo declaradas
                                                                                      free_lexical_value($2, TYPE_NA);} // Libera a memoria usada para o delimitador (;)
;

/**
 * Um identificador local pode ser:
 *
 * Um identificador simples
 * Um identificador simples inicializado com um literal
 * Um identificador simples inicializado com outro identificador simples
 */
identificador_local:   TK_IDENTIFICADOR                           {$$ = NULL;} // TODO Insere na lista de inserção para a tabela de simbolos
                     | TK_IDENTIFICADOR TK_OC_LE literal          {create_init(create_lexical_node($1, TYPE_TBA, CMD_OPERAND),
                                                                               $3,
                                                                               create_lexical_node($2, TYPE_NA,  CMD_INIT_VARIABLE));}
                     | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {create_init(create_lexical_node($1, TYPE_TBA, CMD_OPERAND),
                                                                               create_lexical_node($3, TYPE_TBA, CMD_OPERAND),
                                                                               create_lexical_node($2, TYPE_NA,  CMD_INIT_VARIABLE));}
;

/** Uma atribuicao pode ser:
 * 
 * Uma atribuicao a um identificador simples
 * Uma atribuicao a um identificador indexado
 */
atribuicao:   TK_IDENTIFICADOR '=' expressao {$$ = create_attrib(create_lexical_node($1, TYPE_TBA, CMD_OPERAND), $3, create_lexical_node($2, TYPE_TBA, CMD_ATTRIB_VARIABLE));}
            | vetor_indexado '=' expressao   {$$ = create_attrib($1, $3, create_lexical_node($2, TYPE_TBA, CMD_ATTRIB_VARIABLE));}
;

/** Um comando de entrada ou saida pode ser:
 * 
 * A palavra input, seguida de um identificador
 * A palavra output, seguida de um identificador
 * A palavra output, seguida de um literal
 */
comando_es:   TK_PR_INPUT TK_IDENTIFICADOR  {$$ = create_input(create_lexical_node($2, TYPE_NA, CMD_OPERAND));}
            | TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = create_output(create_lexical_node($2, TYPE_NA, CMD_OPERAND));}
            | TK_PR_OUTPUT literal          {$$ = create_output($2);}
;

/**
 * Um operador de shift pode ser:
 *
 * O operador de shift left  <<
 * O operador de shift right >> 
 */
operador_shift:   TK_OC_SL {$$ = create_lexical_node($1, TYPE_TBA, CMD_SHIFT);} // Cria nodo para o operador de shift left 
                | TK_OC_SR {$$ = create_lexical_node($1, TYPE_TBA, CMD_SHIFT);} // Cria nodo para o operador de shift right
;

/**
* Um comando de shift pode ser:
*
* Um identificador, seguido de um operador de shift e um literal inteiro positivo
* Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo
*/
comando_shift:   TK_IDENTIFICADOR operador_shift TK_LIT_INT {$$ = create_shift(create_lexical_node($1, TYPE_INT, CMD_OPERAND), 
                                                                               create_lexical_node($3, TYPE_INT, CMD_OPERAND), $2);}
               | vetor_indexado operador_shift TK_LIT_INT   {$$ = create_shift($1, create_lexical_node($3, TYPE_INT, CMD_OPERAND), $2);}
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
comando_if:   TK_PR_IF '(' expressao ')' bloco_comandos {$$ = create_if($3,$5, NULL);      // Cria um nodo para o IF
                                                         free_lexical_value($2, TYPE_NA);  // Libera a memoria usada para o delimitador (
                                                         free_lexical_value($4, TYPE_NA);} // Libera a memoria usada para o delimitador )
            | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos {$$ = create_if($3,$5,$7);         // Cria um nodo para o IF
                                                                                   free_lexical_value($2, TYPE_NA);  // Libera a memoria usada para o delimitador (
                                                                                   free_lexical_value($4, TYPE_NA);} // Libera a memoria usada para o delimitador )
;

/**
 * Um comando for e:
 *
 * A palavra for, seguida de, entre parenteses, ...
 * ... uma atribuicao, expressao e atribuicao, nessa ordem, separados por dois pontos (:)
 * ... terminado por um bloco de comandos
 */
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos {$$ = create_for($3,$5,$7,$9);     // Cria um nodo para o FOR
                                                                                       free_lexical_value($2, TYPE_NA);  // Libera a memoria usada para o delimitador (
                                                                                       free_lexical_value($4, TYPE_NA);  // Libera a memoria usada para o delimitador dois pontos :
                                                                                       free_lexical_value($6, TYPE_NA);  // Libera a memoria usada para o delimitador dois pontos :
                                                                                       free_lexical_value($8, TYPE_NA);} // Libera a memoria usada para o delimitador )
;

/**
 * Um comando while pode ser:
 *
 * A palavra while, seguida de uma expressao entre parenteses...
 * ... terminado pela palavra do, seguida de um bloco de comandos
 */
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos {$$ = create_while($3,$6);         // Cria um nodo para o WHILE
                                                                      free_lexical_value($2, TYPE_NA);  // Libera a memoria usada para o delimitador (
                                                                      free_lexical_value($4, TYPE_NA);} // Libera a memoria usada para o delimitador )
;

// EXPRESSOES


/**
 * Uma expressao pode ser:
 *
 * Uma expressao associada a um termo por um operador binario de baixa precedencia
 * Uma termo apenas
 */
expressao:   expressao operador_binario_baixa_prec termo {$$ = create_binop($2, $1, $3);} // Cria o nodo de operação binária
           | termo                                       {$$ = $1;}             // Retorna o nodo do termo
;

/** Um termo pode ser:
 *
 * Um termo asoociado a um fator por um operador binario de alta precedencia
 * Um fator apenas
 */
termo:   termo operador_binario_alta_prec fator {$$ = create_binop($2, $1, $3);} // Cria o nodo de operação binária
       | fator                                  {$$ = $1;}                       // Retorna o nodo do fator
;

/**
 * Um fator pode ser:
 *
 * Uma expressao entre parenteses
 * Um operando da linguagem
 */
fator:   '(' expressao ')' {$$ = $2;                          // Retorna o nodo da expressao
                            free_lexical_value($1, TYPE_NA);  // Libera a memoria usada para o delimitador (
                            free_lexical_value($3, TYPE_NA);} // Libera a memoria usada para o delimitador )
       | operando          {$$ = $1;}                         // Retorna o nodo do operando
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
operando:   TK_IDENTIFICADOR       {$$ = create_lexical_node($1, TYPE_TBA, CMD_OPERAND);}   // Cria um nodo para o identificador
          | vetor_indexado         {$$ = $1;}                
          | TK_LIT_INT             {$$ = create_lexical_node($1, TYPE_INT,   CMD_OPERAND);} // Cria um nodo para o literal inteiro
          | TK_LIT_FLOAT           {$$ = create_lexical_node($1, TYPE_FLOAT, CMD_OPERAND);} // Cria um nodo para o literal float
          | chamada_funcao         {$$ = $1;}                                      
          | TK_LIT_TRUE            {$$ = create_lexical_node($1, TYPE_BOOL,  CMD_OPERAND);} // Cria um nodo para o literal true
          | TK_LIT_FALSE           {$$ = create_lexical_node($1, TYPE_BOOL,  CMD_OPERAND);} // Cria um nodo para o literal false
          | operador_unario fator  {$$ = create_unop($1, $2);}
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
comparador_relacional:   TK_OC_GE {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o comparador maior ou igual
                       | TK_OC_LE {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o comparador menor ou igual
                       | TK_OC_EQ {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o comparador igual
                       | TK_OC_NE {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o comparador diferente
                       | '<'      {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o comparador menor
                       | '>'      {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o comparador maior
;

/* Um operador logico pode ser:
 *
 * O operador AND (&&)
 * O operador OR  (||)
 */
operador_logico:   TK_OC_AND {$$ = create_lexical_node($1, TYPE_BOOL, CMD_BINOP);} // Cria nodo para o operador AND
                 | TK_OC_OR  {$$ = create_lexical_node($1, TYPE_BOOL, CMD_BINOP);} // Cria nodo para o operador OR
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
operador_binario_baixa_prec:   '+'                   {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria um nodo para o operador de some
                             | '-'                   {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria um nodo para o operador de subtracao
                             | '|'                   {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria um nodo para o operador bitwise or
                             | '&'                   {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria um nodo para o operador bitwise and
                             | comparador_relacional {$$ = $1;}                           // Retorna o nodo do comparador relacional 
                             | operador_logico       {$$ = $1;}                           // Retorna o nodo do operador logico
                             | '?' expressao ':'     {$$ = create_ternop($2);             // Cria o operador ternario parcialmente
                                                      free_lexical_value($1, TYPE_NA);    // Libera a memoria usada para o delimitador interrogacao (?)
                                                      free_lexical_value($3, TYPE_NA);}   // Libera a memoria usada para o delimitador dois pontos (:)
;

/**
 * Um operador binario de alta precedencia pode ser:
 *
 * O operador de multiplicacao
 * O operador de divisao
 * O operador de modulo
 * O operador de exponenciacao
 */
operador_binario_alta_prec:   '*' {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o operador de multiplicacao
                            | '/' {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o operador de divisao
                            | '%' {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o operador de modulo
                            | '^' {$$ = create_lexical_node($1, TYPE_INT, CMD_BINOP);} // Cria nodo para o operador de exponenciacao
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
operador_unario:   '+' {$$ = create_lexical_node($1, TYPE_INT,  CMD_UNOP);} // Cria nodo para o operador de positividade explicita
                 | '-' {$$ = create_lexical_node($1, TYPE_INT,  CMD_UNOP);} // Cria nodo para o operador de inversao de sinal
                 | '!' {$$ = create_lexical_node($1, TYPE_BOOL, CMD_UNOP);} // Cria nodo para o operador de negacao
                 | '&' {$$ = create_lexical_node($1, TYPE_INT,  CMD_UNOP);} // Cria nodo para o operador de acesso a endereco
                 | '*' {$$ = create_lexical_node($1, TYPE_INT,  CMD_UNOP);} // Cria nodo para o operador de resolucao de ponteiros
                 | '?' {$$ = create_lexical_node($1, TYPE_BOOL, CMD_UNOP);} // Cria nodo para o operador de avaliacao de expressao
                 | '#' {$$ = create_lexical_node($1, TYPE_INT,  CMD_UNOP);} // Cria nodo para o operador de acesso a tabela hash
;

%%
// OPTIONAL FUNCTIONS

// Functions for handling syntax errors
int yyerror(char const *s)
{
    // Get line where error ocurred
    int error_line = get_line_number(); 
    
    // Signal syntax error
    fprintf(stderr,"%s\nOn line %d\n", s, error_line);
    
    // Return 1
    return 1;
}
