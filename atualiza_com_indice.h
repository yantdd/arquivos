#ifndef ATUALIZA_COM_INDICE_H
#define ATUALIZA_COM_INDICE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "csv_to_bin.h"
#include "arvore_b.h"
#include "busca_multipla.h"
#include "atualiza_reg_bin.h"

/*
 * Executa atualizações usando índice árvore-B
 * Parâmetros:
 *  nome_arquivo_dados - nome do arquivo de dados
 *  nome_arquivo_indice - nome do arquivo de índice
 *  num_atualizacoes - número de atualizações a realizar
 * Retorno:
 *  true se bem-sucedido, false caso contrário
 */
bool executa_atualizacoes_com_indice(char *nome_arquivo_dados, char *nome_arquivo_indice, int num_atualizacoes);

/*
 * Atualiza registros SEMPRE usando o arquivo de índices.
 * Se idAttack está nos critérios, faz atualização indexada O(log n).
 * Caso contrário, percorre a árvore-B em ordem (in-order traversal) O(n).
 */
bool atualiza_com_busca_indice(FILE *arquivo_dados, FILE *arquivo_indice, REG_HEADER *header, BUSCA_MULTIPLA *busca, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao);

/*
 * Atualiza registros encontrados via in-order traversal da árvore-B (quando não há idAttack).
 * SEMPRE usa o arquivo de índices.
 */
bool atualiza_com_in_order(FILE *arquivo_dados, FILE *arquivo_indice, REG_HEADER *header, BUSCA_MULTIPLA *busca, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao);

#endif 