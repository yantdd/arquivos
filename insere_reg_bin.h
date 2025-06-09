#ifndef INSERE_REG_BIN_H
#define INSERE_REG_BIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "csv_to_bin.h"

void atualiza_dados_header_insercao(REG_HEADER *header, FILE* bin);
void atualiza_header_inserido(REG_HEADER *header, FILE* bin);
REG_DADOS *cria_registro_para_insercao(void);
void escreve_registro_no_bin(FILE *bin, REG_DADOS *reg);
void preenche_lixo(FILE *bin, int tamanho_lixo);
void match_insere_bin(FILE *bin, REG_HEADER *header, REG_DADOS *novo_registro);
void chama_match_insere_bin(char *nome_arquivo, int num_insercoes);
void scan_quote_string(char *str);

#endif // INSERE_REG_BIN_H 