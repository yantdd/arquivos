#include "parse_csv.h"
#include <stdint.h>

// O nosso csv tem os seguintes campos:
// 0 - idAttack (fixo)
// 1 - year (fixo)
// 2 - financialLoss (fixo)
// 3 - country
// 4 - attackType
// 5 - targetIndustry
// 6 - defenseMechanism

enum {
    ID_ATTACK = 0,
    YEAR,
    FINANCIAL_LOSS,
    COUNTRY,
    ATTACK_TYPE,
    TARGET_INDUSTRY,
    DEFENSE_MECHANISM
};

typedef struct linha_csv LINHA_CSV;

bool eh_delimitador(int c) {
    return c == DELIMITADOR;
}

LINHA_CSV *incializa_linha_csv() {
    LINHA_CSV *linha = (LINHA_CSV *)calloc(1, sizeof(LINHA_CSV));
    if (linha == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a linha CSV.\n");
        return NULL;
    }

    
    linha->idAttack = -1;
    linha->year = -1;
    linha->financialLoss = -1.0;
    
    // Garantir que as strings sejam inicializadas corretamente
    strcpy(linha->country, "$");
    strcpy(linha->attackType, "$");
    strcpy(linha->targetIndustry, "$");
    strcpy(linha->defenseMechanism, "$");

    return linha;
}

void pula_descricao_csv(FILE *csv) {
    if (ftell(csv) == 0){
        fseek(csv, 254, SEEK_SET);
    }
}

LINHA_CSV *le_linha_csv(FILE *csv) {
    LINHA_CSV *linha = incializa_linha_csv();
    if (linha == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a linha CSV.\n");
        return NULL;
    }

    pula_descricao_csv(csv);

    int c;
    int campo_atual = 0;
    
    // Verificar se já estamos no final do arquivo
    if ((c = fgetc(csv)) == EOF) {
        free(linha);
        return NULL;
    }
    ungetc(c, csv);
    
    while ((c = fgetc(csv)) != EOF && c != '\n') {
        if (eh_delimitador(c)) {
            campo_atual++;
            continue;
        }

        ungetc(c, csv);
        switch (campo_atual) {
            case ID_ATTACK:
                fscanf(csv, "%d", &linha->idAttack);
                break;
            case YEAR:
                if (fscanf(csv, "%d", &linha->year) != 1) {
                    linha->year = -1; // Valor padrão em caso de falha
                }
                break;
            case FINANCIAL_LOSS:
                if (fscanf(csv, "%f", &linha->financialLoss) != 1) {
                    linha->financialLoss = -1.0; // Valor padrão em caso de falha
                }
                break;
            case COUNTRY:
                if (fscanf(csv, "%[^,]", linha->country) < 0) {
                    strcpy(linha->country, "$"); // Valor padrão em caso de falha
                }
                break;
            case ATTACK_TYPE:
                if (fscanf(csv, "%[^,]", linha->attackType) < 0) {
                    strcpy(linha->attackType, "$"); // Valor padrão em caso de falha
                }
                break;
            case TARGET_INDUSTRY:
                if (fscanf(csv, "%[^,]", linha->targetIndustry) < 0) {
                    strcpy(linha->targetIndustry, "$"); // Valor padrão em caso de falha
                }
                break;
            case DEFENSE_MECHANISM:
                if (fscanf(csv, "%[^\n]", linha->defenseMechanism) < 0) {
                    strcpy(linha->defenseMechanism, "$"); // Valor padrão em caso de falha
                }
                break;
            default:
                // Consumir o resto da linha em caso de campos extras
                fscanf(csv, "%*[^,\n]");
                break;
        }
    }

    return linha;
    
}
