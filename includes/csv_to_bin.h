#ifndef CSV_TO_BIN_H
#define CSV_TO_BIN_H
#include "parse_csv.h"


// Outras funções acessíveis externamente
bool escreve_registros_no_bin(FILE *csv, FILE *bin);
void binarioNaTela(char *nomeArquivoBinario);

#endif