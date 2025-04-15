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
    case 1:                                             // Converter CSV para binário
        scanf("%s %s", arquivo_entrada, arquivo_saida); // Lê p mp,e dps arquivps

        // Abre os arquivos
        FILE *csv = fopen(arquivo_entrada, "r");
        if (csv == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }
        FILE *bin = fopen(arquivo_saida, "wb");
        if (bin == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            fclose(csv);
            return 0;
        }
        escreve_registros_no_bin(csv, bin);
        binarioNaTela(arquivo_saida);
        break;

    case 2:                           // Imprimir arquivo binário
        scanf("%s", arquivo_entrada); // Lê o arquivo binário
        // Abre o arquivo para leitura
        FILE *bin2 = fopen(arquivo_entrada, "rb");
        if (bin2 == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }
        imprime_bin(bin2); // imprime o conteúdo do arquivo
        break;

    case 3: // Filtrar registros
        int num_buscas;
        scanf("%s", arquivo_entrada);
        scanf("%d", &num_buscas);
        chama_match_bin(arquivo_entrada, num_buscas); // execução das buscas
        break;

    default:
        printf("Funcionalidade inválida. Escolha 1, 2 ou 3.\n");
        return 0;
    }

    return 0;
}