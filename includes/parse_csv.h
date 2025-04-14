#ifndef PARSE_CSV_H
#define PARSE_CSV_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DELIMITADOR ','

typedef struct linha_csv {
    int idAttack;
    int year;
    float financialLoss;
    char country[50];
    char attackType[50];
    char targetIndustry[50];
    char defenseMechanism[50];
} LINHA_CSV;

bool eh_delimitador(int c);
LINHA_CSV *incializa_linha_csv();
void pula_descricao_csv(FILE *csv);
LINHA_CSV *le_linha_csv(FILE *csv);

#endif // PARSE_CSV_H