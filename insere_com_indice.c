/*******************************************************************************
 *                                                                             *
 *                     SCC0215 - Organização de Arquivos                       *
 *                                                                             *
 *                   Funcionalidade 10 - Inserção com Índice                   *
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
#include "insere_com_indice.h"
#include "csv_to_bin.h"
#include "busca_bin.h"
#include "arvore_b.h"
#include "imprime_bin.h"
#include "insere_reg_bin.h"

#define LIXO_STRING "$"
#define LIXO_INT -1
#define LIXO_FLOAT -1.0

/*
 * Executa inserções de registros com reaproveitamento de espaços removidos e
 * atualização automática do índice árvore-B. Implementa estratégia First Fit
 * para reutilização de espaços e mantém consistência entre arquivo de dados e índice.
 * Parâmetros:
 *  nome_arquivo_dados - nome do arquivo de dados
 *  nome_arquivo_indice - nome do arquivo de índice
 *  num_insercoes - número de inserções a serem realizadas
 * Retorno:
 *  true se todas as inserções foram realizadas com sucesso, false caso contrário
 */
bool executa_insercoes_com_indice(const char* nome_arquivo_dados, const char* nome_arquivo_indice, int num_insercoes) {
    // Abre arquivo de dados
    FILE *arquivo_dados = fopen(nome_arquivo_dados, "r+b");
    if (arquivo_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return false;
    }
    
    // Verifica consistência do arquivo de dados
    REG_HEADER *header = get_reg_header(arquivo_dados);
    if (header == NULL || header->status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo_dados);
        if (header) free(header);
        return false;
    }
    
    // Abre arquivo de índice
    FILE *arquivo_indice = fopen(nome_arquivo_indice, "r+b");
    if (arquivo_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo_dados);
        free(header);
        return false;
    }
    
    // Verifica consistência do arquivo de índice
    CABECALHO_INDICE cab_indice;
    le_cabecalho_indice(arquivo_indice, &cab_indice);
    if (cab_indice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo_dados);
        fclose(arquivo_indice);
        free(header);
        return false;
    }
    
    // Marca arquivos como inconsistentes durante as operações
    header->status = '0';
    fseek(arquivo_dados, 0, SEEK_SET);
    fwrite(&header->status, sizeof(char), 1, arquivo_dados);
    
    // Processa cada inserção
    for (int i = 0; i < num_insercoes; i++) {
        // Lê dados do novo registro da entrada
        REG_DADOS *reg = cria_registro_para_insercao();
        if (reg == NULL) {
            printf("Falha no processamento do arquivo.\n");
            header->status = '1';
            fseek(arquivo_dados, 0, SEEK_SET);
            fwrite(&header->status, sizeof(char), 1, arquivo_dados);
            fclose(arquivo_dados);
            fclose(arquivo_indice);
            free(header);
            return false;
        }
        
        // Insere no arquivo de dados com reaproveitamento de espaço (First Fit)
        // e retorna o offset onde o registro foi inserido
        long long int offset_registro = match_insere_bin_com_offset(arquivo_dados, header, reg);
        
        // Atualiza o índice árvore-B com nova chave e ponteiro
        // Mantém consistência entre estruturas de dados e índice
        if (!insere_arvore_b(arquivo_indice, reg->idAttack, offset_registro)) {
            printf("Falha no processamento do arquivo.\n");
            free(reg);
            header->status = '1';
            fseek(arquivo_dados, 0, SEEK_SET);
            fwrite(&header->status, sizeof(char), 1, arquivo_dados);
            fclose(arquivo_dados);
            fclose(arquivo_indice);
            free(header);
            return false;
        }
        
        free(reg);
    }
    
    // Marca como consistente após todas as inserções
    header->status = '1';
    fseek(arquivo_dados, 0, SEEK_SET);
    fwrite(&header->status, sizeof(char), 1, arquivo_dados);
    fwrite(&header->topo, sizeof(long long int), 1, arquivo_dados);
    fwrite(&header->proxByteOffset, sizeof(long long int), 1, arquivo_dados);
    fwrite(&header->nroRegArq, sizeof(int), 1, arquivo_dados);
    fwrite(&header->nroRegRem, sizeof(int), 1, arquivo_dados);
    
    fclose(arquivo_dados);
    fclose(arquivo_indice);
    free(header);
    
    // Exibe os resultados usando binarioNaTela
    binarioNaTela((char*)nome_arquivo_dados);
    binarioNaTela((char*)nome_arquivo_indice);
    
    return true;
} 