#include <stdio.h>
#include <stdlib.h>

#include "data_types.h"
#include "parser.tab.h"

/**
 * Nodo da arvore de sintaxe abstrata que sera gerada 
 */
typedef struct node
{
    valor_lexico_t*     valor_lexico; // Informacoes deste nodo
    struct node*        filhos;       // Ponteiro para o primeiro filho deste nodo
    struct node*        irmao;        // Ponteiro para o proximo irmao deste nodo (next)

}node_t;

/* 
 * Exporta a arvore de sintaxe abstrata (AST) para um arquivo CSV e a printa
 * na saida padrao de acordo com o modelo especificado na secao 2.4
 * da especificacao
 * @param arvore Ponteiro para a raiz da arvore de sintaxe abstrata que sera exportada
 * 
 */
extern void exporta(void *arvore);

/**
 * Libera a memoria utilizada pela arvore de sintaxe abstrata fornecida, utilizando
 * uma estrategia bottom-up para que nao haja vazamentos
 * @param arvore Ponteiro para a raiz da arvore de sintaxe abstrata cuja memoria sera liberada
 *
 */
extern void libera(void *arvore);

/**
 * Insere o nodo na posicao indicada da arvore de sintaxe abstrata fornecida
 * @param arvore Ponteiro para a posicao na arvore de sintaxe abstrata onde este novo nodo deve ser inserido
 * @param TODO Atributos do nodo
 */
extern void insere_no(void *arvore, valor_lexico_t* valor_lexico);

/**
 * Remove o nodo na posicao indicada da arvore de sintaxe abstrata fornecida
 * Caso este nodo seja uma folha, simplesmente remove o nodo e libera a memoria
 * Se este nodo for intermediario, remove todos os seus filhos de forma recursiva, tambem liberando a memoria
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata a ser removido
 */
extern void remove_no(void *arvore);

/**
 * Imprime as informacoes do nodo na posicao apontada 
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata cujas informacoes devem ser impressas
 */
extern void imprime_no(void *arvore);

