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



#include "parse_csv.h"
#include <stdint.h>

/**
 * 
 * O csv a ser parseado tem os seguintes campos:
 * 0 - idAttack (fixo)
 * 1 - year (fixo)
 * 2 - financialLoss (fixo)
 * 3 - country
 * 4 - attackType
 * 5 - targetIndustry
 * 6 - defenseMechanism
 * 
 **/ 


// Enumeração dos campos, para uma nomenclatura mais significativa ao tratar dados
enum
{
    ID_ATTACK = 0,
    YEAR,
    FINANCIAL_LOSS,
    COUNTRY,
    ATTACK_TYPE,
    TARGET_INDUSTRY,
    DEFENSE_MECHANISM
};

/*
 * Indica se o caractere lido do csv é um delimitador (',').
 * Parâmetro:
 *  c - caractere a ser verificado
 */
bool eh_delimitador(int c)
{
    return c == DELIMITADOR;
}

/*
 * Inicializa uma struct LINHA_CSV com valores padrão, antes que seja preenchida com dados reais de uma linha do CSV.
 * Retorno:
 *  ponteiro para a struct LINHA_CSV inicializada ou NULL em caso de erro.
 */
LINHA_CSV *incializa_linha_csv()
{
    LINHA_CSV *linha = (LINHA_CSV *)calloc(1, sizeof(LINHA_CSV));
    if (linha == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    // Inicializa os campos inteiros com -1 e float com -1.0
    linha->idAttack = -1;
    linha->year = -1;
    linha->financialLoss = -1.0;

    // INcializa campos de texto com o valor de lixo padrão ('$')
    strcpy(linha->country, "$");
    strcpy(linha->attackType, "$");
    strcpy(linha->targetIndustry, "$");
    strcpy(linha->defenseMechanism, "$");

    return linha;
}

/*
 * Pula a primeira linha do arquivo CSV (254 bytes), para ter acesso aos dados.
 * Parâmetro:
 *  csv - ponteiro para o arquivo CSV
 */
void pula_descricao_csv(FILE *csv)
{
    // Se estiver no início do arquivo, pula a descrição
    if (ftell(csv) == 0)
    {                              
        fseek(csv, 254, SEEK_SET);
    }
}

/*
 * Lê uma linha do arquivo CSV e preenche a struct LINHA_CSV com os dados lidos.
 * Parâmetro:
 *  csv - ponteiro para o arquivo CSV
 * Retorno:
 *  ponteiro para a linha lida ou NULL em caso de erro ou fim do arquivo.
 */
LINHA_CSV *le_linha_csv(FILE *csv)
{
    LINHA_CSV *linha = incializa_linha_csv();
    if (linha == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    pula_descricao_csv(csv);

    int c;               // Armazena o caractere lido do arquivo.
    int campo_atual = 0; // Controla em qual campo da linha CSV estamos (0 = primeiro campo, 1 = segundo campo, etc.)


    if ((c = fgetc(csv)) == EOF)
    {
        free(linha);
        return NULL;
    }

    ungetc(c, csv); 

    // Loop para leitura da linha até que se atinja o fim do arquivo ou fim da linha
    while ((c = fgetc(csv)) != EOF && c != '\n')
    {
        if (eh_delimitador(c))
        { // Se for delimitador, mudamos de campo
            campo_atual++;
            continue;
        }
        ungetc(c, csv);

        // Preenchimento da struct LINHA_CSV de acordo com o campo lido
        // Caso não haja dados no campo, preenche com o valor padrão
        switch (campo_atual)
        {
        case ID_ATTACK:
            fscanf(csv, "%d", &linha->idAttack); // Campo idAttack sempre estará presente
            break;
        case YEAR:
            if (fscanf(csv, "%d", &linha->year) != 1)
            {
                linha->year = -1; // Valor padrão em caso de falha
            }
            break;
        case FINANCIAL_LOSS:
            if (fscanf(csv, "%f", &linha->financialLoss) != 1)
            {
                linha->financialLoss = -1.0; // Valor padrão em caso de campo vazio
            }
            break;
        case COUNTRY:
            if (fscanf(csv, "%[^,]", linha->country) < 0)
            {
                strcpy(linha->country, "$"); // Valor padrão em caso de campo vazio
            }
            break;
        case ATTACK_TYPE:
            if (fscanf(csv, "%[^,]", linha->attackType) < 0)
            {
                strcpy(linha->attackType, "$"); // Valor padrão em caso de campo vazio
            }
            break;
        case TARGET_INDUSTRY:
            if (fscanf(csv, "%[^,]", linha->targetIndustry) < 0)
            {
                strcpy(linha->targetIndustry, "$"); // Valor padrão em caso de campo vazio
            }
            break;
        case DEFENSE_MECHANISM:
            if (fscanf(csv, "%[^\n]", linha->defenseMechanism) < 0)
            {
                strcpy(linha->defenseMechanism, "$"); // Valor padrão em caso de campo vazio
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
