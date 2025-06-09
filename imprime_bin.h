#ifndef IMPRIME_BIN_H
#define IMPRIME_BIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


REG_DADOS *get_registro(FILE *bin);
void imprime_registro_bin(REG_DADOS *reg_dados);
void imprime_bin(FILE *bin);


#endif // IMPRIME_BIN_H