#ifndef IMPRIME_BIN_H
#define IMPRIME_BIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct reg_dados REG_DADOS;
// Funções chamadas diretamente pela main
void imprime_bin(FILE *bin);
REG_DADOS *get_registro(FILE *bin);
void imprime_registro_bin(REG_DADOS *reg_dados);
void chama_match_bin(char *nome_arquivo, int num_buscas);

#endif