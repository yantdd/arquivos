#include "parse_csv.h"
#include "csv_to_bin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define LIXO_STRING "$"
#define LIXO_INT -1
#define LIXO_DOUBLE -1.0
#define DELIM_CAMPO '|'

typedef struct reg_header {
    char status; // '0' para arquivo inconsistente, '1' para arquivo consistente.
    long long int topo; // Byte offset de um registro logicamente removido, ou -1 caso não haja.
    long long int proxByteOffset; // Próximo byte offset disponível. Deve ser iniciado com o valor ‘0’. 
    int nroRegArq; // Número de registros não removidos presentes no arquivo. Deve ser iniciado com o valor ‘0’. 
    int nroRegRem; // Número de registros logicamente marcados como removidos. Deve ser iniciado com o valor ‘0’
    char descreveIdentificador[23];
    char descreveYear[27];
    char descreveFinancialLoss[28];
    char codDescreveCountry; // Cod. keyword, assume o valor “1”.
    char descreveCountry[26];
    char codDescreveType; // Cod. keyword, assume o valor “2”.
    char descreveType[38]; 
    char codDescreveTargetIndustry; // Cod. keyword, assume o valor “3”.
    char descreveTargetIndustry[38];
    char codDescreveDefense; // Cod. keyword, assume o valor “4”.
    char descreveDefense[67];
} REG_HEADER;

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







REG_HEADER *inicializa_header() {
    REG_HEADER *header = (REG_HEADER *)calloc(1, sizeof(REG_HEADER));
    if (header == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o cabeçalho.\n");
        return NULL;
    }

    header->status = '0';
    header->topo = -1;
    header->proxByteOffset = 0;
    header->nroRegArq = 0;
    header->nroRegRem = 0;
    memcpy(header->descreveIdentificador, "IDENTIFICADOR DO ATAQUE", 23);
    memcpy(header->descreveYear, "ANO EM QUE O ATAQUE OCORREU", 27);
    memcpy(header->descreveFinancialLoss, "PREJUIZO CAUSADO PELO ATAQUE", 28);
    header->codDescreveCountry = '1';
    memcpy(header->descreveCountry, "PAIS ONDE OCORREU O ATAQUE", 26);
    header->codDescreveType = '2';
    memcpy(header->descreveType, "TIPO DE AMEACA A SEGURANCA CIBERNETICA", 38);
    header->codDescreveTargetIndustry = '3';
    memcpy(header->descreveTargetIndustry, "SETOR DA INDUSTRIA QUE SOFREU O ATAQUE", 38);
    header->codDescreveDefense = '4';
    memcpy(header->descreveDefense, "ESTRATEGIA DE DEFESA CIBERNETICA EMPREGADA PARA RESOLVER O PROBLEMA", 67);
    return header;
}




bool escreve_header_no_bin(FILE *bin) {
    REG_HEADER *header = inicializa_header();
    if (header == NULL) {
        return false;
    }
    //fwrite(header, sizeof(REG_HEADER), 1, bin);
    fwrite(&header->status, sizeof(char), 1, bin);
    fwrite(&header->topo, sizeof(long long int), 1, bin);
    fwrite(&header->proxByteOffset, sizeof(long long int), 1, bin);
    fwrite(&header->nroRegArq, sizeof(int), 1, bin);
    fwrite(&header->nroRegRem, sizeof(int), 1, bin);
    fwrite(header->descreveIdentificador, sizeof(char), 23, bin);
    fwrite(header->descreveYear, sizeof(char), 27, bin);
    fwrite(header->descreveFinancialLoss, sizeof(char), 28, bin);
    fwrite(&header->codDescreveCountry, sizeof(char), 1, bin);
    fwrite(header->descreveCountry, sizeof(char), 26, bin);
    fwrite(&header->codDescreveType, sizeof(char), 1, bin);
    fwrite(header->descreveType, sizeof(char), 38, bin);
    fwrite(&header->codDescreveTargetIndustry, sizeof(char), 1, bin);
    fwrite(header->descreveTargetIndustry, sizeof(char), 38, bin);
    fwrite(&header->codDescreveDefense, sizeof(char), 1, bin);
    fwrite(header->descreveDefense, sizeof(char), 67, bin);
    free(header);
    return true;
}





int calcula_tamanho_reg(LINHA_CSV *linha) {
    int tamanho = 0;
    tamanho += sizeof(long long int); // Campo 'prox'
    if (linha->idAttack != LIXO_INT) {
        tamanho += sizeof(int);
    }
    if (linha->year != LIXO_INT) {
        tamanho += sizeof(int);
    }
    if (linha->financialLoss != LIXO_DOUBLE) {
        tamanho += sizeof(double); // acho que tem um erro aqui, deveria ser float!!!!!!!!!
    }
    if (strcmp(linha->country, LIXO_STRING) != 0) {
        tamanho += strlen(linha->country) + 1; // +1 para o delimitador
    }
    if (strcmp(linha->attackType, LIXO_STRING) != 0) {
        tamanho += strlen(linha->attackType) + 1; // +1 para o delimitador
    }
    if (strcmp(linha->targetIndustry, LIXO_STRING) != 0) {
        tamanho += strlen(linha->targetIndustry) + 1; // +1 para o delimitador
    }
    if (strcmp(linha->defenseMechanism, LIXO_STRING) != 0) {
        tamanho += strlen(linha->defenseMechanism) + 1; // +1 para o delimitador
    }
    return tamanho;
}




REG_DADOS *linha_csv_para_reg_bin(FILE *csv) {
    REG_DADOS *reg_dados = (REG_DADOS *)calloc(1, sizeof(REG_DADOS));
    if (reg_dados == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o registro de dados.\n");
        return NULL;
    }

    
    reg_dados->removido = '0';
    reg_dados->prox = -1;
    
    LINHA_CSV *linha = le_linha_csv(csv);
    if (linha == NULL) {
        free(reg_dados);
        return NULL;
    }

    reg_dados->tamanhoRegistro = calcula_tamanho_reg(linha);
    reg_dados->idAttack = linha->idAttack;
    reg_dados->year = linha->year;
    reg_dados->financialLoss = linha->financialLoss;

    reg_dados->country = malloc(strlen(linha->country));
    if (reg_dados->country == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o país.\n");
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->country, linha->country);

    reg_dados->attackType = malloc(strlen(linha->attackType));
    if (reg_dados->attackType == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o tipo de ataque.\n");
        free(reg_dados->country);
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->attackType, linha->attackType);

    reg_dados->targetIndustry = malloc(strlen(linha->targetIndustry));
    if (reg_dados->targetIndustry == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a indústria alvo.\n");
        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->targetIndustry, linha->targetIndustry);

    reg_dados->defenseMechanism = malloc(strlen(linha->defenseMechanism));
    if (reg_dados->defenseMechanism == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o mecanismo de defesa.\n");
        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados->targetIndustry);
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->defenseMechanism, linha->defenseMechanism);
    free(linha);

    return reg_dados;
}




bool escreve_dados_no_bin(FILE *csv, FILE *bin) {

    escreve_header_no_bin(bin); // Escreve o cabeçalho no arquivo binário
    
    
    int nroRegArq = 0; // Inicializa um contador para armazenar o número de registros
    
    while (true) {
        REG_DADOS *reg_dados = linha_csv_para_reg_bin(csv);
        if (reg_dados == NULL) {
            break;
        }
        
        fwrite(&reg_dados->removido, sizeof(char), 1, bin);
        fwrite(&reg_dados->tamanhoRegistro, sizeof(int), 1, bin);
        fwrite(&reg_dados->prox, sizeof(long long int), 1, bin);
        

        if (reg_dados->idAttack != LIXO_INT) {
            fwrite(&reg_dados->idAttack, sizeof(int), 1, bin);
        }

        if (reg_dados->year != LIXO_INT) {
            fwrite(&reg_dados->year, sizeof(int), 1, bin);
        }

        if (reg_dados->financialLoss != LIXO_DOUBLE) {
            fwrite(&reg_dados->financialLoss, sizeof(float), 1, bin);
        }

        
        if (strcmp(reg_dados->country, LIXO_STRING) != 0) {
            fputc('1', bin); // Codigo do campo country
            fwrite(reg_dados->country, strlen(reg_dados->country), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        if (strcmp(reg_dados->attackType, LIXO_STRING) != 0) {
            fputc('2', bin); // Codigo do campo attaclkType
            fwrite(reg_dados->attackType, strlen(reg_dados->attackType), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        if (strcmp(reg_dados->targetIndustry, LIXO_STRING) != 0) {
            fputc('3', bin); // Codigo do campo targetIndustry
            fwrite(reg_dados->targetIndustry, strlen(reg_dados->targetIndustry), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        if (strcmp(reg_dados->defenseMechanism, LIXO_STRING) != 0) {
            fputc('4', bin); // Codigo do campo defenseMechanism
            fwrite(reg_dados->defenseMechanism, strlen(reg_dados->defenseMechanism), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados->targetIndustry);
        free(reg_dados->defenseMechanism);
        free(reg_dados);
        
        nroRegArq++;
    }

    long long int proxOffset = ftell(bin);
    
    // Atualizar o cabeçalho com o número de registros e proxByteOffset
    fseek(bin, 0, SEEK_SET);
    char status = '1';
    fwrite(&status, sizeof(char), 1, bin);
    
    // Pular o campo 'topo' que já está definido
    fseek(bin, sizeof(long long int), SEEK_CUR);
    
    // Escrever o próximo byte disponível
    fwrite(&proxOffset, sizeof(long long int), 1, bin);
    
    // Escrever o número de registros
    fwrite(&nroRegArq, sizeof(int), 1, bin);

    return true;
}

void binarioNaTela(char *nomeArquivoBinario) {
    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

int main() {
    FILE *csv = fopen("ataque1.csv", "r");
    if (csv == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo CSV.\n");
        return 1;
    }

    FILE *bin = fopen("out.bin", "wb");
    if (bin == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo binário.\n");
        fclose(csv);
        return 1;
    }

    escreve_dados_no_bin(csv, bin);

    fclose(csv);
    fclose(bin);

    binarioNaTela("out.bin");

    return 0;
}