%{
    #include <cstdio>
    #include "Manager.h"

    // Function for getting tokens from input 
    extern "C" int yylex(void);
    
    // Function for getting current line
    int get_line_number();
    
    // Function for handling syntactic errors
    int yyerror (char const *s);

    // Extern reference to the AST
    extern void *arvore;

    // Data structures manager
    Manager mngr;

%}

// Union for token lexical value
%union{
    Node* node;     // AST Node
    Token* lexval;  // Token
    Type type;      // Token type
    Symbol* symbol; // Symbol
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
%start inicio

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
%type <node> definicao_funcao
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
 * Simbolo inicial da gramatica
 */
inicio: programa {mngr.addDriverCode(arvore);} // Apos reconhecer o programa, adicionar o codigo que faz jump para a main
;

/**
 * Um programa pode ser:
 *
 * Vazio
 * Uma declaracao de funcao, seguida de mais programa
 * Uma definicao de variavel, seguida de mais programa
 */
programa:   /* empty */                 {$$ = NULL;} // Fim
          | declaracao_funcao programa  {$1->insertCommand($2); $$ = $1; arvore = $$;} // Insere o resto do programa como proximo da declaracao de funcao
          | var_global programa         {$$ = $2;}                                     // Ignora variaveis nao inicializadas
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
literal:   TK_LIT_INT       {$$ = mngr.createLiteral($1, TYPE_INT);}    // Cria um nodo para o lit int
         | TK_LIT_FLOAT     {$$ = mngr.createLiteral($1, TYPE_FLOAT);}  // Cria um nodo para o lit float
         | TK_LIT_TRUE      {$$ = mngr.createLiteral($1, TYPE_BOOL);}   // Cria um nodo para o lit true
         | TK_LIT_FALSE     {$$ = mngr.createLiteral($1, TYPE_BOOL);}   // Cria um nodo para o lit false
         | TK_LIT_CHAR      {$$ = mngr.createLiteral($1, TYPE_CHAR);}   // Cria um nodo para o lit char
         | TK_LIT_STRING    {$$ = mngr.createLiteral($1, TYPE_STRING);} // Cria um nodo para o lit string
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
var_global: tipo_estatico lista_identificadores_globais ';' {mngr.declareVariables($1); delete $3;}
;

/**
 * Uma lista de identificadores globais pode ser:
 *
 * Apenas um identificador
 * Um identificador, seguido de uma lista de identificadores, separados por virgula
 */
lista_identificadores_globais:  lista_identificadores_globais ',' identificador_global {mngr.addToVarList($3); delete $2;}
                              | identificador_global                                   {mngr.addToVarList($1);}
; 

/**
 * Um identificador global pode ser:
 *
 * Um identificador simples
 * Um identificador indexado por um literal inteiro positivo
 */
identificador_global:   TK_IDENTIFICADOR                    {$$ = new Symbol($1, NAT_IDENTIFIER, TYPE_ANY, 1, true);
                                                             delete $1;}
                      | TK_IDENTIFICADOR '[' TK_LIT_INT ']' {$$ = new Symbol($1, NAT_VECTOR, TYPE_ANY, $3->getValue().integer, true);
                                                             delete $1; delete $2; delete $3, delete $4;} 
;


// DECLARACAO DE FUNCOES

/**
 * Uma definicao de funcao e:
 * Um tipo estatico de retorno...
 * ...seguido pelo nome da funcao e sua assinatura
 */
definicao_funcao: tipo_estatico TK_IDENTIFICADOR assinatura {$$ = mngr.createDeclaration($2, $1, ST_FUNCTION_DECLARATION);
                                                                  mngr.declareFunction($$, $1);}
                                                            
;

/**
 * Uma declaracao de funcao e:
 *
 * A definicao da sua assinatura ...
 * ... terminada por um bloco de comandos
 */
declaracao_funcao: definicao_funcao bloco_comandos {$$ = mngr.createFunctionDeclaration($1, $2); mngr.leaveFunction();}
;

/**
 * A assinatura de uma funcao pode ser:
 *
 * Vazia, caracterizada apenas pelos parenteses
 * Um ou mais parametros entre parenteses
 */
assinatura:   '(' ')'                    {$$ = NULL; delete $1; delete $2;} // Libera a memoria usada para o delimitador parenteses )
            | '(' lista_parametros ')'   {$$ = $2;   delete $1; delete $3;} // Libera a memoria usada para o delimitador parenteses )
;

/**
 * Uma lista de parametros de uma funcao pode ser:
 *
 * Um unico parametro
 * Um parametro, seguido de uma lista de parametros separados por virugla (,)
 */
lista_parametros:   lista_parametros ',' parametro {mngr.addToVarList($3); delete $2;}
                  | parametro                      {mngr.addToVarList($1);}
;

/**
 * Um parametro de uma funcao e:
 *
 * Um tipo possivelmente const ... 
 * ... seguido de seu nome
 *
 * OBS.: Nao pode ser vetor
 */
parametro: tipo_const TK_IDENTIFICADOR {$$ = new Symbol($2, NAT_IDENTIFIER, $1, 1); delete $2;}
;


// COMANDOS SIMPLES E BLOCOS DE COMANDOS

// O inicio de um bloco de comandos inicia um novo escopo
bloco_comandos_inicio: '{' {delete $1; mngr.enterScope();} // Entra em um novo escopo
;

// O fim de um bloco de comandos fecha o escopo atual
bloco_comandos_fim: '}' {delete $1; mngr.leaveScope();} // Sai do escopo atual
;

/**
 * Um bloco de comandos pode ser:
 *
 * Vazio, sendo caracterizado apenas pelas chaves de inicio de e fim
 * Uma lista de comandos simples entre chaves
 */
bloco_comandos:   bloco_comandos_inicio bloco_comandos_fim                        {$$ = NULL;} 
                | bloco_comandos_inicio lista_comandos_simples bloco_comandos_fim {$$ = $2;}
;

/** 
 * Uma lista de comandos simples pode ser:
 *
 * Um unico comando simples (O primeiro de um bloco a ser detectado)
 * Um comando simples, seguido de uma lista de comandos simples, separados por virgula (,)
 */
lista_comandos_simples:   comando_simples lista_comandos_simples {if ($1 != NULL){ $1->insertCommand($2); $$ = $1; } else $$ = $2;} // A lista de comandos e o comando segunte do comando primeiro
                        | comando_simples                        {$$ = $1;}                        // O comando e o primeiro
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
comando_simples:   controle_fluxo ';'         {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
                 | TK_PR_RETURN expressao ';' {$$ = mngr.createReturn($2); delete $3;}              // Libera a memoria usada para o delimitador (;)
                 | TK_PR_BREAK   ';'          {$$ = mngr.createBreak($2->getLine()); delete $2;}    // Libera a memoria usada para o delimitador (;)
                 | TK_PR_CONTINUE ';'         {$$ = mngr.createContinue($2->getLine()); delete $2;} // Libera a memoria usada para o delimitador (;)
                 | comando_shift ';'          {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
                 | chamada_funcao ';'         {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
                 | comando_es ';'             {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
                 | atribuicao ';'             {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
                 | var_local ';'              {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
                 | bloco_comandos ';'         {$$ = $1; delete $2;} // Libera a memoria usada para o delimitador (;)
;


// CHAMADA DE FUNCAO


/** 
 * Uma chamada de funcao e:
 *
 * O nome da funcao, seguido de parenteses sem argumentos
 * O nome da funcao, seguido de parenteses e uma lista de argumentos
 */
chamada_funcao:   TK_IDENTIFICADOR '(' ')'                  {$$ = mngr.createFunctionCall(mngr.createId($1, ST_FUNCTION_CALL), NULL);
                                                             delete $2; delete $3;}  // Libera a memoria usada para os delimitadores
                | TK_IDENTIFICADOR '(' lista_argumentos ')' {$$ = mngr.createFunctionCall(mngr.createId($1, ST_FUNCTION_CALL), $3);
                                                             delete $2; delete $4;}// Libera a memoria usada para os delimitadores
;

/** 
 * Uma lista de argumentos pode ser:
 *
 * Um unico argumento
 * Um argumento, seguido de uma lista de argumentos, separados por virgula
 */
lista_argumentos:   argumento                      {$$ = $1;} // Retorna o nodo criado para esse argumento
                  | argumento ',' lista_argumentos {$1->insertNext($3); $$ = $1;    
                                                    delete $2;} // Libera a memoria usada para o delimitador (,)
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
vetor_indexado: TK_IDENTIFICADOR '[' expressao ']' {$$ = mngr.createVectorAccess(mngr.createId($1, ST_OPERAND), $3);
                                                    delete $2; delete $4;}  // Libera a memoria usada para os delimitadores
;

/**
 * Uma declaracao de variavel local e:
 *
 * Um tipo possivelmente const static, ...
 * ... seguido de uma lista de identificadores
 */
var_local: tipo_const_estatico lista_identificadores_locais {$$ = $2; mngr.declareVariables($1);} // Reseta a lista de vars locais (Memoria foi liberada na declaracao)
;

/**
 * Uma lista de identificadores locais pode ser:
 *
 * Um identificador local
 * Um identificador local, seguido de uma lista de identificadores locais, separados por virgula
 */
lista_identificadores_locais:   identificador_local                                  {$$ = $1;}
                              | identificador_local ',' lista_identificadores_locais {if ($1 != NULL){$1->insertNext($3); $$ = $1;} else $$ = $3; 
                                                                                      delete $2;} // Libera a memoria usada para o delimitador
;

/**
 * Um identificador local pode ser:
 *
 * Um identificador simples
 * Um identificador simples inicializado com um literal
 * Um identificador simples inicializado com outro identificador simples
 */
identificador_local:   TK_IDENTIFICADOR                           {$$ = NULL; mngr.addToVarList(new Symbol($1, NAT_IDENTIFIER, TYPE_ANY, 1));
                                                                   delete $1;}
                     | TK_IDENTIFICADOR TK_OC_LE literal          {$$ = mngr.createInitialization(mngr.createDeclaration($1, TYPE_ANY, ST_OPERAND), 
                                                                                                  mngr.createDeclaration($2, TYPE_ANY, ST_INIT_VARIABLE),
                                                                                                  $3);
                                                                   mngr.addToVarList(new Symbol($1, NAT_IDENTIFIER, TYPE_ANY, 1), $$);
                                                                  }
                     | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {$$ = mngr.createInitialization(mngr.createDeclaration($1, TYPE_ANY, ST_OPERAND), 
                                                                                                  mngr.createDeclaration($2, TYPE_ANY, ST_INIT_VARIABLE),
                                                                                                  mngr.createId($3, ST_OPERAND));
                                                                   mngr.addToVarList(new Symbol($1, NAT_IDENTIFIER, TYPE_ANY, 1), $$);}
;

/** Uma atribuicao pode ser:
 * 
 * Uma atribuicao a um identificador simples
 * Uma atribuicao a um identificador indexado
 */
atribuicao:   TK_IDENTIFICADOR '=' expressao {$$ = mngr.createAttribution(mngr.createId($1, ST_OPERAND),
                                                                          mngr.createDeclaration($2, TYPE_ANY, ST_ATTRIB_VARIABLE),
                                                                          $3);}
            | vetor_indexado '=' expressao   {$$ = mngr.createAttribution($1, 
                                                                          mngr.createDeclaration($2, TYPE_ANY, ST_ATTRIB_VARIABLE),
                                                                          $3);}
;

/** Um comando de entrada ou saida pode ser:
 * 
 * A palavra input, seguida de um identificador
 * A palavra output, seguida de um identificador
 * A palavra output, seguida de um literal
 */
comando_es:   TK_PR_INPUT TK_IDENTIFICADOR  {$$ = mngr.createInput(mngr.createId($2, ST_OPERAND));}
            | TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = mngr.createOutput(mngr.createId($2, ST_OPERAND));}
            | TK_PR_OUTPUT literal          {$$ = mngr.createOutput($2);}
;

/**
 * Um operador de shift pode ser:
 *
 * O operador de shift left  <<
 * O operador de shift right >> 
 */
operador_shift:   TK_OC_SL {$$ = mngr.createDeclaration($1, TYPE_ANY, ST_SHIFT);} // Cria nodo para o operador de shift left 
                | TK_OC_SR {$$ = mngr.createDeclaration($1, TYPE_ANY, ST_SHIFT);} // Cria nodo para o operador de shift right
;

/**
* Um comando de shift pode ser:
*
* Um identificador, seguido de um operador de shift e um literal inteiro positivo
* Um vetor indexado por uma expressao, seguido de um operador de shift e um literal inteiro positivo
*/
comando_shift:   TK_IDENTIFICADOR operador_shift TK_LIT_INT {$$ = mngr.createShift(mngr.createId($1, ST_OPERAND), $2, mngr.createLiteral($3, TYPE_INT));}
               | vetor_indexado operador_shift TK_LIT_INT   {$$ = mngr.createShift($1, $2, mngr.createLiteral($3, TYPE_INT));}
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
comando_if:   TK_PR_IF '(' expressao ')' bloco_comandos {$$ = mngr.createIf($3, $5); delete $2; delete $4;}
            | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos {$$ = mngr.createIf($3,$5,$7); delete $2; delete $4;}
;

/**
 * Um comando for e:
 *
 * A palavra for, seguida de, entre parenteses, ...
 * ... uma atribuicao, expressao e atribuicao, nessa ordem, separados por dois pontos (:)
 * ... terminado por um bloco de comandos
 */
comando_for: TK_PR_FOR '(' atribuicao ':' expressao ':' atribuicao ')' bloco_comandos {$$ = mngr.createFor($3,$5,$7,$9);
                                                                                       delete $2; delete $4, delete $6, delete $8;}
;

/**
 * Um comando while pode ser:
 *
 * A palavra while, seguida de uma expressao entre parenteses...
 * ... terminado pela palavra do, seguida de um bloco de comandos
 */
comando_while: TK_PR_WHILE '(' expressao ')' TK_PR_DO bloco_comandos {$$ = mngr.createWhile($3,$6);
                                                                      delete $2; delete $4;}
;

// EXPRESSOES


/**
 * Uma expressao pode ser:
 *
 * Uma expressao associada a um termo por um operador binario de baixa precedencia
 * Uma termo apenas
 */
expressao:   expressao operador_binario_baixa_prec termo {$$ = mngr.createBinop($1, $2, $3);} // Cria o nodo de operação binária
           | termo                                       {$$ = $1;}                           // Retorna o nodo do termo
;

/** Um termo pode ser:
 *
 * Um termo asoociado a um fator por um operador binario de alta precedencia
 * Um fator apenas
 */
termo:   termo operador_binario_alta_prec fator {$$ = mngr.createBinop($1, $2, $3);} // Cria o nodo de operação binária
       | fator                                  {$$ = $1;}                           // Retorna o nodo do fator
;

/**
 * Um fator pode ser:
 *
 * Uma expressao entre parenteses
 * Um operando da linguagem
 */
fator:   '(' expressao ')' {$$ = $2; delete $1; delete $3;}
       | operando          {$$ = $1;}
;

/**
 * Um operando pode ser:
 *
 * Um identificador
 * Um vetor indexado
 * Um literal
 * Uma chamada de funcao
 * Um operador unario aplicado a um fator
 */
operando:   TK_IDENTIFICADOR       {$$ = mngr.createId($1, ST_OPERAND, false);}
          | vetor_indexado         {$$ = $1; $1->setRval();}                
          | literal                {$$ = $1;}
          | chamada_funcao         {$$ = $1;}                                      
          | operador_unario fator  {$$ = mngr.createUnop($1, $2);}
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
comparador_relacional:   TK_OC_GE {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o comparador maior ou igual
                       | TK_OC_LE {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o comparador menor ou igual
                       | TK_OC_EQ {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o comparador igual
                       | TK_OC_NE {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o comparador diferente
                       | '<'      {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o comparador menor
                       | '>'      {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o comparador maior
;

/* Um operador logico pode ser:
 *
 * O operador AND (&&)
 * O operador OR  (||)
 */
operador_logico:   TK_OC_AND {$$ = mngr.createDeclaration($1, TYPE_BOOL, ST_BINOP);} // Cria nodo para o operador AND
                 | TK_OC_OR  {$$ = mngr.createDeclaration($1, TYPE_BOOL, ST_BINOP);} // Cria nodo para o operador OR
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
operador_binario_baixa_prec:   '+'                   {$$ = mngr.createDeclaration($1, TYPE_ANY, ST_BINOP);}  // Cria um nodo para o operador de soma
                             | '-'                   {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);}  // Cria um nodo para o operador de subtracao
                             | '|'                   {$$ = mngr.createDeclaration($1, TYPE_BOOL, ST_BINOP);} // Cria um nodo para o operador bitwise or
                             | '&'                   {$$ = mngr.createDeclaration($1, TYPE_BOOL, ST_BINOP);} // Cria um nodo para o operador bitwise and
                             | operador_logico       {$$ = $1;}                           // Retorna o nodo do operador logico
                             | '?' expressao ':'     {$$ = mngr.createTernop($2);             // Cria o operador ternario parcialmente
                                                      delete $1; delete $3;}   // Libera a memoria usada para os delimitadors
;

/**
 * Um operador binario de alta precedencia pode ser:
 *
 * O operador de multiplicacao
 * O operador de divisao
 * O operador de modulo
 * O operador de exponenciacao
 */
operador_binario_alta_prec:   '*' {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o operador de multiplicacao
                            | '/' {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o operador de divisao
                            | '%' {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o operador de modulo
                            | '^' {$$ = mngr.createDeclaration($1, TYPE_INT, ST_BINOP);} // Cria nodo para o operador de exponenciacao
                            | comparador_relacional {$$ = $1;}                           // Retorna o nodo do comparador relacional 
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
operador_unario:   '+' {$$ = mngr.createDeclaration($1, TYPE_INT , ST_UNOP);} // Cria nodo para o operador de positividade explicita
                 | '-' {$$ = mngr.createDeclaration($1, TYPE_INT , ST_UNOP);} // Cria nodo para o operador de inversao de sinal
                 | '!' {$$ = mngr.createDeclaration($1, TYPE_BOOL, ST_UNOP);} // Cria nodo para o operador de negacao
                 | '&' {$$ = mngr.createDeclaration($1, TYPE_ANY , ST_UNOP);} // Cria nodo para o operador de acesso a endereco
                 | '*' {$$ = mngr.createDeclaration($1, TYPE_ANY , ST_UNOP);} // Cria nodo para o operador de resolucao de ponteiros
                 | '?' {$$ = mngr.createDeclaration($1, TYPE_BOOL, ST_UNOP);} // Cria nodo para o operador de avaliacao de expressao
                 | '#' {$$ = mngr.createDeclaration($1, TYPE_ANY , ST_UNOP);} // Cria nodo para o operador de acesso a tabela hash
;

%%

// Functions for handling syntax errors
int yyerror(char const *s)
{    
    // Signal syntax error
    std::cerr << s << std::endl << "On line " << get_line_number() << std::endl;
    
    // Return 1
    return 1;
}
