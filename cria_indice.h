#ifndef CRIA_INDICE_H
#define CRIA_INDICE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "csv_to_bin.h"
#include "arvore_b.h"

// Função principal da funcionalidade 7
bool cria_indice_arvore_b(char *nome_arquivo_dados, char *nome_arquivo_indice);

// Função auxiliar para ler um registro de dados
REG_DADOS* le_registro_dados(FILE *arquivo_dados, long long int offset);

// Função para verificar se um arquivo existe e está acessível
bool arquivo_existe(char *nome_arquivo);

#endif