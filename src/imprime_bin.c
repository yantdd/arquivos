#include "parse_csv.h"
#include "csv_to_bin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define LIXO_STRING "$"
#define LIXO_INT -1
#define LIXO_FLOAT -1.0
#define DELIM '|'
#define NADA_CONSTA "NADA CONSTA"


typedef struct reg_dados {
    char removido;
    int tamanhoRegistro;
    long long int prox;
    int idAttack;
    int year;
    float financialLoss;
    char *country;
    char *attackType;
    char *targetIndustry;
    char *defenseMechanism;
} REG_DADOS;



REG_DADOS *get_registro(FILE *bin) {
    REG_DADOS *reg_dados = (REG_DADOS *)calloc(1, sizeof(REG_DADOS));
    if (reg_dados == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o registro de dados.\n");
        return NULL;
    }

    reg_dados->country = NULL;
    reg_dados->attackType = NULL;
    reg_dados->targetIndustry = NULL;
    reg_dados->defenseMechanism = NULL;

    if (fread(&reg_dados->removido, sizeof(char), 1, bin) != 1) {
        free(reg_dados);
        return NULL;  // End of file
    }

    fread(&reg_dados->tamanhoRegistro, sizeof(int), 1, bin);
    fread(&reg_dados->prox, sizeof(long long int), 1, bin);
    fread(&reg_dados->idAttack, sizeof(int), 1, bin);
    fread(&reg_dados->year, sizeof(int), 1, bin);
    fread(&reg_dados->financialLoss, sizeof(float), 1, bin);

    int tamanho_reg = reg_dados->tamanhoRegistro - sizeof(long long int) - 2*sizeof(int) - sizeof(float);
    

    while (tamanho_reg > 0) {
        char c = fgetc(bin);
        tamanho_reg--;

        switch (c) {
            case '1':
                reg_dados->country = (char *)malloc(50);
                fscanf(bin, "%[^|]", reg_dados->country);
                tamanho_reg -= strlen(reg_dados->country);
                break;
            case '2':
                reg_dados->attackType = (char *)malloc(50);
                fscanf(bin, "%[^|]", reg_dados->attackType);
                tamanho_reg -= strlen(reg_dados->attackType);
                break;
            case '3':
                reg_dados->targetIndustry = (char *)malloc(50);
                fscanf(bin, "%[^|]", reg_dados->targetIndustry);
                tamanho_reg -= strlen(reg_dados->targetIndustry);
                break;
            case '4':
                reg_dados->defenseMechanism = (char *)malloc(50);
                fscanf(bin, "%[^|]", reg_dados->defenseMechanism);
                tamanho_reg -= strlen(reg_dados->defenseMechanism);
                break;
        }
    }
    
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

void imprime_registro_bin(REG_DADOS *reg_dados) {
    if (reg_dados == NULL) {
        fprintf(stderr, "Registro de dados é nulo.\n");
        return;
    }
    
    // Imprimir os dados do registro
    printf("IDENTIFICADOR DO ATAQUE: %d\n", reg_dados->idAttack);

    if(reg_dados->year == LIXO_INT) {
        printf("ANO EM QUE O ATAQUE OCORREU: NADA CONSTA\n");
    } else {
        printf("ANO EM QUE O ATAQUE OCORREU: %d\n", reg_dados->year);
    }
    
    printf("PAIS ONDE OCORREU O ATAQUE: %s\n", reg_dados->country);
    printf("SETOR DA INDUSTRIA QUE SOFREU O ATAQUE: %s\n", reg_dados->targetIndustry);
    printf("TIPO DE AMEACA A SEGURANCA CIBERNETICA: %s\n", reg_dados->attackType);

    if (reg_dados->financialLoss == LIXO_FLOAT) {
        printf("PREJUIZO CAUSADO PELO ATAQUE: NADA CONSTA\n");
    } else {
        printf("PREJUIZO CAUSADO PELO ATAQUE: %.2f\n", reg_dados->financialLoss);
    }
    
    printf("ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA: %s\n\n", reg_dados->defenseMechanism);

    free(reg_dados->country);
    free(reg_dados->attackType);
    free(reg_dados->targetIndustry);
    free(reg_dados->defenseMechanism);
    free(reg_dados);
    reg_dados = NULL;
    return;
}

void imprime_bin(FILE *bin) {
    if (ftell(bin) == 0) {
        fseek(bin, 276, SEEK_SET);
    }

    REG_DADOS *reg_dados = get_registro(bin);
    while (ftell(bin) != -1 && reg_dados != NULL) {
        imprime_registro_bin(reg_dados);
        reg_dados = get_registro(bin);
    }
}


// int main(void) {
//     FILE *bin = fopen("ataque2.bin", "rb");

//     if (bin == NULL) {
//         fprintf(stderr, "Erro ao abrir o arquivo binário.\n");
//         return 1;
//     }

    
//     imprime_bin(bin);

//     fclose(bin);
//     return 0;
// }