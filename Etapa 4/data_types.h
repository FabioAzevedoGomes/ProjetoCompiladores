#ifndef DATA_TYPES_H
#define DATA_TYPES_H

// Etapa 3

/**
 * Union com os diferentes tipos de valores para os tokens correspondentes 
 */
union Valores_Token
{
    char *nome; // Valor de tokens do tipo CARACTERE_ESPECIAL, OPERADOR e IDENTIFICADOR

    int inteiro;           // Valor de tokens do tipo TK_LIT_INT
    float ponto_flutuante; // Valor de tokens do tipo TK_LIT_FLOAT
    char *string;          // Valor de tokens do tipo TK_LIT_STRING
    char caractere;        // Valor de tokens do tipo TK_LIT_CHAR
    int booleano;          // Valor de tokens do tipo TK_LIT_BOOL
};

// Os 4 tipos validos para tokens definidos na especificacao da E3 mais um tipo para comandos
typedef enum Tipos_Token
{
    CARACTERE_ESPECIAL,
    OPERADOR_COMPOSTO,
    IDENTIFICADOR,
    LITERAL,
    PALAVRA_RESERVADA
} Tipos_Token;

/**
 * Struct que representa o valor lexico de um token
 */
typedef struct valor_lexico
{
    int linha_ocorrencia;      // Linha onde este token ocorreu
    Tipos_Token tipo;          // Tipo deste token
    union Valores_Token valor; // Valor deste token

} valor_lexico_t;

// Os tipos possiveis de nodo da arvore
typedef enum Tipos_Nodos
{
    FUNC_LIST,
    CMD_LIST,
    EXP_LIST,
    VAR_INIT,
    VAR_ATRIB,
    CMD_ES,
    FUNC_CALL,
    CMD_SHIFT,
    CMD_BREAK_CONTINUE,
    CMD_RETURN,
    CMD_IF,
    CMD_FOR,
    CMD_WHILE,
    UNOP,
    BINOP,
    TERNOP,
    VEC_IND,
    LIT_INT,
    LIT_FLOAT,
    LIT_STRING,
    LIT_CHAR,
    LIT_BOOL,
    ID,
    DELIM
} Tipos_Nodos;

/**
 * Nodo da arvore de sintaxe abstrata que sera gerada 
 */
typedef struct node
{
    valor_lexico_t *valor_lexico; // Informacoes deste nodo
    Tipos_Nodos tipo;             // Tipo deste nodo (Especificacao sobre if, for, atrib, etc)
    struct node *filhos;          // Ponteiro para o primeiro filho deste nodo
    struct node *irmao;           // Ponteiro para o proximo irmao deste nodo

    /**
     *  Ponteiro para o proximo comando apos este nodo
     * Tecnicamente e um filho, mas deve ser separado para distincao de blocos de comando
     *  Exemplo: If com else opcional
     * Se nao houvesse distincao, nao haveria como separar se o bloco de comandos aponta para
     * o proximo comando ou se aponta pra o bloco do else
     */
    struct node *prox_comando;

} node_t;

// Etapa 4

// Os tipos da linguagem
typedef enum Tipos_Linguagem
{
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_CHAR,
    TIPO_BOOL,
    TIPO_STRING,
    TIPO_NULL

} Tipos_Linguagem;

/**
 * Uma entrada na tabela hash 
 */
typedef struct entrada_tabela_simbolos
{
    char *chave; // Chave da hash table (Vai ser o nome da variavel/funcao)

    int linha_declaracao; // Linha onde o simbolo foi declarado TODO Porque se ja temos o valor_lexico?
    Tipos_Nodos natureza; // Natureza deste simbolo (literal, string, etc)
    Tipos_Linguagem tipo; // Tipo da linguagem ao qual este simbolo pertence
    int tamanho;          // Tamanho ocupado por este simbolo

    int arg_count;                            // Contagem de argumentos, caso for uma funcao, -1 caso contrario
    struct entrada_tabela_simbolos *args;     // Argumentos que este simbolo recebe (Se ele for uma funcao)
    struct entrada_tabela_simbolos *prox_arg; // Ponteiro para o proximo argumento da lista (para args de funcoes)

    valor_lexico_t *dados; // Dados do valor deste simbolo

    struct entrada_tabela_simbolos *prox; // Proxima entrada na tabela

} symbol_table_entry_t;

/**
 * Estrutura para a tabela de simbolos 
 */
typedef struct tabela_simbolos
{
    int tamanho;                 // Tamanho em memoria ocupado pela hash table
    int entradas;                // Numero de entradas na hash table
    symbol_table_entry_t *start; // Primeira entrada da tabela

} symbol_table_t;

#endif