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

        // Libera a memoria usada para o valor lexico deste nodo (alocada em scanner.l)
        libera_valor_lexico(((node_t*)arvore)->valor_lexico, ((node_t*)arvore)->tipo);

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
    if ((node_t*)arvore != NULL)
    {
        // Printa on inicio comum a todos os nodos
        printf("%p [label=\"", arvore);

        switch (((node_t *)arvore)->valor_lexico->tipo)
        {
        // Literais
        case LITERAL:
            switch (((node_t *)arvore)->tipo)
            {
            case LIT_CHAR:
                printf("%c", ((node_t *)arvore)->valor_lexico->valor.caractere);
                break;
            case LIT_STRING:
                printf("%s", ((node_t *)arvore)->valor_lexico->valor.string);
                break;
            case LIT_INT:
                printf("%d", ((node_t *)arvore)->valor_lexico->valor.inteiro);
                break;
            case LIT_FLOAT:
                printf("%f", ((node_t *)arvore)->valor_lexico->valor.ponto_flutuante);
                break;
            case LIT_BOOL:
                printf("%d", ((node_t *)arvore)->valor_lexico->valor.booleano);
                break;
            default:
                break;
            }
            break;
        // Identificadores
        case IDENTIFICADOR:
            printf("%s", ((node_t *)arvore)->valor_lexico->valor.nome);
            break;
        // Operadores compostos
        case OPERADOR_COMPOSTO:
            printf("%s", ((node_t *)arvore)->valor_lexico->valor.nome);
            break;
        // Caracteres especiais
        case CARACTERE_ESPECIAL:
            switch (((node_t *)arvore)->tipo)
            {
            case FUNC_LIST:          // Para declaracao de funcao, usa o nome da mesma
            case UNOP:               // Para operadores unarios, usa o proprio operador
            case BINOP:              // Para operadores binarios, usa o proprio operador
            case CMD_SHIFT:          // Para comando de shift, usa o label << ou >>
            case CMD_BREAK_CONTINUE: // Para comando de break/continue, usa o label 'break' ou 'continue'
            case CMD_RETURN:         // Para comando de return, usa o label 'return'
            case CMD_FOR:            // Para comando de for, usa o label 'for'
            case CMD_IF:             // Para comando de if, usa o label 'if'
            case CMD_WHILE:          // Para comando de while, usa o label 'while'
            case VAR_INIT:           // Para inicializacao de variavel, usa o label <=
            case VAR_ATRIB:          // Para atribuicao de variavel, usa o label =
                printf("%s", ((node_t *)arvore)->valor_lexico->valor.nome);
                break;
            case VEC_IND: // Para indexacao do vetor, usa o label []
                printf("[]");
                break;
            case FUNC_CALL: // Para chamada de funcao, usa o label call + nome da funcao
                printf("call %s", ((node_t *)arvore)->valor_lexico->valor.nome);
                break;
            case TERNOP: // Para o operador ternario, usa o label '?:'
                printf("?:");
                break;
            default: // ERRO
                break;
            }
            break;
        case PALAVRA_RESERVADA:
            printf("%s", ((node_t *)arvore)->valor_lexico->valor.nome);
            break;
        // Erro
        default:
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
            printf("%p, %p\n", (node_t *)arvore, aux);

            // Passa para o proximo filho
            aux = aux->irmao;
        }

        // Printa o proximo comando do nodo, se ele existe
        if (((node_t *)arvore)->prox_comando != NULL)
            printf("%p, %p\n", (node_t *)arvore, ((node_t *)arvore)->prox_comando);
    }
}

node_t *insere_no_filho(node_t **nodo_pai, node_t *nodo_filho)
{
    // Verifica se o nodo pai existe:
    if (*nodo_pai != NULL)
    {
        // Inicializa o ponteiro auxiliar com o filho do nodo pai
        node_t *aux = (*nodo_pai)->filhos;

        // Se o pai tem filhos
        if (aux != NULL)
        {
            // Itera pelos nodos filhos ate chegar no ultimo
            while (aux->irmao != NULL)
                aux = aux->irmao;

            // Quando chega no ultimo, insere o novo filho
            aux->irmao = nodo_filho;
        }
        // Se o pai nao tem filhos
        else
        {
            // Novo nodo vira o filho
            (*nodo_pai)->filhos = nodo_filho;
        }

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

node_t *cria_nodo_lexico(valor_lexico_t *valor_lexico, Tipos_Nodos tipo_nodo)
{
    // Cria um nodo novo e inicializa com os valores informados
    node_t *nodo = (node_t *)malloc(sizeof(node_t));
    memset(nodo, 0, sizeof(node_t));
    nodo->valor_lexico = valor_lexico;
    nodo->tipo = tipo_nodo;
    nodo->filhos = NULL;
    nodo->irmao = NULL;

    // Retorna o ponteiro para este nodo
    return nodo;
}

node_t *cria_nodo_intermed(Tipos_Token tipo_token, Tipos_Nodos tipo_nodo, char *valor, int linha)
{
    // Aloca a estrutura para o valor lexico
    valor_lexico_t *valor_lexico = (valor_lexico_t *)malloc(sizeof(valor_lexico_t));
    memset(valor_lexico, 0, sizeof(valor_lexico_t));
    valor_lexico->linha_ocorrencia = linha;
    valor_lexico->valor.nome = strdup(valor);
    valor_lexico->tipo = tipo_token;

    // Cria um nodo com esta estrutura e retorna
    return cria_nodo_lexico(valor_lexico, tipo_nodo);
}

node_t *preenche_nodo(node_t **nodo_pai, node_t *filho_1, node_t *filho_2, node_t *filho_3, node_t *filho_4)
{
    // Se o nodo pai e o primeiro filho existirem
    if (*nodo_pai != NULL && filho_1)
    {
        // Insere o filho 1
        *nodo_pai = insere_no_filho(nodo_pai, filho_1);

        // Se o filho 2 existir
        if (filho_2 != NULL)
        {
            // Insere o filho 2
            *nodo_pai = insere_no_filho(nodo_pai, filho_2);

            // Se o filho 3 existir
            if (filho_3 != NULL)
            {
                // Insere o filho 3
                *nodo_pai = insere_no_filho(nodo_pai, filho_3);

                // Se o filho 4 existir
                if (filho_4 != NULL)
                {
                    // Insere o filho 4
                    *nodo_pai = insere_no_filho(nodo_pai, filho_4);
                }
            }
        }
        // Retorna o nodo pai
        return *nodo_pai;
    }

    return NULL;
}

void libera_valor_lexico(valor_lexico_t* valor_lexico, Tipos_Nodos tipo)
{
    // Libera a cadeia de caracteres criada por strdup no scanner.l, caso haja uma
    switch (valor_lexico->tipo)
    {
    // Caracteres especiais, operadores compostos e identificadores
    case CARACTERE_ESPECIAL:
    case OPERADOR_COMPOSTO:
    case IDENTIFICADOR:
    case PALAVRA_RESERVADA:
        // Libera o nome
        free(valor_lexico->valor.nome);
        break;
    case LITERAL:
        // Se for string, libera a string
        if (tipo == LIT_STRING)
            free(valor_lexico->valor.string);
    }

    // Libera a estrutura
    free(valor_lexico);

}