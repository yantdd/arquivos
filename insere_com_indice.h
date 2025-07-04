#ifndef INSERE_COM_INDICE_H
#define INSERE_COM_INDICE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "csv_to_bin.h"
#include "arvore_b.h"
#include "busca_bin.h"

/*
 * Estrutura para armazenar dados de um registro a ser inserido
 */
typedef struct {
    int idAttack;
    int year;
    float financialLoss;
    char *country;
    char *attackType;
    char *targetIndustry;
    char *defenseMechanism;
} REGISTRO_INSERCAO;

/*
 * Executa inserções de registros com reaproveitamento de espaços e atualização do índice
 */
bool executa_insercoes_com_indice(const char* nome_arquivo_dados, const char* nome_arquivo_indice, int num_insercoes);

/*
 * Lê os dados de um registro a ser inserido da entrada
 */
REGISTRO_INSERCAO* le_registro_insercao();

/*
 * Calcula o tamanho necessário para um registro
 */
int calcula_tamanho_registro_insercao(REGISTRO_INSERCAO *reg);

/*
 * Procura um espaço removido que caiba o registro (First Fit)
 */
long long int procura_espaco_first_fit(FILE *arquivo, REG_HEADER *header, int tamanho_necessario);

/*
 * Escreve um registro no formato binário
 */
bool escreve_registro_binario(FILE *arquivo, REGISTRO_INSERCAO *reg, long long int posicao, int tamanho_disponivel);

#endif 