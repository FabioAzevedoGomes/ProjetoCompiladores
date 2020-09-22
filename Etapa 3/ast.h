#include <stdio.h>
#include <stdlib.h>

#include "data_types.h"
#include "parser.tab.h"

/**
 * Exporta a arvore de sintaxe abstrata (AST) para um arquivo CSV e a printa
 * na saida padrao a arvore seguindo uma busca em profundidade
 * Formato de impressao:
 * Nodos  (0 ... n), um por linha
 * Arestas(0 ... m), uma por linha
 * @param arvore Ponteiro para a raiz da arvore de sintaxe abstrata que sera exportada
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
 * Insere um nodo filho no nodo apontado por nodo_pai
 * @param nodo_pai Ponteiro para um ponteiro para o nodo pai
 * @param nodo_filho Ponteiro para o nodo filho
 * @returns Ponteiro para o nodo pai
 */
extern node_t *insere_no_filho(node_t **nodo_pai, node_t *nodo_filho);

/**
 * Insere um nodo como proximo comando do nodo informado
 * @param nodo_primeiro Ponteiro para um ponteiro para o nodo primario
 * @param nodo_segundo Ponteiro para o nodo que sera inserido como subsequente do primeiro
 * @returns Ponteiro para o nodo primario
 */
extern node_t *insere_no_comando(node_t **nodo_primeiro, node_t *nodo_segundo);

/**
 * Remove o nodo na posicao indicada da arvore de sintaxe abstrata fornecida
 * Caso este nodo seja uma folha, simplesmente remove o nodo e libera a memoria
 * Se este nodo for intermediario, remove todos os seus filhos de forma recursiva, tambem liberando a memoria
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata a ser removido
 * @returns Ponteiro para o primeiro irmao deste nodo, ou NULL se nao tiver irmao
 */
extern void remove_no(void *arvore);

/**
 * Imprime todos os nodos da arvore informada de acordo com a busca em profundidade
 * @param arvore Ponteiro para a arvore cujos nodos serao impressos
 */
void imprime_nos_arvore(void *arvore);

/**
 * Imprime todas as arestas da arvore informada de acordo com uma busca em profundidade
 * dos nodos
 * @param arvore Ponteiro para a arvore cujas arestas serao impressas
 */
void imprime_arestas_arvore(void *arvore);

/**
 * Imprime as informacoes do nodo na posicao apontada 
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata cujas informacoes devem ser impressas
 */
void imprime_no(void *arvore);

/**
 * Imprime todas as arestas do nodo informado
 * Segue o formato na definicao:
 * <endereco_no>, <endereco_filho>
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata cujas arestas devem sem impressas
 */
void imprime_aresta(void *arvore);

/**
 * Aloca memoria para uma estrutura do tipo nodo, e inicializa com o valor lexico informado
 * @param valor_lexico Ponteiro para a struct de valor lexico do token
 */
node_t *cria_nodo(valor_lexico_t *valor_lexico);
