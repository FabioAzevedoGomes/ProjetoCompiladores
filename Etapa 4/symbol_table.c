#include "symbol_table.h"

int tamanho_de(Tipos_Linguagem tipo, int count)
{
    int tamanho = -1;

    switch (tipo)
    {
    case TIPO_INT:
        tamanho = 4 * count;
        break;
    case TIPO_FLOAT:
        tamanho = 8 * count;
        break;
    case TIPO_CHAR:
        tamanho = 1 * count;
        break;
    case TIPO_BOOL:
        tamanho = 1 * count;
        break;
    case TIPO_STRING:
        tamanho = 1 * count;
        break;
    case TIPO_NULL:
        tamanho = 0;
        break;
    }

    return tamanho;
}

symbol_table_t *cria_tabela_simbolos()
{
    // Tabela sendo criada
    symbol_table_t *tabela = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    tabela->entradas = 0;
    tabela->start = NULL;

    tabela->bottom = NULL;

    return tabela;
}

symbol_table_entry_t *cria_simbolo(char *chave, int linha, Tipos_Nodos natureza,
                                   Tipos_Linguagem tipo, int quant, valor_lexico_t *dados)
{

    // Aloca memoria
    symbol_table_entry_t *simbolo = (symbol_table_entry_t *)malloc(sizeof(symbol_table_entry_t));

    // Inicializa os atributos comuns
    simbolo->chave = chave;
    simbolo->linha_declaracao = linha;
    simbolo->natureza = natureza;
    simbolo->tipo = tipo;
    simbolo->arg_count = (natureza == FUNC_LIST ? 0 : -1);
    simbolo->dados = dados;

    // Calcula o tamanho ocupado
    simbolo->tamanho = tamanho_de(tipo, quant);

    // Inicializa ponteiros com NULL
    simbolo->args = NULL;
    simbolo->prox_arg = NULL;
    simbolo->prox = NULL;

    // Retorna o simbolo criado
    return simbolo;
}

void insere_argumento(symbol_table_entry_t *simbolo, symbol_table_entry_t *argumento)
{
    symbol_table_entry_t *entry; // Entrada da tabela de simbolos

    // So insere em simbolos que existem
    if (simbolo != NULL && argumento != NULL)
    {
        // Incrementa a contagem de argumentos
        simbolo->arg_count++;

        // Obtem o primeiro argumento
        entry = simbolo->args;

        if (entry != NULL)
        {
            // Percorre os argumentos ate chegar no ultimo
            while (entry->prox != NULL)
            {
                entry = entry->prox;
            }

            // Insere o argumento
            entry->prox = argumento;
        }
        else
        {
            // Se nao possui argumentos ainda, adiciona somente
            simbolo->args = argumento;
        }
    }
}

int insere_simbolo(symbol_table_t *tabela, symbol_table_entry_t *valor)
{
    symbol_table_entry_t *entry; // Entrada da tabela de simbolos

    // So insere em tabelas que existem
    if (tabela != NULL)
    {
        tabela->entradas++;

        // Se ja ha simbolos na tabela
        if (tabela->entradas > 0)
        {
            // Obtem a primeira entrada
            entry = tabela->start;

            // Para cada entrada
            while (entry->prox != NULL)
            {
                // Se o simbolo ja existe
                if (!strcmp(valor->chave, entry->chave))
                {
                    // Retorna fracasso
                    return 1;
                }
                else
                {
                    // Senao, segue em frente
                    entry = entry->prox;
                }
            }

            // Ao chegar no final, insere
            entry->prox = valor;
        }
        else
        {
            // Se nao ha simbolos na tabela, apenas insere
            tabela->start = valor;
        }
    }

    // Retorna sucesso
    return 0;
}

symbol_table_entry_t *consulta_simbolo(symbol_table_t *tabela, char *chave)
{
    symbol_table_entry_t *entry; // Entrada da tabela de simbolos

    // So consulta tabelas que existem e estao preenchidas
    if (tabela != NULL && tabela->entradas > 0)
    {
        // Obtem a primeira entrada
        entry = tabela->start;

        // Para cada entrada
        while (entry != NULL)
        {
            // Se a chave bate
            if (!strcmp(entry->chave, chave))
            {
                // Retorna a entrada
                return entry;
            }
            else
            {
                // Senao, segue em frente
                entry = entry->prox;
            }
        }
    }

    // Se tabela e null ou nao encontrou a entrada, retorna null
    return NULL;
}

symbol_table_stack_t *cria_pilha()
{
    // Aloca a memoria para a pilha
    symbol_table_stack_t *pilha = (symbol_table_stack_t *)malloc(sizeof(symbol_table_stack_t));

    // Inicializa a pilha como vazia
    pilha->entradas = 0;
    pilha->top = NULL;

    // Retorna referencia para a pilha
    return pilha;
}

void push_st(symbol_table_stack_t *pilha, symbol_table_t *tabela)
{
    // So pusha em pilhas que existem
    if (pilha != NULL && tabela != NULL)
    {
        // Atualiza a tabela de baixo da recebida
        tabela->bottom = pilha->top;

        // Atualiza a tabela do topo
        pilha->top = tabela;

        // Incrementa as entradas
        pilha->entradas++;
    }
}

symbol_table_t *pop_st(symbol_table_stack_t *pilha)
{
    symbol_table_t * tabela = NULL; // Tabela que sera poppada

    // So poppa de pilhas que existem e com tabelas
    if (pilha != NULL && pilha->top != NULL)
    {
        // Pega a tabela do topo da pilha
        tabela = pilha->top;

        // Atualiza o topo da pilha
        pilha->top = tabela->bottom;

        // Decrementa a contagem de entradas
        pilha->entradas--;
    }

    return tabela;
}