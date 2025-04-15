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

int main(void)
{
    int funcionalidade;
    scanf("%d", &funcionalidade);

    // Strings para armazenar nomes de arquivos
    char arquivo_entrada[100];
    char arquivo_saida[100];

    switch (funcionalidade)
    {
    // Funcionalidade 1: converter CSV para binário    
    case 1:             
        scanf("%s %s", arquivo_entrada, arquivo_saida);

        // Abre o CSV para leitura
        FILE *csv = fopen(arquivo_entrada, "r");
        if (csv == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Abre o arquivo binário para escrita
        FILE *bin = fopen(arquivo_saida, "wb");
        if (bin == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            fclose(csv);
            return 0;
        }
        escreve_registros_no_bin(csv, bin); // O binário é fechado dentro da funçaõ para que possa chamar binarioNaTela
        binarioNaTela(arquivo_saida); // O binário é fechado dentro da função
        break;

    // Funcionalidade 1: imprimir registros do binário
    case 2:
        scanf("%s", arquivo_entrada);

        // Abre o binário para leitura
        FILE *bin2 = fopen(arquivo_entrada, "rb");
        if (bin2 == NULL)
        {
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

    default:
        printf("Funcionalidade inválida. Escolha 1, 2 ou 3.\n");
        return 0;
    }

    return 0;
}