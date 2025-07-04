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
 * Atualiza registros encontrados via busca com índice
 * Parâmetros:
 *  arquivo_dados - ponteiro para arquivo de dados
 *  arquivo_indice - ponteiro para arquivo de índice
 *  header - ponteiro para header do arquivo de dados
 *  busca - critérios de busca
 *  num_campos_atualizacao - número de campos a atualizar
 *  nomes_campos_atualizacao - nomes dos campos a atualizar
 *  valores_campos_atualizacao - valores dos campos a atualizar
 * Retorno:
 *  true se encontrou e atualizou registros, false caso contrário
 */
bool atualiza_com_busca_indice(FILE *arquivo_dados, FILE *arquivo_indice, REG_HEADER *header, 
                              BUSCA_MULTIPLA *busca, int num_campos_atualizacao, 
                              char **nomes_campos_atualizacao, char **valores_campos_atualizacao);

/*
 * Atualiza registros encontrados via busca sequencial
 * Parâmetros:
 *  arquivo_dados - ponteiro para arquivo de dados
 *  header - ponteiro para header do arquivo de dados
 *  busca - critérios de busca
 *  num_campos_atualizacao - número de campos a atualizar
 *  nomes_campos_atualizacao - nomes dos campos a atualizar
 *  valores_campos_atualizacao - valores dos campos a atualizar
 * Retorno:
 *  true se encontrou e atualizou registros, false caso contrário
 */
bool atualiza_com_busca_sequencial(FILE *arquivo_dados, REG_HEADER *header, 
                                  BUSCA_MULTIPLA *busca, int num_campos_atualizacao, 
                                  char **nomes_campos_atualizacao, char **valores_campos_atualizacao);

#endif 