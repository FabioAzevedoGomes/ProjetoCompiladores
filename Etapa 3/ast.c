#include "ast.h"

extern void exporta(void *arvore)
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

extern void libera(void *arvore)
{
    // Remove todos os nodos da arvore de forma recursiva
    remove_no(arvore);
}

extern node_t *insere_no_filho(node_t **nodo_pai, node_t *nodo_filho)
{
    // Verifica se o nodo pai existe:
    if (*nodo_pai != NULL)
    {
        // Inicializa o ponteiro auxiliar com o filho do nodo pai
        node_t *aux = (*nodo_pai)->filhos;

        // Itera pelos nodos filhos ate chegar no ultimo
        while (aux->irmao != NULL)
            aux = aux->irmao;

        // Quando chega no ultimo, insere o novo filho
        aux->irmao = nodo_filho;

        // Retorna o nodo pai (Redundante)
        return *(nodo_pai);
    }
    else
    {
        // Se o nodo pai nao existe, o filho e o novo pai
        return nodo_filho;
    }
}

extern node_t *insere_no_comando(node_t **nodo_primeiro, node_t *nodo_segundo)
{
    // Verifica se o nodo primario existe:
    if (*nodo_primeiro != NULL)
    {
        // Inicializa o ponteiro auxiliar com o filho do nodo pai
        (*nodo_primeiro)->prox_comando = nodo_segundo;

        // Retorna o nodo primario
        return *nodo_primeiro;
    }
    else
    {
        // Se o nodo primario nao existe, o filho e o novo pai
        return nodo_segundo;
    }
}

extern void remove_no(void *arvore)
{
    // Ponteiros auxiliares
    node_t *aux_1 = NULL;
    node_t *aux_2 = NULL;
    node_t *swap = NULL;

    // So remove nodos que existem
    if (arvore != NULL)
    {
        // Se este nodo tem filhos
        if (((node_t *)arvore)->filhos != NULL)
        {
            // Inicializa o ponteiro auxiliar 1 no primeiro filho
            aux_1 = ((node_t *)arvore)->filhos;

            // Inicializa o ponteiro auxiliar 2 no segundo filho
            aux_2 = aux_1->irmao;

            // Percorre todos os filhos removendo de forma recursiva
            while (aux_2 != NULL || aux_1 != NULL)
            {
                // Remove o filho i
                if (aux_1 != NULL)
                    remove_no(aux_1);

                // Atualiza o ponteiro para o filho i+1
                if (aux_2 != NULL)
                    aux_1 = aux_2->irmao;
                else
                    aux_1 = NULL;

                // Troca os ponteiros
                swap = aux_1;
                aux_1 = aux_2;
                aux_2 = swap;
            }
        }

        // Se este nodo tem comandos subsequentes, tambem remove
        if (((node_t *)arvore)->prox_comando != NULL)
            remove_no(((node_t *)arvore)->prox_comando);

        // Salva o primeiro irmao desta arvore
        aux_1 = ((node_t *)arvore)->irmao;

        // Libera a cadeia de caracteres criada por strdup no scanner.l, caso haja uma
        switch (((node_t *)arvore)->valor_lexico->tipo)
        {
            // Literais string
            case TK_LIT_STRING:
                free(((node_t *)arvore)->valor_lexico->valor.string);
                break;
            case TK_IDENTIFICADOR:
            case TK_OC_LE:
            case TK_OC_GE:
            case TK_OC_EQ:
            case TK_OC_NE:
            case TK_OC_AND:
            case TK_OC_OR:
            case TK_OC_SR:
            case TK_OC_SL:
                free(((node_t *)arvore)->valor_lexico->valor.nome);
                break;
            default: // Nos demais nao faz nada
                break;
        }

        // Remove a estrutura de valor lexico deste nodo
        free(((node_t *)arvore)->valor_lexico);

        // Remove o nodo informado
        free(arvore);
    }
}

void imprime_nos_arvore(void *arvore)
{
    // So imprime arvores que existem
    if (arvore != NULL)
    {
        // Imprime o nodo atual
        imprime_no(arvore);

        // Imprime os filhos do nodo atual
        imprime_nos_arvore(((node_t *)arvore)->filhos);

        // Imprime os irmaos do nodo atual
        imprime_nos_arvore(((node_t *)arvore)->irmao);

        // Imprime o comando subsequente do nodo atual
        imprime_nos_arvore(((node_t *)arvore)->prox_comando);
    }
}

void imprime_arestas_arvore(void *arvore)
{
    // So imprime arestas de arvores que existem
    if (arvore != NULL)
    {
        // Imprime as arestas do nodo atual
        imprime_aresta(arvore);

        // Imprime as arestas dos filhos
        imprime_arestas_arvore(((node_t *)arvore)->filhos);

        // Imprime as arestas dos irmaos
        imprime_arestas_arvore(((node_t *)arvore)->irmao);

        // Imprime as arestas dos comandos seguintes
        imprime_arestas_arvore(((node_t *)arvore)->prox_comando);
    }
}

void imprime_no(void *arvore)
{
    // So imprime nodos que existem
    if (arvore != NULL)
    {
        // Printa on inicio comum a todos os nodos
        printf("%p [label=\"", arvore);

        switch (((node_t *)arvore)->valor_lexico->tipo)
        {
        // Literais
        case TK_LIT_CHAR:
            printf("%c", ((node_t *)arvore)->valor_lexico->valor.caractere);
            break;
        case TK_LIT_STRING:
            printf("%s", ((node_t *)arvore)->valor_lexico->valor.string);
            break;
        case TK_LIT_INT:
            printf("%d", ((node_t *)arvore)->valor_lexico->valor.inteiro);
            break;
        case TK_LIT_FLOAT:
            printf("%f", ((node_t *)arvore)->valor_lexico->valor.ponto_flutuante);
            break;
        case TK_LIT_TRUE:
            printf("%d", ((node_t *)arvore)->valor_lexico->valor.booleano);
            break;
        case TK_LIT_FALSE:
            printf("%d", ((node_t *)arvore)->valor_lexico->valor.booleano);
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
            printf("%s", ((node_t *)arvore)->valor_lexico->valor.nome); // TODO Printar os valores especificados na definicao
            break;
        }

        // Printa o final comum a todos os nodos
        printf("\"];\n");
    }
}

void imprime_aresta(void *arvore)
{
    // So imprime arestas que existem
    if (arvore != NULL)
    {
        // Ponteiro auxiliar, aponta para o primeiro filhos
        node_t *aux = ((node_t *)arvore)->filhos;

        // Percorre os filhos do nodo
        while (aux != NULL)
        {
            // Printa o endereco do nodo pai e do filho
            printf("%p, %p", (node_t *)arvore, aux);

            // Passa para o proximo filho
            aux = aux->irmao;
        }

        // Printa o proximo comando do nodo, se ele existe
        if (((node_t *)arvore)->prox_comando != NULL)
            printf("%p, %p", (node_t *)arvore, ((node_t *)arvore)->prox_comando);
    }
}

node_t *cria_nodo(valor_lexico_t *valor_lexico)
{
    // Cria um nodo novo e inicializa com os valores informados
    node_t *nodo = (node_t *)malloc(sizeof(node_t));
    nodo->valor_lexico = valor_lexico;
    nodo->filhos = NULL;
    nodo->irmao = NULL;

    // Retorna o ponteiro para este nodo
    return nodo;
}