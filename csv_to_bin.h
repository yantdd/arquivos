#ifndef CSV_TO_BIN_H
#define CSV_TO_BIN_H
#include "parse_csv.h"


// Estrutura para armazenar o cabeçalho do arquivo binário
typedef struct reg_header {
    char status;                        // '0' para arquivo inconsistente, '1' para arquivo consistente.
    long long int topo;                 // Byte offset de um registro logicamente removido, ou -1 caso não haja.
    long long int proxByteOffset;       // Próximo byte offset disponível. Deve ser iniciado com o valor ‘0’. 
    int nroRegArq;                      // Número de registros não removidos presentes no arquivo. Deve ser iniciado com o valor ‘0’. 
    int nroRegRem;                      // Número de registros logicamente marcados como removidos. Deve ser iniciado com o valor ‘0’
    char descreveIdentificador[23];
    char descreveYear[27];
    char descreveFinancialLoss[28];
    char codDescreveCountry;            // Cod. keyword, assume o valor “1”.
    char descreveCountry[26];
    char codDescreveType;               // Cod. keyword, assume o valor “2”.
    char descreveType[38]; 
    char codDescreveTargetIndustry;     // Cod. keyword, assume o valor “3”.
    char descreveTargetIndustry[38];
    char codDescreveDefense;            // Cod. keyword, assume o valor “4”.
    char descreveDefense[67];
} REG_HEADER;


// Estrutura para armazenar os dados de cada registro do arquivo binário
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

REG_HEADER *inicializa_header_registro();
bool escreve_header_no_bin(FILE *bin);
int calcula_tamanho_registro(LINHA_CSV *linha);
void atualiza_header(FILE *bin, int nroRegArq, long long int proxByteOffset);
REG_DADOS *converte_linha_csv_para_registro_bin(FILE *csv);
bool escreve_registros_no_bin(FILE *csv, FILE *bin);
void binarioNaTela(char *nomeArquivoBinario);
REG_HEADER* get_reg_header(FILE *bin);

#endif