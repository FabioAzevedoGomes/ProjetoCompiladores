#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_types.h"

/**
 * Retorna o tamanho associado aquele tipo
 * @param tipo  Qual o tipo
 * @param count Quantos daquele tipo (para o caso de string/vetor)
 * @returns Tamanho do tipo dado a quantidade
 */
int tamanho_de(Tipos_Linguagem tipo, int count);

/**
 * Cria uma nova tabela de simbolos vazia 
 */
symbol_table_t *cria_tabela_simbolos();

/**
 * Cria um novo simbolo para ser inserido na tabela de simbolos
 * @param chave     Chave do simbolo
 * @param linha     Linha onde ocorre
 * @param natureza  Natureza do simbolo
 * @param tipo      Tipo do simbolo
 * @param quant     Quantidade daquele tipo (Para vetores e strings)
 * @param dados     Ponteiro para o valor lexico do simbolo
 * @returns Ponteiro para a estrutura criada
 */
symbol_table_entry_t *cria_simbolo(char *chave, int linha, Tipos_Nodos natureza,
                                   Tipos_Linguagem tipo, int quant, valor_lexico_t* dados);

/**
 * Insere um novo simbolo como argumento de um simbolo declarado como funcao
 * @param simbolo   Simbolo no qual se deseja inserir um argumento (deve ser funcao)
 * @param argumento Argumento que se deseja inserir
 */
void insere_argumento(symbol_table_entry_t *simbolo, symbol_table_entry_t *argumento);

/**
 * Insere uma nova entrada na tabela fornecida 
 * @param tabela Ponteiro para a tabela de simbolos
 * @param valor Informacoes sobre o simbolo a serem guardadas
 * @returns 0 se inseriu na tabela com sucesso, 1 se simbolo ja existia na tabela
 */
int insere_simbolo(symbol_table_t *tabela, symbol_table_entry_t *valor);

/**
 * Consulta pela chave na tabela e retorna seu valor
 * @param tabela Tabela na qual sera realizada a busca
 * @param chave Valor para o qual deve-se recuperar as informacoes
 * @returns Struct symbol_t com as informacoes se encontrou, NULL caso contrario
 */
symbol_table_entry_t *consulta_simbolo(symbol_table_t *tabela, char *chave);