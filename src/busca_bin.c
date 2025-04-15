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
#include <ctype.h>
#include "parse_csv.h"
#include "csv_to_bin.h"
#include "imprime_bin.h"

void scan_quote_string(char *str); // Função fornecida pelo monitor

/*
 * Busca registros que correspondem a critérios de busca especificados pelo usuário, a função lê os registros
 * do arquivo binário e compara os campos especificados com os valores fornecidos. Imprime os registros encontrados ou, em
 * caso de não encontrar nenhum, imprime uma mensagem padrão.
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  num_campos - número de campos a serem buscados
 *  nome_campo - array de strings com os nomes dos campos
 *  valor_campo - array de strings com os valores dos campos
 */
void match_registro(FILE *bin, int num_campos, char **nome_campo, char **valor_campo)
{
    // Pula o cabeçalho do binário
    fseek(bin, 276, SEEK_SET);

    // Inicializa ponteiro para o registro e variáveis de controle
    REG_DADOS *reg_dados = get_registro(bin); 
    bool match = false;
    bool found = false;

    // Processa cada registro do arquivo
    while (ftell(bin) != -1 && reg_dados != NULL)
    {
        // Assume que encontrou um match até provar o contrário
        match = true;

        // Verificar cada campo do critério de busca
        for (int i = 0; i < num_campos; i++)
        {
            if (strcmp(nome_campo[i], "idAttack") == 0)
            {
                int valor = atoi(valor_campo[i]);
                if (reg_dados->idAttack != valor)
                {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "year") == 0)
            {
                int valor = atoi(valor_campo[i]);
                if (reg_dados->year != valor)
                {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "financialLoss") == 0)
            {
                float valor = atof(valor_campo[i]);
                if (reg_dados->financialLoss != valor)
                {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "country") == 0)
            {
                if (strcmp(reg_dados->country, valor_campo[i]) != 0)
                {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "attackType") == 0)
            {
                if (strcmp(reg_dados->attackType, valor_campo[i]) != 0)
                {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "targetIndustry") == 0)
            {
                if (strcmp(reg_dados->targetIndustry, valor_campo[i]) != 0)
                {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo[i], "defenseMechanism") == 0)
            {
                if (strcmp(reg_dados->defenseMechanism, valor_campo[i]) != 0)
                {
                    match = false;
                    break;
                }
            }
        }

        // Se todos os critérios de busca foram atendidos, imprime o registro
        if (match)
        {
            imprime_registro_bin(reg_dados);
            found = true; // Encontrou um registro que corresponde a todos os critérios
        }

        reg_dados = get_registro(bin);
    }

    // Se não encontrou nenhum registro correspondente, imprime mensagem padrão
    if (!found)
    {
        printf("Registro inexistente.\n\n");
    }

    printf("**********\n");
    return;
}

/*
 * Lê critérios de busca do usuário e chama a função de busca no arquivo binário aplicando cada filtro especificado.
 * Parâmetro:
 *  nome_arquivo - nome do arquivo binário
 *  num_buscas - número de buscas a serem realizadas
 */
void chama_match_bin(char *nome_arquivo, int num_buscas)
{
    // Abre arquivo binário
    FILE *bin = fopen(nome_arquivo, "rb");
    if (bin == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Aloca estruturas para armazenar critérios de busca
    int *qtd_campos = (int *)malloc(num_buscas * sizeof(int));
    char ***nomes_campos = (char ***)malloc(num_buscas * sizeof(char **));
    char ***valores_campos = (char ***)malloc(num_buscas * sizeof(char **));

    // Ler todos os critérios de busca primeiro
    for (int i = 0; i < num_buscas; i++)
    {
        scanf(" %d", &qtd_campos[i]);

        // Aloca arrays para nomes e valores dos campos
        nomes_campos[i] = (char **)malloc(qtd_campos[i] * sizeof(char *));
        valores_campos[i] = (char **)malloc(qtd_campos[i] * sizeof(char *));

        for (int j = 0; j < qtd_campos[i]; j++)
        {
            nomes_campos[i][j] = (char *)malloc(50 * sizeof(char));
            valores_campos[i][j] = (char *)malloc(100 * sizeof(char));

            scanf("%s", nomes_campos[i][j]);

            // Verificar o tipo de campo para decidir como ler o valor
            if (strcmp(nomes_campos[i][j], "idAttack") == 0 ||
                strcmp(nomes_campos[i][j], "year") == 0 ||
                strcmp(nomes_campos[i][j], "financialLoss") == 0)
            {
                // Campos numéricos, ler sem aspas
                scanf("%s", valores_campos[i][j]);
            }
            else
            {
                // Campos string, usar scan_quote_string
                scan_quote_string(valores_campos[i][j]);
            }
        }
    }

    // Agora realizar todas as buscas chamando a função match_registro
    for (int i = 0; i < num_buscas; i++)
    {
        match_registro(bin, qtd_campos[i], nomes_campos[i], valores_campos[i]);
    }

    // Liberar toda a memória alocada
    for (int i = 0; i < num_buscas; i++)
    {
        for (int j = 0; j < qtd_campos[i]; j++)
        {
            free(nomes_campos[i][j]);
            free(valores_campos[i][j]);
        }
        free(nomes_campos[i]);
        free(valores_campos[i]);
    }

    free(qtd_campos);
    free(nomes_campos);
    free(valores_campos);

    // Fecha o arquivo binário
    fclose(bin);
    return;
}


// Função fornecida pelo monitor
void scan_quote_string(char *str)
{

    /*
     *	Use essa função para ler um campo string delimitado entre aspas (").
     *	Chame ela na hora que for ler tal campo. Por exemplo:
     *
     *	A entrada está da seguinte forma:
     *		nomeDoCampo "MARIA DA SILVA"
     *
     *	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
     *		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
     *		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
     *
     */

    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n')
    { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // ignorar aspas fechando
    }
    else if (R != EOF)
    { // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}