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
#include <stdbool.h>
#include "csv_to_bin.h"
#include "imprime_bin.h"
#include "busca_bin.h"

/*
 * Atualiza apenas os dados da lista encadeada no header (sem alterar status)
 * Parâmetros:
 *  header - ponteiro para o header
 *  bin - ponteiro para o arquivo binário
 */
void atualiza_dados_header(REG_HEADER *header, FILE* bin)
{
    fseek(bin, 0, SEEK_SET); // Vai para o início do arquivo
    
    // Pula o status (não altera durante as operações)
    fseek(bin, sizeof(char), SEEK_CUR);
    
    // Escreve o topo da lista de removidos
    fwrite(&header->topo, sizeof(long long int), 1, bin);
    
    // Pula o proxByteOffset
    fseek(bin, sizeof(long long int), SEEK_CUR);
    
    // Atualiza números de registros
    fwrite(&header->nroRegArq, sizeof(int), 1, bin);
    fwrite(&header->nroRegRem, sizeof(int), 1, bin);
}

/*
 * Atualiza o header do arquivo com informações da lista de registros removidos
 * Parâmetros:
 *  header - ponteiro para o header
 *  bin - ponteiro para o arquivo binário
 */
void atualiza_header_removido(REG_HEADER *header, FILE* bin)
{
    fseek(bin, 0, SEEK_SET); // Vai para o início do arquivo
    
    // Marca o arquivo como consistente
    char status = '1';
    fwrite(&status, sizeof(char), 1, bin);
    
    // Escreve o topo da lista de removidos
    fwrite(&header->topo, sizeof(long long int), 1, bin);
    
    // Pula o proxByteOffset
    fseek(bin, sizeof(long long int), SEEK_CUR);
    
    // Atualiza números de registros
    fwrite(&header->nroRegArq, sizeof(int), 1, bin);
    fwrite(&header->nroRegRem, sizeof(int), 1, bin);
}

/*
 * Busca e remove registros que correspondem aos critérios especificados
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  header - ponteiro para o header
 *  num_campos - número de campos nos critérios de busca
 *  nome_campo - array de nomes dos campos
 *  valor_campo - array de valores dos campos
 */
void match_registro_remove(FILE *bin, REG_HEADER *header, int num_campos, char **nome_campo, char **valor_campo)
{
    // Posiciona após o cabeçalho
    fseek(bin, 276, SEEK_SET);
    
    bool match = false;
    
    // Processa cada registro do arquivo
    while (true) {
        long long posicao_registro = ftell(bin);
        REG_DADOS *reg_dados = get_registro(bin);
        
        if (reg_dados == NULL) {
            break; // Fim do arquivo
        }
        
        // Pula registros já removidos
        if (reg_dados->removido == '1') {
            // Libera memória e continua
            free(reg_dados->country);
            free(reg_dados->attackType);
            free(reg_dados->targetIndustry);
            free(reg_dados->defenseMechanism);
            free(reg_dados);
            continue;
        }
        
        // Verifica se o registro atende aos critérios
        match = true;
        for (int i = 0; i < num_campos; i++) {
            if (strcmp(nome_campo[i], "idAttack") == 0) {
                int valor = atoi(valor_campo[i]);
                if (reg_dados->idAttack != valor) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "year") == 0) {
                int valor = atoi(valor_campo[i]);
                if (reg_dados->year != valor) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "financialLoss") == 0) {
                float valor = atof(valor_campo[i]);
                if (reg_dados->financialLoss != valor) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "country") == 0) {
                if (strcmp(reg_dados->country, valor_campo[i]) != 0) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "attackType") == 0) {
                if (strcmp(reg_dados->attackType, valor_campo[i]) != 0) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "targetIndustry") == 0) {
                if (strcmp(reg_dados->targetIndustry, valor_campo[i]) != 0) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "defenseMechanism") == 0) {
                if (strcmp(reg_dados->defenseMechanism, valor_campo[i]) != 0) {
                    match = false;
                    break;
                }
            }
        }
        
        // Se encontrou um match, remove o registro
        if (match) {
            // Salva a posição atual para restaurar depois
            long long posicao_atual = ftell(bin);
            
            // Posiciona no início do registro para escrever o campo removido
            fseek(bin, posicao_registro, SEEK_SET);
            
            // Marca como removido
            char removido = '1';
            fwrite(&removido, sizeof(char), 1, bin);
            
            // Pula o tamanhoRegistro
            fseek(bin, sizeof(int), SEEK_CUR);
            
            // Escreve o campo prox com o valor atual do topo (First Fit)
            fwrite(&header->topo, sizeof(long long int), 1, bin);
            
            // Atualiza o topo para apontar para este registro recém removido
            header->topo = posicao_registro;
            
            // Atualiza contadores no header
            header->nroRegArq--;
            header->nroRegRem++;
            
            // Atualiza apenas os dados da lista no arquivo (sem marcar como consistente)
            atualiza_dados_header(header, bin);
            
            // Restaura a posição para continuar a leitura
            fseek(bin, posicao_atual, SEEK_SET);
        }
        
        // Libera memória do registro
        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados->targetIndustry);
        free(reg_dados->defenseMechanism);
        free(reg_dados);
    }
}

/*
 * Função principal que coordena a remoção de registros
 * Parâmetros:
 *  nome_arquivo - nome do arquivo binário
 *  num_buscas - número de buscas a serem realizadas
 */
void chama_match_remove_bin(char *nome_arquivo, int num_buscas)
{
    // Abre arquivo binário em modo leitura/escrita
    FILE *bin = fopen(nome_arquivo, "r+b");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    // Lê o header do arquivo
    REG_HEADER *header = get_reg_header(bin);
    if (header == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    
    // Aloca estruturas para armazenar critérios de busca
    int *qtd_campos = (int *)malloc(num_buscas * sizeof(int));
    char ***nomes_campos = (char ***)malloc(num_buscas * sizeof(char **));
    char ***valores_campos = (char ***)malloc(num_buscas * sizeof(char **));
    
    // Lê todos os critérios de busca
    for (int i = 0; i < num_buscas; i++) {
        scanf(" %d", &qtd_campos[i]);
        
        // Aloca arrays para nomes e valores dos campos
        nomes_campos[i] = (char **)malloc(qtd_campos[i] * sizeof(char *));
        valores_campos[i] = (char **)malloc(qtd_campos[i] * sizeof(char *));
        
        for (int j = 0; j < qtd_campos[i]; j++) {
            nomes_campos[i][j] = (char *)malloc(50 * sizeof(char));
            valores_campos[i][j] = (char *)malloc(100 * sizeof(char));
            
            scanf("%s", nomes_campos[i][j]);
            
            // Verifica o tipo de campo para decidir como ler o valor
            if (strcmp(nomes_campos[i][j], "idAttack") == 0 ||
                strcmp(nomes_campos[i][j], "year") == 0 ||
                strcmp(nomes_campos[i][j], "financialLoss") == 0) {
                // Campos numéricos
                scanf("%s", valores_campos[i][j]);
            }
            else {
                // Campos string, usar scan_quote_string
                scan_quote_string(valores_campos[i][j]);
            }
        }
    }
    
    // Realiza todas as remoções
    for (int i = 0; i < num_buscas; i++) {
        match_registro_remove(bin, header, qtd_campos[i], nomes_campos[i], valores_campos[i]);
    }
    
    // Marca como consistente após todas as operações
    atualiza_header_removido(header, bin);
    
    // Libera toda a memória alocada
    for (int i = 0; i < num_buscas; i++) {
        for (int j = 0; j < qtd_campos[i]; j++) {
            free(nomes_campos[i][j]);
            free(valores_campos[i][j]);
        }
        free(nomes_campos[i]);
        free(valores_campos[i]);
    }
    
    free(qtd_campos);
    free(nomes_campos);
    free(valores_campos);
    free(header);

    fclose(bin);
    binarioNaTela(nome_arquivo);
} 