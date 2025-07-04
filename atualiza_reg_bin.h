#ifndef ATUALIZA_REG_BIN_H
#define ATUALIZA_REG_BIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "csv_to_bin.h"

void atualiza_dados_header_atualizacao(REG_HEADER *header, FILE* bin);
void atualiza_header_atualizacao_final(REG_HEADER *header, FILE* bin);
void atualiza_dados_header(REG_HEADER *header, FILE* bin);
void match_registro_remove(FILE *bin, REG_HEADER *header, int num_campos, char **nome_campo, char **valor_campo);
bool verifica_criterio_busca(REG_DADOS *reg_dados, int num_campos_busca, char **nome_campo_busca, char **valor_campo_busca);
REG_DADOS *aplica_atualizacao(REG_DADOS *reg_original, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao);
int calcula_tamanho_registro_atualizado(REG_DADOS *reg);
void escreve_registro_atualizado_no_local(FILE *bin, REG_DADOS *reg_atualizado, int tamanho_original);
void remove_registro_logicamente(FILE *bin, REG_HEADER *header, long long posicao_registro);
void match_registro_atualiza(FILE *bin, REG_HEADER *header, int num_campos_busca, char **nome_campo_busca, char **valor_campo_busca, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao);
void chama_match_atualiza_bin(char *nome_arquivo, int num_atualizacoes);
void scan_quote_string(char *str);
void insere_sem_atualizar_header(FILE *bin, REG_HEADER *header, REG_DADOS *novo_registro);
int calcula_tamanho_fisico_registro(FILE *bin, long long posicao_registro);

#endif // ATUALIZA_REG_BIN_H 