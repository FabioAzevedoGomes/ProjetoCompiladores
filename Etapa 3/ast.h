#include <stdio.h>
#include <stdlib.h>

#include "data_types.h"
#include "parser.tab.h"

/* 
 * Exporta a arvore de sintaxe abstrata (AST) para um arquivo CSV e a printa
 * na saida padrao a arvore seguindo uma busca em profundidade
 * Formato de impressao:
 * TODO
 * @param arvore Ponteiro para a raiz da arvore de sintaxe abstrata que sera exportada
 * 
 */
extern void exporta(void* arvore);

/**
 * Libera a memoria utilizada pela arvore de sintaxe abstrata fornecida, utilizando
 * uma estrategia bottom-up para que nao haja vazamentos
 * @param arvore Ponteiro para a raiz da arvore de sintaxe abstrata cuja memoria sera liberada
 *
 */
extern void libera(void* arvore);

/**
 * Insere um nodo como pai do nodo apontado por arvore
 * @param arvore Ponteiro para o nodo que recebera um pai
 * @param valor_lexico Valor lexico do nodo pai
 * @returns Ponteiro para o nodo pai
 */
extern void insere_pai(void** arvore, valor_lexico_t* valor_lexico);

/**
 * Insere o nodo na posicao indicada da arvore de sintaxe abstrata fornecida
 * Se o ponteiro arvore e vazio, o novo nodo se torna a raiz
 * Caso contrario, o novo nodo e inserido como filho do nodo apontado
 * @param arvore Ponteiro para a posicao na arvore de sintaxe abstrata onde este novo nodo deve ser inserido
 * @param valor_lexico Ponteiro para a struct de valor lexico do nodo
 * @returns Ponteiro para o nodo que acaba de ser inserido
 */
extern void insere_filho(void** arvore, valor_lexico_t* valor_lexico);

/**
 * Remove o nodo na posicao indicada da arvore de sintaxe abstrata fornecida
 * Caso este nodo seja uma folha, simplesmente remove o nodo e libera a memoria
 * Se este nodo for intermediario, remove todos os seus filhos de forma recursiva, tambem liberando a memoria
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata a ser removido
 * @returns Ponteiro para o primeiro irmao deste nodo, ou NULL se nao tiver irmao
 */
extern void remove_no(void* arvore);

/**
 * Imprime todos os nodos da arvore informada de acordo com a busca em profundidade
 * @param arvore Ponteiro para a arvore cujos nodos serao impressos
 */
void imprime_nos_arvore(void* arvore);

/**
 * Imprime todas as arestas da arvore informada de acordo com uma busca em profundidade
 * dos nodos
 * @param arvore Ponteiro para a arvore cujas arestas serao impressas
 */
void imprime_arestas_arvore(void* arvore);

/**
 * Imprime as informacoes do nodo na posicao apontada 
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata cujas informacoes devem ser impressas
 */
void imprime_no(void* arvore);

/**
 * Imprime todas as arestas do nodo informado
 * Segue o formato na definicao:
 * <endereco_no>, <endereco_filho>
 * @param arvore Ponteiro para o nodo da arvore de sintaxe abstrata cujas arestas devem sem impressas
 */
void imprime_aresta(void* arvore);
