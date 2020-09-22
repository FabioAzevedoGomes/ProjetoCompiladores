#include "ast.h"

extern void exporta(void *arvore)
{

}

extern void libera(void *arvore)
{

}

extern void insere_no(void *arvore, valor_lexico_t* valor_lexico)
{
    // So insere em nodos que existem
    if (arvore != NULL)
    {
        // Cria um nodo novo e inicializa com os valores
        node_t* nodo = (node_t*)malloc(sizeof(node_t));
        nodo->valor_lexico = valor_lexico;
        nodo->filhos = NULL;
        nodo->irmao  = NULL;

        // Insere nos filhos do nodo recebido
        if (((node_t*)arvore)->filhos != NULL)
        {
            // Ponteiro auxiliar
            node_t* aux = ((node_t*)arvore)->filhos;
            
            // Percorre filhos ate chegar no ultimo
            while (aux->irmao != NULL);
                aux = aux->irmao;

            // Insere na arvore
            aux->irmao = nodo;
        }
        else
        {
            // Se nao possui filhos, insere novo nodo apenas
            ((node_t*)arvore)->filhos = nodo;
        }
    }
}

extern void remove_no(void *arvore)
{

}

extern void imprime_no(void *arvore)
{
    // So imprime nodos que existem
    if (arvore != NULL)
    {
        printf("===== Nodo =====\n");
        printf("Linha: %d\n",((node_t*)arvore)->valor_lexico->linha_ocorrencia);
        printf("Tipo:  %d\n",((node_t*)arvore)->valor_lexico->tipo);
        switch(((node_t*)arvore)->valor_lexico->tipo)
        {
            // Literais
            case TK_LIT_CHAR:
                printf("Valor: %c\n",*((node_t*)arvore)->valor_lexico->valor.caractere);
                break;
            case TK_LIT_STRING:
                printf("Valor: %s\n",((node_t*)arvore)->valor_lexico->valor.string);
                break;
            case TK_LIT_INT:
                printf("Valor: %d\n",((node_t*)arvore)->valor_lexico->valor.inteiro);
                break;
            case TK_LIT_FLOAT:
                printf("Valor: %f\n",((node_t*)arvore)->valor_lexico->valor.ponto_flutuante);
                break;
            case TK_LIT_TRUE:
                printf("Valor: %d\n",((node_t*)arvore)->valor_lexico->valor.booleano);
                break;
            case TK_LIT_FALSE:
                printf("Valor: %d\n",((node_t*)arvore)->valor_lexico->valor.booleano);
                break;
            // Identificadores
            case TK_IDENTIFICADOR:
            // Operadores compostos
            case TK_OC_LE:
            case TK_OC_GE:
            case TK_OC_EQ:
            case TK_OC_NE:
            case TK_OC_AND:
            case TK_OC_OR:
            case TK_OC_SR:
            case TK_OC_SL:
            // Caracteres especiais
            default:
                printf("Valor: %s\n", ((node_t*)arvore)->valor_lexico->valor.nome);
                break;
        }
    }
}
