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
#include "parse_csv.h"
#include "csv_to_bin.h"
#include "imprime_bin.h"
#include "busca_bin.h"
#include "remove_reg_bin.h"
#include "insere_reg_bin.h"
#include "atualiza_reg_bin.h"
#include "cria_indice.h"
#include "busca_multipla.h"
#include "insere_com_indice.h"
#include "atualiza_com_indice.h"

int main(void)
{
    int funcionalidade;
    scanf("%d", &funcionalidade);

    // Strings para armazenar nomes de arquivos
    char arquivo_entrada[100];
    char arquivo_saida[100];

    switch (funcionalidade) {
    // Funcionalidade 1: converter CSV para binário    
    case 1:             
        scanf("%s %s", arquivo_entrada, arquivo_saida);

        // Abre o CSV para leitura
        FILE *csv = fopen(arquivo_entrada, "r");
        if (csv == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Abre o arquivo binário para escrita
        FILE *bin = fopen(arquivo_saida, "wb");
        if (bin == NULL) {
            printf("Falha no processamento do arquivo.\n");
            fclose(csv);
            return 0;
        }
        escreve_registros_no_bin(csv, bin); // O binário é fechado dentro da funçaõ para que possa chamar binarioNaTela
        binarioNaTela(arquivo_saida); // O binário é fechado dentro da função
        break;

    // Funcionalidade 2: imprimir registros do binário
    case 2:
        scanf("%s", arquivo_entrada);

        // Abre o binário para leitura
        FILE *bin2 = fopen(arquivo_entrada, "rb");
        if (bin2 == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }
        imprime_bin(bin2); // O binário é fechado dentro da função
        break;
        
    // Funcionalidade 3: filtrar registros
    case 3: 
        int num_buscas;
        scanf("%s", arquivo_entrada);
        scanf("%d", &num_buscas);
        chama_match_bin(arquivo_entrada, num_buscas); // O binário é fechado dentro da função
        break;

    // Funcionalidade 4: remoção lógica de registros
    case 4:
        int num_remocoes;
        scanf("%s", arquivo_entrada);
        scanf("%d", &num_remocoes);
        chama_match_remove_bin(arquivo_entrada, num_remocoes);
        break;

    // Funcionalidade 5: inserção de registros
    case 5:
        int num_insercoes;
        scanf("%s", arquivo_entrada);
        scanf("%d", &num_insercoes);
        chama_match_insere_bin(arquivo_entrada, num_insercoes);
        break;

    // Funcionalidade 6: atualização de registros
    case 6:
        int num_atualizacoes;
        scanf("%s", arquivo_entrada);
        scanf("%d", &num_atualizacoes);
        chama_match_atualiza_bin(arquivo_entrada, num_atualizacoes);
        break;

    // Funcionalidade 7: criar índice árvore-B
    case 7:
        scanf("%s %s", arquivo_entrada, arquivo_saida);
        if (!cria_indice_arvore_b(arquivo_entrada, arquivo_saida)) {
            return 0;
        }
        break;

    // Funcionalidade 8: busca múltipla
    case 8: {
        char nome_arquivo_dados[100];
        char nome_arquivo_indice[100];
        int num_buscas;
        
        scanf("%s %s %d", nome_arquivo_dados, nome_arquivo_indice, &num_buscas);
        
        if (!executa_busca_multipla(nome_arquivo_dados, nome_arquivo_indice, num_buscas)) {
            printf("Falha no processamento do arquivo.\n");
        }
        break;
    }

    // Funcionalidade 10: inserção com índice
    case 10: {
        char nome_arquivo_dados[100];
        char nome_arquivo_indice[100];
        int num_insercoes;
        
        scanf("%s %s %d", nome_arquivo_dados, nome_arquivo_indice, &num_insercoes);
        
        if (!executa_insercoes_com_indice(nome_arquivo_dados, nome_arquivo_indice, num_insercoes)) {
            printf("Falha no processamento do arquivo.\n");
        }
        break;
    }

    // Funcionalidade 11: atualização com índice
    case 11: {
        char nome_arquivo_dados[100];
        char nome_arquivo_indice[100];
        int num_atualizacoes;
        
        scanf("%s %s %d", nome_arquivo_dados, nome_arquivo_indice, &num_atualizacoes);
        
        if (!executa_atualizacoes_com_indice(nome_arquivo_dados, nome_arquivo_indice, num_atualizacoes)) {
            printf("Falha no processamento do arquivo.\n");
        }
        break;
    }

    default:
        printf("Funcionalidade inválida. Escolha 1, 2, 3, 4, 5, 6, 7, 8, 10 ou 11.\n");
        return 0;
    }

    return 0;
}