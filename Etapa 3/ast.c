#include "ast.h"

extern void exporta(void* arvore)
{
    // So exporta arvores que existem
    if (arvore != NULL)
    {
        // Imprime a informacao dos nodos da arvore
        imprime_nos_arvore(arvore);

        // Imprime a informacao das arestas da arvore
        imprime_arestas_arvore(arvore);
    }
}

extern void libera(void* arvore)
{
    // Remove todos os nodos da arvore de forma recursiva
    remove_no(arvore);
}

extern node_t* insere_pai(void* arvore, valor_lexico_t* valor_lexico)
{
    // Cria um nodo novo e inicializa com os valores
    node_t* nodo = (node_t*)malloc(sizeof(node_t));
    nodo->valor_lexico = valor_lexico;
    nodo->filhos = arvore;
    nodo->irmao  = NULL;

    // Retorna este novo nodo
    return nodo;
}

extern node_t* insere_filho(void* arvore, valor_lexico_t* valor_lexico)
{
    // Cria um nodo novo e inicializa com os valores
    node_t* nodo = (node_t*)malloc(sizeof(node_t));
    nodo->valor_lexico = valor_lexico;
    nodo->filhos = NULL;
    nodo->irmao  = NULL;

    // Se a arvore ja existe, cria o nodo como filho
    if (arvore != NULL)
    {
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
    else
    {
        // Se a arvore nao existe, utiliza o nodo como raiz
        arvore = nodo;
    }
    
    // Retorna o nodo inserido
    return nodo;
}

extern node_t* remove_no(void* arvore)
{
    // Ponteiros auxiliares
    node_t* aux_1 = NULL;
    node_t* aux_2 = NULL;
    node_t* swap  = NULL;

    // So remove nodos que existem
    if (arvore != NULL)
    {
        // Se este nodo tem filhos
        if (((node_t*)arvore)->filhos != NULL)
        {
            // Inicializa o ponteiro auxiliar 1 no primeiro filho
            aux_1 = ((node_t*)arvore)->filhos;
            
            // Inicializa o ponteiro auxiliar 2 no segundo filho
            aux_2 = aux_1->irmao;

            // Percorre todos os filhos removendo de forma recursiva
            while (aux_2 != NULL || aux_1 != NULL)
            {
                // Remove o filho i
                if (aux_1 != NULL) remove_no(aux_1);

                // Atualiza o ponteiro para o filho i+1
                if (aux_2 != NULL) aux_1 = aux_2->irmao;
                else aux_1 = NULL;

                // Troca os ponteiros
                swap = aux_1;
                aux_1 = aux_2;
                aux_2 = swap;
            }
        }

        // Salva o primeiro irmao desta arvore
        aux_1 = ((node_t*)arvore)->irmao;

        // Remove a estrutura de valor lexico deste nodo
        free(((node_t*)arvore)->valor_lexico);

        // Remove o nodo informado
        free(arvore);

        printf("No removido \n");
    }

    // Retorna o primeiro irmao desta arvore, se existir
    return aux_1;
}

void imprime_nos_arvore(void* arvore)
{
    // So imprime arvores que existem
    if (arvore != NULL)
    {
        // Imprime o nodo atual
        imprime_no(arvore);
        
        // Imprime os filhos do nodo atual
        imprime_nos_arvore(((node_t*)arvore)->filhos);

        // Imprime os irmaos do nodo atual
        imprime_nos_arvore(((node_t*)arvore)->irmao);
    }
}

void imprime_arestas_arvore(void* arvore)
{
    // So imprime arestas de arvores que existem
    if (arvore != NULL)
    {
        // Imprime as arestas do nodo atual
        imprime_aresta(arvore);

        // Imprime as arestas dos filhos
        imprime_arestas_arvore(((node_t*)arvore)->filhos);
    
        // Imprime as arestas dos irmaos
        imprime_arestas_arvore(((node_t*)arvore)->irmao);
    }
}

void imprime_no(void* arvore)
{
    // So imprime nodos que existem
    if (arvore != NULL)
    {
        // Printa on inicio comum a todos os nodos
        printf("%p [label=\"", (node_t*)arvore);

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
                printf("%s\n", ((node_t*)arvore)->valor_lexico->valor.nome); // TODO Printar os valores especificados na definicao
                break;
        }
        
        // Printa o final comum a todos os nodos
        printf("\"];\n");
    }
}

void imprime_aresta(void* arvore)
{
    // So imprime arestas que existem
    if (arvore != NULL)
    {
        // Ponteiro auxiliar, aponta para o primeiro filhos
        node_t* aux = ((node_t*)arvore)->filhos;

        // Percorre os filhos do nodo
        while (aux != NULL)
        {
            // Printa o endereco do nodo pai e do filho
            printf("%p, %p", (node_t*)arvore, aux);

            // Passa para o proximo filho
            aux = aux->irmao;
        }

    }
}
