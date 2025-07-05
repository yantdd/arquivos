/*******************************************************************************
 *                                                                             *
 *                     SCC0215 - Organização de Arquivos                       *
 *                                                                             *
 *                    Funcionalidade 7 - Criar Índice                          *
 *                                                                             *
 * Professora: Cristina Dutra de Aguiar                                        *
 *                                                                             *
 * Aluno: Yan Trindade Meireles - 13680035                                     *
 *                                                                             *
 * Aluno: Rafael Perez Carmanhani - 15485420                                   *
 *                                                                             *
 *******************************************************************************/

#include "cria_indice.h"
#include <string.h>

/*
 * Verifica se um arquivo existe e está acessível para leitura.
 * Parâmetro:
 *  nome_arquivo - nome do arquivo a ser verificado
 * Retorno:
 *  true se o arquivo existe e pode ser aberto, false caso contrário
 */
bool arquivo_existe(char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return false;
    }
    fclose(arquivo);
    return true;
}

/*
 * Lê apenas os campos essenciais de um registro para criação do índice.
 * Esta função é otimizada para ler apenas o que é necessário para o índice (idAttack),
 * evitando carregar dados desnecessários como strings.
 * Parâmetros:
 *  arquivo_dados - ponteiro para o arquivo de dados
 *  offset - posição do registro no arquivo
 * Retorno:
 *  ponteiro para o registro com campos básicos preenchidos ou NULL em caso de erro
 */
REG_DADOS* le_registro_dados(FILE *arquivo_dados, long long int offset) {
    fseek(arquivo_dados, offset, SEEK_SET);
    
    REG_DADOS *reg = malloc(sizeof(REG_DADOS));
    if (reg == NULL) return NULL;
    
    // Lê apenas os campos fixos necessários para o índice
    if (fread(&reg->removido, sizeof(char), 1, arquivo_dados) != 1) {
        free(reg);
        return NULL;
    }
    
    if (fread(&reg->tamanhoRegistro, sizeof(int), 1, arquivo_dados) != 1) {
        free(reg);
        return NULL;
    }
    
    if (fread(&reg->prox, sizeof(long long int), 1, arquivo_dados) != 1) {
        free(reg);
        return NULL;
    }
    
    if (fread(&reg->idAttack, sizeof(int), 1, arquivo_dados) != 1) {
        free(reg);
        return NULL;
    }
    
    // Inicializa os campos de string como NULL para evitar problemas na liberação
    reg->country = NULL;
    reg->attackType = NULL;
    reg->targetIndustry = NULL;
    reg->defenseMechanism = NULL;
    
    return reg;
}

/*
 * Cria um arquivo de índice árvore-B a partir de um arquivo de dados existente.
 * Percorre todos os registros não removidos do arquivo de dados e insere suas chaves
 * (idAttack) e ponteiros (offset) na árvore-B. O índice permite busca otimizada O(log n).
 * Parâmetros:
 *  nome_arquivo_dados - nome do arquivo de dados origem
 *  nome_arquivo_indice - nome do arquivo de índice a ser criado
 * Retorno:
 *  true se o índice foi criado com sucesso, false caso contrário
 */
bool cria_indice_arvore_b(char *nome_arquivo_dados, char *nome_arquivo_indice) {
    // Verifica se o arquivo de dados existe
    if (!arquivo_existe(nome_arquivo_dados)) {
        printf("Falha no processamento do arquivo.\n");
        return false;
    }
    
    // Abre o arquivo de dados para leitura
    FILE *arquivo_dados = fopen(nome_arquivo_dados, "rb");
    if (arquivo_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return false;
    }
    
    // Lê o cabeçalho do arquivo de dados
    REG_HEADER *header = get_reg_header(arquivo_dados);
    if (header == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo_dados);
        return false;
    }
    
    // Verifica se o arquivo está consistente
    if (header->status != '1') {
        printf("Falha no processamento do arquivo.\n");
        free(header);
        fclose(arquivo_dados);
        return false;
    }
    
    // Cria o arquivo de índice vazio
    if (!cria_arquivo_indice(nome_arquivo_indice)) {
        printf("Falha no processamento do arquivo.\n");
        free(header);
        fclose(arquivo_dados);
        return false;
    }
    
    // Abre o arquivo de índice para escrita
    FILE *arquivo_indice = fopen(nome_arquivo_indice, "r+b");
    if (arquivo_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        free(header);
        fclose(arquivo_dados);
        return false;
    }
    
    // Percorre todos os registros do arquivo de dados até proxByteOffset
    fseek(arquivo_dados, 276, SEEK_SET); // Pula o cabeçalho (276 bytes)
    int registros_inseridos = 0;
    
    while (ftell(arquivo_dados) < header->proxByteOffset) {
        long long int offset_atual = ftell(arquivo_dados);
        
        char removido;
        int tamanhoRegistro;
        long long int prox;
        int idAttack;
        
        // Lê apenas os campos necessários para evitar overhead
        if (fread(&removido, sizeof(char), 1, arquivo_dados) != 1) break;
        if (fread(&tamanhoRegistro, sizeof(int), 1, arquivo_dados) != 1) break;
        if (fread(&prox, sizeof(long long int), 1, arquivo_dados) != 1) break;
        if (fread(&idAttack, sizeof(int), 1, arquivo_dados) != 1) break;
        
        // Se o registro não está removido logicamente, insere no índice
        if (removido == '0') {
            // Insere chave (idAttack) e ponteiro (offset) na árvore-B
            if (!insere_arvore_b(arquivo_indice, idAttack, offset_atual)) {
                printf("Falha no processamento do arquivo.\n");
                free(header);
                fclose(arquivo_dados);
                fclose(arquivo_indice);
                return false;
            }
            registros_inseridos++;
        }
        
        // Pula para o próximo registro usando o tamanho
        fseek(arquivo_dados, offset_atual + tamanhoRegistro + 5, SEEK_SET);
    }
    
    // Fecha os arquivos
    fclose(arquivo_dados);
    fclose(arquivo_indice);
    free(header);
    
    // Usa binarioNaTela para mostrar o arquivo de índice criado
    binarioNaTela(nome_arquivo_indice);
    
    return true;
} 