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
#include "insere_reg_bin.h"
#include "csv_to_bin.h"
#include "busca_bin.h"

#define LIXO_STRING "$"
#define LIXO_INT -1
#define LIXO_FLOAT -1.0
#define DELIM_CAMPO '|'

/*
 * Atualiza apenas os dados do header (usado durante as operações)
 * Parâmetros:
 *  header - ponteiro para a estrutura do header
 *  bin - ponteiro para o arquivo binário
 */
void atualiza_dados_header_insercao(REG_HEADER *header, FILE* bin) {
    fseek(bin, 0, SEEK_SET);
    
    // Não marca como consistente durante operações
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
 * Atualiza o header, marcando como consistente (chamada no final)
 * Parâmetros:
 *  header - ponteiro para a estrutura do header
 *  bin - ponteiro para o arquivo binário
 */
void atualiza_header_inserido(REG_HEADER *header, FILE* bin) {
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
 * Cria um registro de dados a partir da entrada padrão
 * Retorno:
 *  ponteiro para o registro criado ou NULL em caso de erro
 */
REG_DADOS *cria_registro_para_insercao(void) {
    REG_DADOS *reg = (REG_DADOS *)calloc(1, sizeof(REG_DADOS));
    if (reg == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    char buffer[1000];
    
    // Lê idAttack
    if (scanf("%s", buffer) != 1) {
        free(reg);
        return NULL;
    }
    if (strcmp(buffer, "NULO") == 0) {
        reg->idAttack = LIXO_INT;
    } else {
        reg->idAttack = atoi(buffer);
    }
    
    // Lê year
    if (scanf("%s", buffer) != 1) {
        free(reg);
        return NULL;
    }
    if (strcmp(buffer, "NULO") == 0) {
        reg->year = LIXO_INT;
    } else {
        reg->year = atoi(buffer);
    }
    
    // Lê financialLoss
    if (scanf("%s", buffer) != 1) {
        free(reg);
        return NULL;
    }
    if (strcmp(buffer, "NULO") == 0) {
        reg->financialLoss = LIXO_FLOAT;
    } else {
        reg->financialLoss = atof(buffer);
    }
    
    // Aloca espaço para campos de string
    reg->country = malloc(1000);
    reg->attackType = malloc(1000);
    reg->targetIndustry = malloc(1000);
    reg->defenseMechanism = malloc(1000);
    
    if (!reg->country || !reg->attackType || !reg->targetIndustry || !reg->defenseMechanism) {
        printf("Falha no processamento do arquivo.\n");
        free(reg->country);
        free(reg->attackType);
        free(reg->targetIndustry);
        free(reg->defenseMechanism);
        free(reg);
        return NULL;
    }
    
    // Lê country usando scan_quote_string
    scan_quote_string(reg->country);
    
    // Lê attackType usando scan_quote_string
    scan_quote_string(reg->attackType);
    
    // Lê targetIndustry usando scan_quote_string
    scan_quote_string(reg->targetIndustry);
    
    // Lê defenseMechanism usando scan_quote_string
    scan_quote_string(reg->defenseMechanism);
    
    // Realoca para economizar memória
    reg->country = realloc(reg->country, strlen(reg->country) + 1);
    reg->attackType = realloc(reg->attackType, strlen(reg->attackType) + 1);
    reg->targetIndustry = realloc(reg->targetIndustry, strlen(reg->targetIndustry) + 1);
    reg->defenseMechanism = realloc(reg->defenseMechanism, strlen(reg->defenseMechanism) + 1);
    
    // Configura campos de controle
    reg->removido = '0';
    reg->prox = -1;
    
    // Calcula tamanho do registro
    int tamanho = 0;
   // tamanho += sizeof(char) + 1;          // Campo 'removido' 
    //tamanho += sizeof(int);           // Campo 'tamanhoRegistro'
    tamanho += sizeof(long long int); // Campo 'prox'
    tamanho += sizeof(int);           // Campo 'idAttack'

    if(reg->year != LIXO_INT) {
        tamanho += (sizeof(int));           // Campo 'year'
    }
    if(reg->financialLoss != LIXO_FLOAT) {
        tamanho += (sizeof(float));         // Campo 'financialLoss'
    }

    // Campos variáveis
    if (strlen(reg->country) > 0) {
        tamanho += strlen(reg->country) + 2; // +2 para código e delimitador
    }
    if (strlen(reg->attackType) > 0) {
        tamanho += strlen(reg->attackType) + 2;
    }
    if (strlen(reg->targetIndustry) > 0) {
        tamanho += strlen(reg->targetIndustry) + 2;
    }
    if (strlen(reg->defenseMechanism) > 0) {
        tamanho += strlen(reg->defenseMechanism) + 2;
    }
    
    reg->tamanhoRegistro = tamanho;
    
    return reg;
}

/*
 * Escreve um registro no arquivo binário
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  reg - ponteiro para o registro a ser escrito
 */
void escreve_registro_no_bin(FILE *bin, REG_DADOS *reg) {
    fwrite(&reg->removido, sizeof(char), 1, bin);
    fwrite(&reg->tamanhoRegistro, sizeof(int), 1, bin);
    fwrite(&reg->prox, sizeof(long long int), 1, bin);
    fwrite(&reg->idAttack, sizeof(int), 1, bin);
    fwrite(&reg->year, sizeof(int), 1, bin);
    fwrite(&reg->financialLoss, sizeof(float), 1, bin);

    // Escreve campos variáveis se não forem lixo
    if (strlen(reg->country) > 0) {
        fputc('1', bin);
        fwrite(reg->country, strlen(reg->country), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }

    if (strlen(reg->attackType) > 0) {
        fputc('2', bin);
        fwrite(reg->attackType, strlen(reg->attackType), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }

    if (strlen(reg->targetIndustry) > 0) {
        fputc('3', bin);
        fwrite(reg->targetIndustry, strlen(reg->targetIndustry), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }

    if (strlen(reg->defenseMechanism) > 0) {
        fputc('4', bin);
        fwrite(reg->defenseMechanism, strlen(reg->defenseMechanism), 1, bin);
        fputc(DELIM_CAMPO, bin);
    }
}

/*
 * Preenche com lixo '$' o espaço restante no registro
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  tamanho_lixo - quantidade de bytes a preencher com lixo
 */
void preenche_lixo(FILE *bin, int tamanho_lixo) {
    for (int i = 0; i < tamanho_lixo; i++) {
        fputc('$', bin);
    }
}

/*
 * Insere um registro usando estratégia First Fit
 * Parâmetros:
 *  bin - ponteiro para o arquivo binário
 *  header - ponteiro para a estrutura do header
 *  novo_registro - ponteiro para o registro a ser inserido
 */
void match_insere_bin(FILE *bin, REG_HEADER *header, REG_DADOS *novo_registro) {
    long long int posicao_anterior = -1;
    long long int posicao_atual = header->topo;
    
    // Procura primeiro espaço que cabe (First Fit)
    while (posicao_atual != -1) {
        fseek(bin, posicao_atual, SEEK_SET);
        
        // Lê informações do registro removido
        char removido;
        int tamanho_removido;
        long long int prox_removido;
        
        fread(&removido, sizeof(char), 1, bin);
        fread(&tamanho_removido, sizeof(int), 1, bin);
        fread(&prox_removido, sizeof(long long int), 1, bin);
        
        // Se cabe no espaço
        if (novo_registro->tamanhoRegistro <= tamanho_removido) {
            // Posiciona para escrever o novo registro
            fseek(bin, posicao_atual, SEEK_SET);
            
            // Salva o tamanho original e substitui temporariamente
            int tamanho_original = novo_registro->tamanhoRegistro;
            novo_registro->tamanhoRegistro = tamanho_removido; // Usa o tamanho do espaço removido
            
            // Escreve o novo registro
            escreve_registro_no_bin(bin, novo_registro);
            
            // Restaura o tamanho original
            novo_registro->tamanhoRegistro = tamanho_original;
            
            // Preenche o resto com lixo se sobrou espaço
            int lixo_restante = tamanho_removido - tamanho_original;
            if (lixo_restante > 0) {
                preenche_lixo(bin, lixo_restante);
            }
            
            // Atualiza a lista de removidos
            if (posicao_anterior == -1) {
                // Era o primeiro da lista
                header->topo = prox_removido;
            } else {
                // Atualiza o anterior para pular este registro
                fseek(bin, posicao_anterior + sizeof(char) + sizeof(int), SEEK_SET);
                fwrite(&prox_removido, sizeof(long long int), 1, bin);
            }
            
            header->nroRegArq++;
            header->nroRegRem--;
            
            // Atualiza dados no header sem marcar como consistente
            atualiza_dados_header_insercao(header, bin);
            return;
        }
        
        posicao_anterior = posicao_atual;
        posicao_atual = prox_removido;
    }
    
    // Não encontrou espaço, insere no final
    fseek(bin, header->proxByteOffset, SEEK_SET);
    escreve_registro_no_bin(bin, novo_registro);
    
    header->proxByteOffset = ftell(bin);
    header->nroRegArq++;
    
    // Atualiza dados no header sem marcar como consistente
    atualiza_dados_header_insercao(header, bin);
}

/*
 * Função principal que coordena a inserção de registros
 * Parâmetros:
 *  nome_arquivo - nome do arquivo binário
 *  num_insercoes - número de inserções a serem realizadas
 */
void chama_match_insere_bin(char *nome_arquivo, int num_insercoes) {
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
    
    // Realiza todas as inserções
    for (int i = 0; i < num_insercoes; i++) {
        REG_DADOS *novo_registro = cria_registro_para_insercao();
        if (novo_registro == NULL) {
            continue;
        }
        
        match_insere_bin(bin, header, novo_registro);
        
        // Libera memória do registro
        free(novo_registro->country);
        free(novo_registro->attackType);
        free(novo_registro->targetIndustry);
        free(novo_registro->defenseMechanism);
        free(novo_registro);
    }
    
    // Marca como consistente após todas as operações
    atualiza_header_inserido(header, bin);
    
    free(header);
    
    // Fecha o arquivo
    fclose(bin);
    
    // Mostra o arquivo binário na tela
    binarioNaTela(nome_arquivo);
}
