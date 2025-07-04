#ifndef REMOVE_REG_BIN_H
#define REMOVE_REG_BIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "csv_to_bin.h"

void atualiza_header_removido(REG_HEADER *header, FILE* bin);
void atualiza_dados_header(REG_HEADER *header, FILE* bin);
void match_registro_remove(FILE *bin, REG_HEADER *header, int num_campos, char **nome_campo, char **valor_campo);
void chama_match_remove_bin(char *nome_arquivo, int num_buscas);
void scan_quote_string(char *str);

#endif // REMOVE_REG_BIN_H 