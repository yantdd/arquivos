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
#define DELIM_CAMPO '|'

/*
 * Inicializa a struct REG_HEADER, que armazena dados do header do registro.
 * Retorno:
 *  ponteiro para o header do registro ou NULL em caso de erro.
 */
REG_HEADER *inicializa_header_registro()
{
    REG_HEADER *header = (REG_HEADER *)calloc(1, sizeof(REG_HEADER));
    if (header == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    // Inicializa valores padrão do header, antes de modificações
    header->status = '0';       // '0' = inconsistente, '1' = consistente
    header->topo = -1;          // Topo da pilha de registros removidos
    header->proxByteOffset = 0; // Próxima posição disponível no arquivo
    header->nroRegArq = 0;      // Número de registros no arquivo
    header->nroRegRem = 0;      // Número de registros removidos

    // Carrega descrições dos campos
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

/*
 * Escreve o header no arquivo binário.
 * Parâmetro:
 *  bin - ponteiro para o arquivo binário
 * Retorno:
 *  true se a operação for bem-sucedida, false caso contrário.
 */
bool escreve_header_no_bin(FILE *bin)
{
    REG_HEADER *header = inicializa_header_registro();
    if (header == NULL)
    {
        return false;
    }

    // Escreve cada campo do header no arquivo binário
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

/*
 * Calcula o tamanho do registro, considerando os campos fixos e variáveis.
 * Parâmetro:
 *  linha - ponteiro para a linha CSV
 * Retorno:
 *  tamanho do registro em bytes.
 */
int calcula_tamanho_registro(LINHA_CSV *linha)
{
    int tamanho = 0;                  // Inicializa o tamanho
    tamanho += sizeof(long long int); // Campo 'prox'
    tamanho += sizeof(int);           // Campo 'idAttack'
    tamanho += sizeof(int);           // Campo 'year'
    tamanho += sizeof(float);         // Campo 'financialLoss'

    // Campo 'country'
    if (memcmp(linha->country, LIXO_STRING, strlen(LIXO_STRING)) != 0)
    {
        tamanho += strlen(linha->country) + 2; // +2 para o delimitador e keyword
    }
    // Campo 'attackType'
    if (memcmp(linha->attackType, LIXO_STRING, strlen(LIXO_STRING)) != 0)
    {
        tamanho += strlen(linha->attackType) + 2; // +2 para o delimitador e keyword
    }
    // Campo 'targetIndustry'
    if (memcmp(linha->targetIndustry, LIXO_STRING, strlen(LIXO_STRING)) != 0)
    {
        tamanho += strlen(linha->targetIndustry) + 2; // +2 para o delimitador e keyword
    }
    // Campo 'defenseMechanism'
    if (memcmp(linha->defenseMechanism, LIXO_STRING, strlen(LIXO_STRING)) != 0)
    {
        tamanho += strlen(linha->defenseMechanism) + 2; // +2 para o delimitador e keyword
    }
    return tamanho;
}

/*
 * Converte uma linha do arquivo CSV para um registro de dados no binário.
 * Parâmetro:
 *  csv - ponteiro para o arquivo CSV
 * Retorno:
 *  ponteiro para o registro binário ou NULL em caso de erro.
 */
REG_DADOS *converte_linha_csv_para_registro_bin(FILE *csv)
{
    REG_DADOS *reg_dados = (REG_DADOS *)calloc(1, sizeof(REG_DADOS));
    if (reg_dados == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    // Leitura da linha do arquivo csv
    LINHA_CSV *linha = le_linha_csv(csv);
    if (linha == NULL)
    {
        free(reg_dados);
        return NULL;
    }

    // Copia dados fixos do CSV para o registro binário
    reg_dados->removido = '0'; // 0 = não removido
    reg_dados->prox = -1;
    reg_dados->tamanhoRegistro = calcula_tamanho_registro(linha);
    reg_dados->idAttack = linha->idAttack;
    reg_dados->year = linha->year;
    reg_dados->financialLoss = linha->financialLoss;

    // Aloca e copia campos variáveis do CSV para o registro binário
    reg_dados->country = malloc(strlen(linha->country));
    if (reg_dados->country == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->country, linha->country);

    reg_dados->attackType = malloc(strlen(linha->attackType));
    if (reg_dados->attackType == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        free(reg_dados->country);
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->attackType, linha->attackType);

    reg_dados->targetIndustry = malloc(strlen(linha->targetIndustry));
    if (reg_dados->targetIndustry == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados);
        return NULL;
    }
    strcpy(reg_dados->targetIndustry, linha->targetIndustry);

    reg_dados->defenseMechanism = malloc(strlen(linha->defenseMechanism));
    if (reg_dados->defenseMechanism == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
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

/*
 * Atualiza o header do arquivo binário com o novo número de registros e o próximo byte offset.
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  nroRegArq - número de registros no arquivo
 *  proxByteOffset - próximo byte offset disponível
 */
void atualiza_header(FILE *bin, int nroRegArq, long long int proxByteOffset)
{
    fseek(bin, 0, SEEK_SET); // Partimos do início do arquivo

    // Marca como consistente
    char status = '1'; 
    fwrite(&status, sizeof(char), 1, bin);

    // Pular o campo 'topo' que já está definido
    fseek(bin, sizeof(long long int), SEEK_CUR);

    // Escreve no header o próximo byte offset disponível
    fwrite(&proxByteOffset, sizeof(long long int), 1, bin);

    // Escreve no header o número de registros
    fwrite(&nroRegArq, sizeof(int), 1, bin);

    return;
}

/*
 * Escreve os registros do arquivo CSV no arquivo binário.
 * Parâmetros:
 *  csv - ponteiro para o arquivo CSV
 *  bin - ponteiro para o arquivo binário
 * Retorno:
 *  true se a operação for bem-sucedida, false caso contrário.
 */
bool escreve_registros_no_bin(FILE *csv, FILE *bin)
{

    escreve_header_no_bin(bin); // Escreve o header no arquivo binário

    int nroRegArq = 0; // Inicializa um contador para armazenar o número de registros

    while (true)
    {
        REG_DADOS *reg_dados = converte_linha_csv_para_registro_bin(csv);
        if (reg_dados == NULL)
        {
            break;
        }

        fwrite(&reg_dados->removido, sizeof(char), 1, bin);
        fwrite(&reg_dados->tamanhoRegistro, sizeof(int), 1, bin);
        fwrite(&reg_dados->prox, sizeof(long long int), 1, bin);
        fwrite(&reg_dados->idAttack, sizeof(int), 1, bin);
        fwrite(&reg_dados->year, sizeof(int), 1, bin);
        fwrite(&reg_dados->financialLoss, sizeof(float), 1, bin);

        // Verifica se não é lixo
        if (memcmp(reg_dados->country, LIXO_STRING, strlen(LIXO_STRING)) != 0)
        {
            fputc('1', bin);                                                // Codigo do campo country
            fwrite(reg_dados->country, strlen(reg_dados->country), 1, bin); // Escreve o country
            fputc(DELIM_CAMPO, bin);                                        // Escreve o delimitador
        }

        if (memcmp(reg_dados->attackType, LIXO_STRING, strlen(LIXO_STRING)) != 0)
        {
            fputc('2', bin); // Codigo do campo attaclkType
            fwrite(reg_dados->attackType, strlen(reg_dados->attackType), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        if (memcmp(reg_dados->targetIndustry, LIXO_STRING, strlen(LIXO_STRING)) != 0)
        {
            fputc('3', bin); // Codigo do campo targetIndustry
            fwrite(reg_dados->targetIndustry, strlen(reg_dados->targetIndustry), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        if (memcmp(reg_dados->defenseMechanism, LIXO_STRING, strlen(LIXO_STRING)) != 0)
        {
            fputc('4', bin); // Codigo do campo defenseMechanism
            fwrite(reg_dados->defenseMechanism, strlen(reg_dados->defenseMechanism), 1, bin);
            fputc(DELIM_CAMPO, bin);
        }

        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados->targetIndustry);
        free(reg_dados->defenseMechanism);
        free(reg_dados);

        // Incrementa o número de registros
        nroRegArq++;
    }

    // Atualiza o próximo byte offset
    long long int proxOffset = ftell(bin);

    // Atualiza o header com o número de registros e o próximo byte offset
    atualiza_header(bin, nroRegArq, proxOffset);
    
    fclose(bin);
    fclose(csv);
    return true;
}

// Função fornecida pelo monitor
void binarioNaTela(char *nomeArquivoBinario)
{
    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb")))
    {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}