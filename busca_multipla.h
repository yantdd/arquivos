#ifndef BUSCA_MULTIPLA_H
#define BUSCA_MULTIPLA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "csv_to_bin.h"
#include "arvore_b.h"
#include "parse_csv.h"
#include "busca_bin.h"

/*
 * Estrutura para armazenar um critério de busca
 */
typedef struct {
    char *nome_campo;
    char *valor_campo;
} CRITERIO_BUSCA;

/*
 * Estrutura para armazenar uma busca completa
 */
typedef struct {
    int num_criterios;
    CRITERIO_BUSCA *criterios;
} BUSCA_MULTIPLA;

/*
 * Executa busca múltipla com critérios especificados
 */
bool executa_busca_multipla(char *nome_arquivo_dados, char *nome_arquivo_indice, int num_buscas);

/*
 * Verifica se um registro satisfaz todos os critérios de busca
 */
bool satisfaz_criterios(REG_DADOS *registro, BUSCA_MULTIPLA *busca);

/*
 * Libera memória alocada para uma busca múltipla
 */
void libera_busca_multipla(BUSCA_MULTIPLA *busca);

/*
 * Busca usando índice árvore-B quando idAttack está nos critérios
 */
bool busca_com_indice(FILE *arquivo_dados, FILE *arquivo_indice, BUSCA_MULTIPLA *busca);

/*
 * Busca sequencial quando idAttack não está nos critérios
 */
bool busca_sequencial(FILE *arquivo_dados, BUSCA_MULTIPLA *busca);

// Funções auxiliares
REG_DADOS* le_registro_completo(FILE *arquivo, long long int offset);

#endif 