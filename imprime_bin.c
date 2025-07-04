/*******************************************************************************
 *                                                                             *
 *                     SCC0215 - Organização de Arquivos                       *
 *                                                                             *
 *                             Trabalho Prático                                *
 *                                                                             *
 * Professora: Cristina Dutra de Aguiar                                        *
 *                                                                             *
 * Aluno: Yan Trindade Meireles - 13680035                                     *
 *                                                                             *
 * Aluno: Rafael Perez Carmanhani - 15485420                                   *
 *                                                                             *
 *******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "parse_csv.h"
#include "csv_to_bin.h"

#define LIXO_STRING "$"
#define LIXO_INT -1
#define LIXO_FLOAT -1.0
#define DELIM '|'
#define NADA_CONSTA "NADA CONSTA"

/*
 * Busca um registro no binário e carrega seus dados na struct REG_DADOS.
 * Parâmetro:
 *  bin - ponteiro para o arquivo binário
 * Retorno:
 *  ponteiro para o registro do binário ou NULL em caso de erro.
 */
REG_DADOS *get_registro(FILE *bin)
{
    REG_DADOS *reg_dados = (REG_DADOS *)calloc(1, sizeof(REG_DADOS));
    if (reg_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    // Inicializa ponteiros para strings como NULL
    reg_dados->country = NULL;
    reg_dados->attackType = NULL;
    reg_dados->targetIndustry = NULL;
    reg_dados->defenseMechanism = NULL;

    if (fread(&reg_dados->removido, sizeof(char), 1, bin) != 1) {
        free(reg_dados);
        return NULL; // End of file
    }

    // Lê campos fixos do registro
    fread(&reg_dados->tamanhoRegistro, sizeof(int), 1, bin);
    fread(&reg_dados->prox, sizeof(long long int), 1, bin);
    fread(&reg_dados->idAttack, sizeof(int), 1, bin);
    fread(&reg_dados->year, sizeof(int), 1, bin);
    fread(&reg_dados->financialLoss, sizeof(float), 1, bin);

    // Subtraímos o tamanho dos campos fixos do tamanho total do registro
    int tamanho_reg = reg_dados->tamanhoRegistro - sizeof(long long int) - 2 * sizeof(int) - sizeof(float);

    // Processa campos variáveis
    while (tamanho_reg > 0) {
        int c = fgetc(bin);
        tamanho_reg--;

        switch (c) {
        // country
        case '1':
        {
            reg_dados->country = (char *)malloc(50);
            fscanf(bin, "%[^|]", reg_dados->country); // Lê até o delimitador '|'
            fgetc(bin); // Consome o delimmitador
            tamanho_reg -= (strlen(reg_dados->country) + 1); // Decrementa o tamanho do registro restante a ser lido
        }
        break;
        // attackType
        case '2':
        {
            reg_dados->attackType = (char *)malloc(50);
            fscanf(bin, "%[^|]", reg_dados->attackType);
            fgetc(bin); 
            tamanho_reg -= (strlen(reg_dados->attackType) + 1);
        }
        break;
        // targetIndustry
        case '3':
        {
            reg_dados->targetIndustry = (char *)malloc(50);
            fscanf(bin, "%[^|]", reg_dados->targetIndustry);
            fgetc(bin);
            tamanho_reg -= (strlen(reg_dados->targetIndustry) + 1);
        }
        break;
        // defenseMechanism
        case '4':
        {
            reg_dados->defenseMechanism = (char *)malloc(50);
            fscanf(bin, "%[^|]", reg_dados->defenseMechanism);
            fgetc(bin);
            tamanho_reg -= (strlen(reg_dados->defenseMechanism) + 1);
        }
        break;
        }
    }

    // Preenche na struct campos não encontrados com "NADA CONSTA" para a futura impressão
    if (reg_dados->country == NULL) {
        reg_dados->country = (char *)malloc(strlen(NADA_CONSTA));
        if (reg_dados->country != NULL) {
            strcpy(reg_dados->country, NADA_CONSTA);
        }
    }
    if (reg_dados->attackType == NULL) {
        reg_dados->attackType = (char *)malloc(strlen(NADA_CONSTA));
        if (reg_dados->attackType != NULL) {
            strcpy(reg_dados->attackType, NADA_CONSTA);
        }
    }
    if (reg_dados->targetIndustry == NULL) {
        reg_dados->targetIndustry = (char *)malloc(strlen(NADA_CONSTA));
        if (reg_dados->targetIndustry != NULL) {
            strcpy(reg_dados->targetIndustry, NADA_CONSTA);
        }
    }
    if (reg_dados->defenseMechanism == NULL) {
        reg_dados->defenseMechanism = (char *)malloc(strlen(NADA_CONSTA));
        if (reg_dados->defenseMechanism != NULL) {
            strcpy(reg_dados->defenseMechanism, NADA_CONSTA);
        }
    }

    return reg_dados;
}

/*
 * Imprime de maneira formatada os dados de um registro binário.
 * Parâmetro:
 *  reg_dados - ponteiro para o registro a ser impresso
 */
void imprime_registro_bin(REG_DADOS *reg_dados)
{
    if (reg_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Imprime todos os campos
    printf("IDENTIFICADOR DO ATAQUE: %d\n", reg_dados->idAttack);

    if (reg_dados->year == LIXO_INT) {
        printf("ANO EM QUE O ATAQUE OCORREU: NADA CONSTA\n");
    }
    else {
        printf("ANO EM QUE O ATAQUE OCORREU: %d\n", reg_dados->year);
    }

    printf("PAIS ONDE OCORREU O ATAQUE: %s\n", reg_dados->country);
    printf("SETOR DA INDUSTRIA QUE SOFREU O ATAQUE: %s\n", reg_dados->targetIndustry);
    printf("TIPO DE AMEACA A SEGURANCA CIBERNETICA: %s\n", reg_dados->attackType);

    if (reg_dados->financialLoss == LIXO_FLOAT) {
        printf("PREJUIZO CAUSADO PELO ATAQUE: NADA CONSTA\n");
    }
    else {
        printf("PREJUIZO CAUSADO PELO ATAQUE: %.2f\n", reg_dados->financialLoss);
    }

    printf("ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA: %s\n\n", reg_dados->defenseMechanism);

    // Libera a memória alocada para o registro
    free(reg_dados->country);
    free(reg_dados->attackType);
    free(reg_dados->targetIndustry);
    free(reg_dados->defenseMechanism);
    free(reg_dados);
    reg_dados = NULL;
    return;
}

/*
 * Imprime todos os registros do arquivo binário.
 * Parâmetro:
 *  bin - ponteiro para o arquivo binário
 */
void imprime_bin(FILE *bin)
{
    // Pula o cabeçalho (276 bytes)
    if (ftell(bin) == 0) {
        fseek(bin, 276, SEEK_SET);
    }

    // Imprime cada registro até o final do arquivo
    REG_DADOS *reg_dados = get_registro(bin);
    while (ftell(bin) != -1 && reg_dados != NULL) {
        imprime_registro_bin(reg_dados);
        
        reg_dados = get_registro(bin);
    }

    fclose(bin); // Fecha o arquivo
}