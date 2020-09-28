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
typedef enum Tipos_Nodo
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
    ID
} Tipos_Nodos;

/**
 * Nodo da arvore de sintaxe abstrata que sera gerada 
 */
typedef struct node
{
    valor_lexico_t *valor_lexico; // Informacoes deste nodo
    Tipos_Nodos tipo;              // Tipo deste nodo (Especificacao sobre if, for, atrib, etc)
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
