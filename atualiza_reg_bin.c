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
#include "atualiza_reg_bin.h"
#include "csv_to_bin.h"
#include "insere_reg_bin.h"
#include "imprime_bin.h"
#include "remove_reg_bin.h"
#include "busca_bin.h"

#define LIXO_STRING "$"
#define LIXO_INT -1
#define LIXO_FLOAT -1.0
#define DELIM_CAMPO '|'

/*
 * Atualiza o header marcando como consistente (chamada no final)
 * Parâmetros:
 *  header - ponteiro para a estrutura do header
 *  bin - ponteiro para o arquivo binário
 */
void atualiza_header_atualizacao_final(REG_HEADER *header, FILE* bin) {
    fseek(bin, 0, SEEK_SET);
    
    // Marca como consistente
    char status = '1';
    fwrite(&status, sizeof(char), 1, bin);
    
    // Escreve o topo atualizado
    fwrite(&header->topo, sizeof(long long int), 1, bin);
    
    // Escreve o próximo byte offset
    fwrite(&header->proxByteOffset, sizeof(long long int), 1, bin);
    
    // Escreve número de registros
    fwrite(&header->nroRegArq, sizeof(int), 1, bin);
    
    // Escreve número de registros removidos
    fwrite(&header->nroRegRem, sizeof(int), 1, bin);
}

/*
 * Aplica as atualizações a um registro criando uma nova versão
 * Parâmetros:
 *  reg_original - ponteiro para o registro original
 *  num_campos_atualizacao - número de campos a atualizar
 *  nomes_campos_atualizacao - array com nomes dos campos
 *  valores_campos_atualizacao - array com valores dos campos
 * Retorno:
 *  ponteiro para o registro atualizado ou NULL em caso de erro
 */
REG_DADOS *aplica_atualizacao(REG_DADOS *reg_original, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao) {
    REG_DADOS *reg_atualizado = (REG_DADOS *)calloc(1, sizeof(REG_DADOS));
    if (reg_atualizado == NULL) {
        return NULL;
    }
    
    // Copia todos os dados originais
    reg_atualizado->removido = '0';
    reg_atualizado->prox = -1;
    reg_atualizado->idAttack = reg_original->idAttack;
    reg_atualizado->year = reg_original->year;
    reg_atualizado->financialLoss = reg_original->financialLoss;
    
    // Aloca e copia campos de string originais
    reg_atualizado->country = malloc(strlen(reg_original->country) + 1);
    reg_atualizado->attackType = malloc(strlen(reg_original->attackType) + 1);
    reg_atualizado->targetIndustry = malloc(strlen(reg_original->targetIndustry) + 1);
    reg_atualizado->defenseMechanism = malloc(strlen(reg_original->defenseMechanism) + 1);
    
    if (!reg_atualizado->country || !reg_atualizado->attackType || 
        !reg_atualizado->targetIndustry || !reg_atualizado->defenseMechanism) {
        free(reg_atualizado->country);
        free(reg_atualizado->attackType);
        free(reg_atualizado->targetIndustry);
        free(reg_atualizado->defenseMechanism);
        free(reg_atualizado);
        return NULL;
    }
    
    strcpy(reg_atualizado->country, reg_original->country);
    strcpy(reg_atualizado->attackType, reg_original->attackType);
    strcpy(reg_atualizado->targetIndustry, reg_original->targetIndustry);
    strcpy(reg_atualizado->defenseMechanism, reg_original->defenseMechanism);
    
    // Aplica as atualizações
    for (int i = 0; i < num_campos_atualizacao; i++) {
        char *nome_campo = nomes_campos_atualizacao[i];
        char *valor_campo = valores_campos_atualizacao[i];
        
        if (strcmp(nome_campo, "idAttack") == 0) {
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->idAttack = LIXO_INT;
            } else {
                reg_atualizado->idAttack = atoi(valor_campo);
            }
        }
        else if (strcmp(nome_campo, "year") == 0) {
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->year = LIXO_INT;
            } else {
                reg_atualizado->year = atoi(valor_campo);
            }
        }
        else if (strcmp(nome_campo, "financialLoss") == 0) {
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->financialLoss = LIXO_FLOAT;
            } else {
                reg_atualizado->financialLoss = atof(valor_campo);
            }
        }
        else if (strcmp(nome_campo, "country") == 0) {
            free(reg_atualizado->country);
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->country = malloc(1);
                strcpy(reg_atualizado->country, "");
            } else {
                reg_atualizado->country = malloc(strlen(valor_campo) + 1);
                strcpy(reg_atualizado->country, valor_campo);
            }
        }
        else if (strcmp(nome_campo, "attackType") == 0) {
            free(reg_atualizado->attackType);
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->attackType = malloc(1);
                strcpy(reg_atualizado->attackType, "");
            } else {
                reg_atualizado->attackType = malloc(strlen(valor_campo) + 1);
                strcpy(reg_atualizado->attackType, valor_campo);
            }
        }
        else if (strcmp(nome_campo, "targetIndustry") == 0) {
            free(reg_atualizado->targetIndustry);
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->targetIndustry = malloc(1);
                strcpy(reg_atualizado->targetIndustry, "");
            } else {
                reg_atualizado->targetIndustry = malloc(strlen(valor_campo) + 1);
                strcpy(reg_atualizado->targetIndustry, valor_campo);
            }
        }
        else if (strcmp(nome_campo, "defenseMechanism") == 0) {
            free(reg_atualizado->defenseMechanism);
            if (strcmp(valor_campo, "NULO") == 0) {
                reg_atualizado->defenseMechanism = malloc(1);
                strcpy(reg_atualizado->defenseMechanism, "");
            } else {
                reg_atualizado->defenseMechanism = malloc(strlen(valor_campo) + 1);
                strcpy(reg_atualizado->defenseMechanism, valor_campo);
            }
        }
    }
    
    // Calcula tamanho do registro atualizado
    reg_atualizado->tamanhoRegistro = calcula_tamanho_registro_atualizado(reg_atualizado);
    
    return reg_atualizado;
}

/*
 * Calcula o tamanho do registro atualizado
 * Parâmetro:
 *  reg - ponteiro para o registro
 * Retorno:
 *  tamanho em bytes do registro
 */
int calcula_tamanho_registro_atualizado(REG_DADOS *reg) {
    int tamanho = 0;
    //tamanho += sizeof(int);           // tamanhoRegistro
    tamanho += sizeof(long long int); // prox
    tamanho += sizeof(int);           // idAttack
    
    if (reg->year != LIXO_INT) {
        tamanho += sizeof(int);       // year
    }
    if (reg->financialLoss != LIXO_FLOAT) {
        tamanho += sizeof(float);     // financialLoss
    }
    
    // Campos variáveis
    if (strcmp(reg->country, "") != 0) {
        tamanho += strlen(reg->country) + 2; // +2 para código e delimitador
    }
    if (strcmp(reg->attackType, "") != 0) {
        tamanho += strlen(reg->attackType) + 2;
    }
    if (strcmp(reg->targetIndustry, "") != 0) {
        tamanho += strlen(reg->targetIndustry) + 2;
    }
    if (strcmp(reg->defenseMechanism, "") != 0) {
        tamanho += strlen(reg->defenseMechanism) + 2;
    }
    
    return tamanho;
}

/*
 * Escreve registro atualizado no local do registro original, preenchendo com lixo se necessário
 * Reutiliza a função preenche_lixo do insere_reg_bin.c
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  reg_atualizado - ponteiro para o registro atualizado
 *  tamanho_original - tamanho original do registro
 */
void escreve_registro_atualizado_no_local(FILE *bin, REG_DADOS *reg_atualizado, int tamanho_original) {
    // Escreve o registro atualizado
    fwrite(&reg_atualizado->removido, sizeof(char), 1, bin);
    fwrite(&reg_atualizado->tamanhoRegistro, sizeof(int), 1, bin);
    fwrite(&reg_atualizado->prox, sizeof(long long int), 1, bin);
    fwrite(&reg_atualizado->idAttack, sizeof(int), 1, bin);
    fwrite(&reg_atualizado->year, sizeof(int), 1, bin);
    fwrite(&reg_atualizado->financialLoss, sizeof(float), 1, bin);

    // Escreve campos variáveis se não forem lixo
    if (strlen(reg_atualizado->country) > 0) {
        fputc('1', bin);
        fwrite(reg_atualizado->country, strlen(reg_atualizado->country), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }

    if (strlen(reg_atualizado->attackType) > 0) {
        fputc('2', bin);
        fwrite(reg_atualizado->attackType, strlen(reg_atualizado->attackType), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }

    if (strlen(reg_atualizado->targetIndustry) > 0) {
        fputc('3', bin);
        fwrite(reg_atualizado->targetIndustry, strlen(reg_atualizado->targetIndustry), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }

    if (strlen(reg_atualizado->defenseMechanism) > 0) {
        fputc('4', bin);
        fwrite(reg_atualizado->defenseMechanism, strlen(reg_atualizado->defenseMechanism), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }
    
    // Calcula espaço restante e preenche com lixo usando função do insere_reg_bin.c
    int bytes_usados = reg_atualizado->tamanhoRegistro;
    int lixo_restante = tamanho_original - bytes_usados;
    
    if (lixo_restante > 0) {
        preenche_lixo(bin, lixo_restante);
    }
}

/*
 * Busca e atualiza registros que correspondem aos critérios especificados
 * Reutiliza a lógica de busca da funcionalidade 4 e a inserção da funcionalidade 5
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  header - ponteiro para a estrutura do header
 *  num_campos_busca - número de campos de busca
 *  nome_campo_busca - array com nomes dos campos de busca
 *  valor_campo_busca - array com valores dos campos de busca
 *  num_campos_atualizacao - número de campos a atualizar
 *  nomes_campos_atualizacao - array com nomes dos campos
 *  valores_campos_atualizacao - array com valores dos campos
 */
void match_registro_atualiza(FILE *bin, REG_HEADER *header, int num_campos_busca, char **nome_campo_busca, char **valor_campo_busca, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao) {
    // Posiciona após o cabeçalho
    fseek(bin, 276, SEEK_SET);
    
    // Processa cada registro do arquivo
    while (true) {
        long long posicao_registro = ftell(bin);
        REG_DADOS *reg_dados = get_registro(bin);
        
        if (reg_dados == NULL) {
            break; // Fim do arquivo
        }
        
        // Pula registros já removidos
        if (reg_dados->removido == '1') {
            free(reg_dados->country);
            free(reg_dados->attackType);
            free(reg_dados->targetIndustry);
            free(reg_dados->defenseMechanism);
            free(reg_dados);
            continue;
        }
        
        // Reutiliza a lógica de verificação de critérios da funcionalidade 4
        bool match = true;
        for (int i = 0; i < num_campos_busca; i++) {
            if (strcmp(nome_campo_busca[i], "idAttack") == 0) {
                int valor = atoi(valor_campo_busca[i]);
                if (reg_dados->idAttack != valor) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo_busca[i], "year") == 0) {
                int valor = atoi(valor_campo_busca[i]);
                if (reg_dados->year != valor) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo_busca[i], "financialLoss") == 0) {
                float valor = atof(valor_campo_busca[i]);
                if (reg_dados->financialLoss != valor) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo_busca[i], "country") == 0) {
                if (strcmp(reg_dados->country, valor_campo_busca[i]) != 0) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo_busca[i], "attackType") == 0) {
                if (strcmp(reg_dados->attackType, valor_campo_busca[i]) != 0) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo_busca[i], "targetIndustry") == 0) {
                if (strcmp(reg_dados->targetIndustry, valor_campo_busca[i]) != 0) {
                    match = false;
                    break;
                }
            }
            else if (strcmp(nome_campo_busca[i], "defenseMechanism") == 0) {
                if (strcmp(reg_dados->defenseMechanism, valor_campo_busca[i]) != 0) {
                    match = false;
                    break;
                }
            }
        }
        
        // Se encontrou um match, aplica a atualização
        if (match) {
            // Aplica as atualizações
            REG_DADOS *reg_atualizado = aplica_atualizacao(reg_dados, num_campos_atualizacao, nomes_campos_atualizacao, valores_campos_atualizacao);
            
            if (reg_atualizado != NULL) {
                int tamanho_original = reg_dados->tamanhoRegistro; 
                int tamanho_atualizado = reg_atualizado->tamanhoRegistro;
                
                if (tamanho_atualizado <= tamanho_original) {
                    // Atualização in-place: tamanho menor ou igual
                    fseek(bin, posicao_registro, SEEK_SET);
                    reg_atualizado->tamanhoRegistro = reg_dados->tamanhoRegistro; // Mantém tamanho original
                    escreve_registro_atualizado_no_local(bin, reg_atualizado, tamanho_original);
                } else {
                    // Caso o registro atualizado seja maior que o original:
                    // Remove logicamente e insere novo
                    fseek(bin, posicao_registro, SEEK_SET);
                    
                    // Marca como removido
                    char removido = '1';
                    fwrite(&removido, sizeof(char), 1, bin);
                    
                    // Pula o tamanhoRegistro
                    fseek(bin, sizeof(int), SEEK_CUR);
                    
                    // Escreve o campo prox com o valor atual do topo
                    fwrite(&header->topo, sizeof(long long int), 1, bin);
                    
                    // Atualiza o topo para apontar para este registro
                    header->topo = posicao_registro;
                    
                    // Atualiza contadores para refletir apenas a remoção
                    header->nroRegArq--;
                    header->nroRegRem++;
                    
                    // Insere o novo registro (match_insere_bin fará nroRegArq++, nroRegRem--)
                    match_insere_bin(bin, header, reg_atualizado);
                    
                    // Correção final: o resultado líquido deveria ser neutro, mas estamos +1/-1
                    // Corrigimos subtraindo 1 de nroRegArq e adicionando 1 a nroRegRem
                    header->nroRegArq--;
                    header->nroRegRem++;
                }
                
                // Libera memória do registro atualizado
                free(reg_atualizado->country);
                free(reg_atualizado->attackType);
                free(reg_atualizado->targetIndustry);
                free(reg_atualizado->defenseMechanism);
                free(reg_atualizado);
            }
            
            // Libera memória do registro original
            free(reg_dados->country);
            free(reg_dados->attackType);
            free(reg_dados->targetIndustry);
            free(reg_dados->defenseMechanism);
            free(reg_dados);
            
            // Para na primeira atualização encontrada para evitar problemas de posicionamento
            break;
        }
        
        // Libera memória do registro se não houve match
        free(reg_dados->country);
        free(reg_dados->attackType);
        free(reg_dados->targetIndustry);
        free(reg_dados->defenseMechanism);
        free(reg_dados);
    }
}

/*
 * Função principal que coordena a atualização de registros
 * Parâmetros:
 *  nome_arquivo - nome do arquivo binário
 *  num_atualizacoes - número de atualizações a realizar
 */
void chama_match_atualiza_bin(char *nome_arquivo, int num_atualizacoes) {
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
    
    // Verifica se arquivo está consistente
    if (header->status != '1') {
        printf("Falha no processamento do arquivo.\n");
        free(header);
        fclose(bin);
        return;
    }
    
    // Aloca estruturas para armazenar critérios de busca e atualização
    int *qtd_campos_busca = (int *)malloc(num_atualizacoes * sizeof(int));
    char ***nomes_campos_busca = (char ***)malloc(num_atualizacoes * sizeof(char **));
    char ***valores_campos_busca = (char ***)malloc(num_atualizacoes * sizeof(char **));
    
    int *qtd_campos_atualizacao = (int *)malloc(num_atualizacoes * sizeof(int));
    char ***nomes_campos_atualizacao = (char ***)malloc(num_atualizacoes * sizeof(char **));
    char ***valores_campos_atualizacao = (char ***)malloc(num_atualizacoes * sizeof(char **));
    
    // Lê todos os critérios de busca e atualização
    for (int i = 0; i < num_atualizacoes; i++) {
        // Lê critérios de busca
        scanf(" %d", &qtd_campos_busca[i]);
        
        nomes_campos_busca[i] = (char **)malloc(qtd_campos_busca[i] * sizeof(char *));
        valores_campos_busca[i] = (char **)malloc(qtd_campos_busca[i] * sizeof(char *));
        
        for (int j = 0; j < qtd_campos_busca[i]; j++) {
            nomes_campos_busca[i][j] = (char *)malloc(50 * sizeof(char));
            valores_campos_busca[i][j] = (char *)malloc(100 * sizeof(char));
            
            scanf("%s", nomes_campos_busca[i][j]);
            
            // Verifica o tipo de campo para decidir como ler o valor
            if (strcmp(nomes_campos_busca[i][j], "idAttack") == 0 ||
                strcmp(nomes_campos_busca[i][j], "year") == 0 ||
                strcmp(nomes_campos_busca[i][j], "financialLoss") == 0) {
                scanf("%s", valores_campos_busca[i][j]);
            } else {
                scan_quote_string(valores_campos_busca[i][j]);
            }
        }
        
        // Lê campos de atualização
        scanf(" %d", &qtd_campos_atualizacao[i]);
        
        nomes_campos_atualizacao[i] = (char **)malloc(qtd_campos_atualizacao[i] * sizeof(char *));
        valores_campos_atualizacao[i] = (char **)malloc(qtd_campos_atualizacao[i] * sizeof(char *));
        
        for (int j = 0; j < qtd_campos_atualizacao[i]; j++) {
            nomes_campos_atualizacao[i][j] = (char *)malloc(50 * sizeof(char));
            valores_campos_atualizacao[i][j] = (char *)malloc(100 * sizeof(char));
            
            scanf("%s", nomes_campos_atualizacao[i][j]);
            
            // Verifica o tipo de campo para decidir como ler o valor
            if (strcmp(nomes_campos_atualizacao[i][j], "idAttack") == 0 ||
                strcmp(nomes_campos_atualizacao[i][j], "year") == 0 ||
                strcmp(nomes_campos_atualizacao[i][j], "financialLoss") == 0) {
                scanf("%s", valores_campos_atualizacao[i][j]);
            } else {
                scan_quote_string(valores_campos_atualizacao[i][j]);
            }
        }
    }
    
    // Realiza todas as atualizações
    for (int i = 0; i < num_atualizacoes; i++) {
        match_registro_atualiza(bin, header, qtd_campos_busca[i], nomes_campos_busca[i], valores_campos_busca[i], qtd_campos_atualizacao[i], nomes_campos_atualizacao[i], valores_campos_atualizacao[i]);
    }
    
    // Marca como consistente após todas as operações
    atualiza_header_atualizacao_final(header, bin);
    
    // Libera toda a memória alocada
    for (int i = 0; i < num_atualizacoes; i++) {
        for (int j = 0; j < qtd_campos_busca[i]; j++) {
            free(nomes_campos_busca[i][j]);
            free(valores_campos_busca[i][j]);
        }
        free(nomes_campos_busca[i]);
        free(valores_campos_busca[i]);
        
        for (int j = 0; j < qtd_campos_atualizacao[i]; j++) {
            free(nomes_campos_atualizacao[i][j]);
            free(valores_campos_atualizacao[i][j]);
        }
        free(nomes_campos_atualizacao[i]);
        free(valores_campos_atualizacao[i]);
    }
    
    free(qtd_campos_busca);
    free(nomes_campos_busca);
    free(valores_campos_busca);
    free(qtd_campos_atualizacao);
    free(nomes_campos_atualizacao);
    free(valores_campos_atualizacao);
    free(header);
    
    // Fecha o arquivo
    fclose(bin);
    
    // Mostra o arquivo binário na tela
    binarioNaTela(nome_arquivo);
}

/*
 * Atualiza dados do header durante processo de atualização, marcando como inconsistente
 * Parâmetros:
 *  header - ponteiro para a estrutura do header
 *  bin - ponteiro para o arquivo binário
 */
void atualiza_dados_header_atualizacao(REG_HEADER *header, FILE* bin) {
    fseek(bin, 0, SEEK_SET);
    
    // Marca como inconsistente
    char status = '0';
    fwrite(&status, sizeof(char), 1, bin);
    
    // Escreve o topo atualizado
    fwrite(&header->topo, sizeof(long long int), 1, bin);
    
    // Escreve o próximo byte offset
    fwrite(&header->proxByteOffset, sizeof(long long int), 1, bin);
    
    // Escreve número de registros
    fwrite(&header->nroRegArq, sizeof(int), 1, bin);
    
    // Escreve número de registros removidos
    fwrite(&header->nroRegRem, sizeof(int), 1, bin);
}

/*
 * Verifica se um registro atende aos critérios de busca especificados
 * Parâmetros:
 *  reg_dados - ponteiro para o registro a verificar
 *  num_campos_busca - número de campos de busca
 *  nome_campo_busca - array com nomes dos campos de busca
 *  valor_campo_busca - array com valores dos campos de busca
 * Retorno:
 *  true se o registro atende a todos os critérios, false caso contrário
 */
bool verifica_criterio_busca(REG_DADOS *reg_dados, int num_campos_busca, char **nome_campo_busca, char **valor_campo_busca) {
    for (int i = 0; i < num_campos_busca; i++) {
        if (strcmp(nome_campo_busca[i], "idAttack") == 0) {
            int valor = atoi(valor_campo_busca[i]);
            if (reg_dados->idAttack != valor) {
                return false;
            }
        }
        else if (strcmp(nome_campo_busca[i], "year") == 0) {
            int valor = atoi(valor_campo_busca[i]);
            if (reg_dados->year != valor) {
                return false;
            }
        }
        else if (strcmp(nome_campo_busca[i], "financialLoss") == 0) {
            float valor = atof(valor_campo_busca[i]);
            if (reg_dados->financialLoss != valor) {
                return false;
            }
        }
        else if (strcmp(nome_campo_busca[i], "country") == 0) {
            if (strcmp(reg_dados->country, valor_campo_busca[i]) != 0) {
                return false;
            }
        }
        else if (strcmp(nome_campo_busca[i], "attackType") == 0) {
            if (strcmp(reg_dados->attackType, valor_campo_busca[i]) != 0) {
                return false;
            }
        }
        else if (strcmp(nome_campo_busca[i], "targetIndustry") == 0) {
            if (strcmp(reg_dados->targetIndustry, valor_campo_busca[i]) != 0) {
                return false;
            }
        }
        else if (strcmp(nome_campo_busca[i], "defenseMechanism") == 0) {
            if (strcmp(reg_dados->defenseMechanism, valor_campo_busca[i]) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

/*
 * Remove um registro logicamente, adicionando-o à lista ligada de removidos
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  header - ponteiro para a estrutura do header
 *  posicao_registro - posição do registro a ser removido
 */
void remove_registro_logicamente(FILE *bin, REG_HEADER *header, long long posicao_registro) {
    fseek(bin, posicao_registro, SEEK_SET);
    
    // Marca como removido
    char removido = '1';
    fwrite(&removido, sizeof(char), 1, bin);
    
    // Pula o tamanhoRegistro
    fseek(bin, sizeof(int), SEEK_CUR);
    
    // Escreve o campo prox com o valor atual do topo
    fwrite(&header->topo, sizeof(long long int), 1, bin);
    
    // Atualiza o topo para apontar para este registro
    header->topo = posicao_registro;
    
    // Atualiza contadores
    header->nroRegArq--;
    header->nroRegRem++;
}

/*
 * Insere registro sem atualizar header (função auxiliar para reuso)
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  header - ponteiro para a estrutura do header
 *  novo_registro - ponteiro para o novo registro
 */
void insere_sem_atualizar_header(FILE *bin, REG_HEADER *header, REG_DADOS *novo_registro)
{
    match_insere_bin(bin, header, novo_registro);
}

