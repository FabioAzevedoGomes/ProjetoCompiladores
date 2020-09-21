
// Tipos validos para tokens obtidos pelo scanner
enum tipos_token{CARACTERE_ESPECIAL, OPERADOR, IDENTIFICADOR, LITERAL};

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
    enum tipos_token    tipo;               // Tipo deste token
    union Valores_Token valor;              // Valor deste token

}valor_lexico_t;
