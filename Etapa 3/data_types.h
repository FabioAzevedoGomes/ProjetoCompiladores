/**
 * Union com os diferentes tipos de valores para os tokens correspondentes 
 */
union Valores_Token
{
    char* nome;             // Valor de tokens do tipo CARACTERE_ESPECIAL, OPERADOR e IDENTIFICADOR

    int   inteiro;          // Valor de tokens do tipo TK_LIT_INT
    float ponto_flutuante;  // Valor de tokens do tipo TK_LIT_FLOAT
    char* string;           // Valor de tokens do tipo TK_LIT_STRING
    char  caractere;        // Valor de tokens do tipo TK_LIT_CHAR
    int   booleano;         // Valor de tokens do tipo TK_LIT_BOOL
};

/**
 * Struct que representa o valor lexico de um token
 */
typedef struct valor_lexico
{
    int                 linha_ocorrencia;   // Linha onde este token ocorreu
    int                 tipo;               // Tipo deste token
    union Valores_Token valor;              // Valor deste token

}valor_lexico_t;

/**
 * Nodo da arvore de sintaxe abstrata que sera gerada 
 */
typedef struct node
{
    valor_lexico_t*     valor_lexico; // Informacoes deste nodo
    struct node*        filhos;       // Ponteiro para o primeiro filho deste nodo
    struct node*        irmao;        // Ponteiro para o proximo irmao deste nodo
    struct node*        prox_comando; // Ponteiro para o proximo comando apos este nodo
}node_t;