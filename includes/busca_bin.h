#ifndef BUSCA_BIN_H
#define BUSCA_BIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "imprime_bin.h"
#include "csv_to_bin.h"




void match_registro(FILE *bin, int num_campos, char **nome_campo, char **valor_campo);
void chama_match_bin(char *nome_arquivo, int num_buscas);
void scan_quote_string(char *str);


#endif // BUSCA _BIN_H