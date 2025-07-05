/*******************************************************************************
 *                                                                             *
 *                     SCC0215 - Organização de Arquivos                       *
 *                                                                             *
 *                Funcionalidade 11 - Atualização com Índice                   *
 *                                                                             *
 * Professora: Cristina Dutra de Aguiar                                        *
 *                                                                             *
 * Aluno: Yan Trindade Meireles - 13680035                                     *
 *                                                                             *
 * Aluno: Rafael Perez Carmanhani - 15485420                                   *
 *                                                                             *
 *******************************************************************************/

#include "atualiza_com_indice.h"
#include "imprime_bin.h"
#include "insere_reg_bin.h"

/*
 * Executa atualizações de registros utilizando índice árvore-B. 
 * Escolhe automaticamente entre busca indexada e sequencial
 * baseado na presença de idAttack nos critérios de busca. Implementa estratégia
 * First Fit para reutilização de espaços removidos.
 * Parâmetros:
 *  nome_arquivo_dados - nome do arquivo de dados
 *  nome_arquivo_indice - nome do arquivo de índice
 *  num_atualizacoes - número de atualizações a realizar
 * Retorno:
 *  true se todas as atualizações foram executadas com sucesso, false caso contrário
 */
bool executa_atualizacoes_com_indice(char *nome_arquivo_dados, char *nome_arquivo_indice, int num_atualizacoes) {
    // Abre arquivo de dados
    FILE *arquivo_dados = fopen(nome_arquivo_dados, "r+b");
    if (arquivo_dados == NULL) {
        return false;
    }
    
    // Verifica consistência do arquivo de dados
    REG_HEADER *header = get_reg_header(arquivo_dados);
    if (header == NULL || header->status != '1') {
        fclose(arquivo_dados);
        if (header) free(header);
        return false;
    }
    
    // Abre arquivo de índice
    FILE *arquivo_indice = fopen(nome_arquivo_indice, "r+b");
    if (arquivo_indice == NULL) {
        fclose(arquivo_dados);
        free(header);
        return false;
    }
    
    // Verifica consistência do arquivo de índice
    CABECALHO_INDICE cab_indice;
    le_cabecalho_indice(arquivo_indice, &cab_indice);
    if (cab_indice.status != '1') {
        fclose(arquivo_dados);
        fclose(arquivo_indice);
        free(header);
        return false;
    }
    
    // Processa cada atualização
    for (int atualizacao_atual = 0; atualizacao_atual < num_atualizacoes; atualizacao_atual++) {
        int num_criterios_busca;
        scanf("%d", &num_criterios_busca);
        
        // Aloca estrutura para a busca
        BUSCA_MULTIPLA busca;
        busca.num_criterios = num_criterios_busca;
        busca.criterios = malloc(num_criterios_busca * sizeof(CRITERIO_BUSCA));
        
        bool tem_id_attack = false;
        
        // Lê os critérios de busca
        for (int i = 0; i < num_criterios_busca; i++) {
            busca.criterios[i].nome_campo = malloc(100);
            busca.criterios[i].valor_campo = malloc(200);
            
            scanf("%s", busca.criterios[i].nome_campo);
            
            // Verifica o tipo de campo para decidir como ler o valor
            if (strcmp(busca.criterios[i].nome_campo, "idAttack") == 0 ||
                strcmp(busca.criterios[i].nome_campo, "year") == 0 ||
                strcmp(busca.criterios[i].nome_campo, "financialLoss") == 0) {
                // Campos numéricos, ler sem aspas
                scanf("%s", busca.criterios[i].valor_campo);
            } else {
                // Campos string, usar scan_quote_string
                scan_quote_string(busca.criterios[i].valor_campo);
            }
            
            // Verifica se tem idAttack nos critérios para otimização
            if (strcmp(busca.criterios[i].nome_campo, "idAttack") == 0) {
                tem_id_attack = true;
            }
        }
        
        // Lê os campos de atualização
        int num_campos_atualizacao;
        scanf("%d", &num_campos_atualizacao);
        
        char **nomes_campos_atualizacao = malloc(num_campos_atualizacao * sizeof(char *));
        char **valores_campos_atualizacao = malloc(num_campos_atualizacao * sizeof(char *));
        
        for (int i = 0; i < num_campos_atualizacao; i++) {
            nomes_campos_atualizacao[i] = malloc(50);
            valores_campos_atualizacao[i] = malloc(100);
            
            scanf("%s", nomes_campos_atualizacao[i]);
            
            // Verifica o tipo de campo para decidir como ler o valor
            if (strcmp(nomes_campos_atualizacao[i], "idAttack") == 0 ||
                strcmp(nomes_campos_atualizacao[i], "year") == 0 ||
                strcmp(nomes_campos_atualizacao[i], "financialLoss") == 0) {
                // Campos numéricos
                scanf("%s", valores_campos_atualizacao[i]);
            } else {
                // Campos string, usar scan_quote_string
                scan_quote_string(valores_campos_atualizacao[i]);
            }
        }
        
        // NOVA ESTRATÉGIA: SEMPRE usar o arquivo de índices
        // COM idAttack: utiliza busca indexada O(log n)
        // SEM idAttack: utiliza in-order traversal O(n) - mantém uso do índice
        if (tem_id_attack) {
            atualiza_com_busca_indice(arquivo_dados, arquivo_indice, header, &busca, 
                                     num_campos_atualizacao, nomes_campos_atualizacao, 
                                     valores_campos_atualizacao);
        } else {
            atualiza_com_in_order(arquivo_dados, arquivo_indice, header, &busca, 
                                 num_campos_atualizacao, nomes_campos_atualizacao, 
                                 valores_campos_atualizacao);
        }
        
        // Libera memória da busca
        libera_busca_multipla(&busca);
        
        // Libera memória dos campos de atualização
        for (int i = 0; i < num_campos_atualizacao; i++) {
            free(nomes_campos_atualizacao[i]);
            free(valores_campos_atualizacao[i]);
        }
        free(nomes_campos_atualizacao);
        free(valores_campos_atualizacao);
    }
    
    // Marca como consistente após todas as operações
    atualiza_header_atualizacao_final(header, arquivo_dados);
    
    fclose(arquivo_dados);
    fclose(arquivo_indice);
    free(header);
    
    // Mostra os arquivos na tela
    binarioNaTela(nome_arquivo_dados);
    binarioNaTela(nome_arquivo_indice);
    
    return true;
}

/*
 * Atualiza registros SEMPRE usando o arquivo de índices.
 * Se idAttack está nos critérios, faz atualização indexada O(log n).
 * Caso contrário, percorre a árvore-B em ordem (in-order traversal) O(n).
 */
bool atualiza_com_busca_indice(FILE *arquivo_dados, FILE *arquivo_indice, REG_HEADER *header, BUSCA_MULTIPLA *busca, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao) {
    // Encontra o valor do idAttack nos critérios
    int id_attack = -1;
    for (int i = 0; i < busca->num_criterios; i++) {
        if (strcmp(busca->criterios[i].nome_campo, "idAttack") == 0) {
            id_attack = atoi(busca->criterios[i].valor_campo);
            break;
        }
    }
    
    // Busca otimizada no índice árvore-B O(log n)
    long long int offset_registro;
    if (!busca_arvore_b(arquivo_indice, id_attack, &offset_registro)) {
        return false;
    }
    
    // Lê o registro diretamente da posição encontrada pelo índice
    fseek(arquivo_dados, offset_registro, SEEK_SET);
    REG_DADOS *registro = get_registro(arquivo_dados);
    if (registro == NULL || registro->removido != '0') {
        if (registro) {
            if (registro->country) free(registro->country);
            if (registro->attackType) free(registro->attackType);
            if (registro->targetIndustry) free(registro->targetIndustry);
            if (registro->defenseMechanism) free(registro->defenseMechanism);
            free(registro);
        }
        return false;
    }
    
    // Verifica se satisfaz todos os critérios (não apenas idAttack)
    if (!satisfaz_criterios(registro, busca)) {
        // Libera memória
        if (registro->country) free(registro->country);
        if (registro->attackType) free(registro->attackType);
        if (registro->targetIndustry) free(registro->targetIndustry);
        if (registro->defenseMechanism) free(registro->defenseMechanism);
        free(registro);
        return false;
    }
    
    // Aplica as atualizações usando a lógica da funcionalidade 6
    REG_DADOS *reg_atualizado = aplica_atualizacao(registro, num_campos_atualizacao, 
                                                   nomes_campos_atualizacao, valores_campos_atualizacao);
    
    if (reg_atualizado != NULL) {
        int tamanho_original = registro->tamanhoRegistro;
        int tamanho_atualizado = reg_atualizado->tamanhoRegistro;
        
        // ATUALIZAÇÃO COM OTIMIZAÇÃO:
        if (tamanho_atualizado <= tamanho_original) {
            // Tamanho menor/igual: atualização in-place
            fseek(arquivo_dados, offset_registro, SEEK_SET);
            reg_atualizado->tamanhoRegistro = registro->tamanhoRegistro; // Mantém tamanho original
            int tamanho_fisico_original = calcula_tamanho_fisico_registro(arquivo_dados, offset_registro);
            escreve_registro_atualizado_no_local(arquivo_dados, reg_atualizado, tamanho_fisico_original);
        } else {
            // Tamanho maior: remoção lógica + inserção nova
            // Mantém integridade do arquivo e índice
            fseek(arquivo_dados, offset_registro, SEEK_SET);
            
            // Marca como removido
            char removido = '1';
            fwrite(&removido, sizeof(char), 1, arquivo_dados);
            
            // Pula o tamanhoRegistro
            fseek(arquivo_dados, sizeof(int), SEEK_CUR);
            
            // Escreve o campo prox com o valor atual do topo
            fwrite(&header->topo, sizeof(long long int), 1, arquivo_dados);
            
            // Atualiza o topo para apontar para este registro
            header->topo = offset_registro;
            
            // Atualiza contadores
            header->nroRegArq--;
            header->nroRegRem++;
            
            // Insere o novo registro e obtém sua nova posição
            long long int nova_posicao = match_insere_bin_com_offset(arquivo_dados, header, reg_atualizado);
            
            // Atualiza o índice para apontar para a nova posição
            atualiza_ponteiro_arvore_b(arquivo_indice, id_attack, nova_posicao);
        }
        
        // Libera memória do registro atualizado
        free(reg_atualizado->country);
        free(reg_atualizado->attackType);
        free(reg_atualizado->targetIndustry);
        free(reg_atualizado->defenseMechanism);
        free(reg_atualizado);
    }
    
    // Libera memória do registro original
    if (registro->country) free(registro->country);
    if (registro->attackType) free(registro->attackType);
    if (registro->targetIndustry) free(registro->targetIndustry);
    if (registro->defenseMechanism) free(registro->defenseMechanism);
    free(registro);
    
    return true;
}

/*
 * Atualiza registros encontrados via in-order traversal da árvore-B (quando não há idAttack).
 * SEMPRE usa o arquivo de índices.
 */
bool atualiza_com_in_order(FILE *arquivo_dados, FILE *arquivo_indice, REG_HEADER *header, BUSCA_MULTIPLA *busca, int num_campos_atualizacao, char **nomes_campos_atualizacao, char **valores_campos_atualizacao) {
    
    // Obtém lista ordenada de todas as chaves via in-order traversal
    LISTA_CHAVES *lista = in_order_traversal(arquivo_indice);
    if (lista == NULL) return false;
    
    // Array para armazenar posições dos registros que fazem match
    long long *posicoes_match = NULL;
    int count_matches = 0;
    int capacity_matches = 10;
    
    posicoes_match = (long long *)malloc(capacity_matches * sizeof(long long));
    if (!posicoes_match) {
        libera_lista_chaves(lista);
        return false;
    }
    
    // PRIMEIRA PASSADA: Identificar registros que fazem match usando in-order traversal
    for (int i = 0; i < lista->num_chaves; i++) {
        long long int offset_atual = lista->chaves[i].ptr;
        
        REG_DADOS *registro = le_registro_completo(arquivo_dados, offset_atual);
        if (registro == NULL) continue;
        
        // Verifica se não está removido e satisfaz critérios
        if (registro->removido == '0' && satisfaz_criterios(registro, busca)) {
            // Expande o array se necessário
            if (count_matches >= capacity_matches) {
                capacity_matches *= 2;
                posicoes_match = (long long *)realloc(posicoes_match, capacity_matches * sizeof(long long));
                if (!posicoes_match) {
                    if (registro->country) free(registro->country);
                    if (registro->attackType) free(registro->attackType);
                    if (registro->targetIndustry) free(registro->targetIndustry);
                    if (registro->defenseMechanism) free(registro->defenseMechanism);
                    free(registro);
                    libera_lista_chaves(lista);
                    return false;
                }
            }
            posicoes_match[count_matches++] = offset_atual;
        }
        
        // Libera memória do registro
        if (registro->country) free(registro->country);
        if (registro->attackType) free(registro->attackType);
        if (registro->targetIndustry) free(registro->targetIndustry);
        if (registro->defenseMechanism) free(registro->defenseMechanism);
        free(registro);
    }
    
    // Libera a lista de chaves
    libera_lista_chaves(lista);
    
    bool encontrou = (count_matches > 0);
    
    // SEGUNDA PASSADA: Aplicar atualizações em ordem reversa
    for (int i = count_matches - 1; i >= 0; i--) {
        fseek(arquivo_dados, posicoes_match[i], SEEK_SET);
        REG_DADOS *registro = get_registro(arquivo_dados);
        
        if (registro != NULL && registro->removido == '0') {
            // Aplica as atualizações
            REG_DADOS *reg_atualizado = aplica_atualizacao(registro, num_campos_atualizacao, 
                                                           nomes_campos_atualizacao, valores_campos_atualizacao);
            
            if (reg_atualizado != NULL) {
                int tamanho_original = registro->tamanhoRegistro;
                int tamanho_atualizado = reg_atualizado->tamanhoRegistro;
                
                if (tamanho_atualizado <= tamanho_original) {
                    // Atualização in-place
                    fseek(arquivo_dados, posicoes_match[i], SEEK_SET);
                    reg_atualizado->tamanhoRegistro = registro->tamanhoRegistro;
                    int tamanho_fisico_original = calcula_tamanho_fisico_registro(arquivo_dados, posicoes_match[i]);
                    escreve_registro_atualizado_no_local(arquivo_dados, reg_atualizado, tamanho_fisico_original);
                } else {
                    // Remove logicamente e insere novo
                    fseek(arquivo_dados, posicoes_match[i], SEEK_SET);
                    
                    char removido = '1';
                    fwrite(&removido, sizeof(char), 1, arquivo_dados);
                    fseek(arquivo_dados, sizeof(int), SEEK_CUR);
                    fwrite(&header->topo, sizeof(long long int), 1, arquivo_dados);
                    
                    header->topo = posicoes_match[i];
                    header->nroRegArq--;
                    header->nroRegRem++;
                    
                    // Insere novo registro
                    long long int nova_posicao = match_insere_bin_com_offset(arquivo_dados, header, reg_atualizado);
                    
                    // Atualiza índice se necessário
                    if (reg_atualizado->idAttack != registro->idAttack) {
                        // Se idAttack mudou, remove a chave antiga e insere nova
                        // Nota: remove_arvore_b não está implementado, então vamos apenas inserir a nova
                        insere_arvore_b(arquivo_indice, reg_atualizado->idAttack, nova_posicao);
                    } else {
                        // Se idAttack não mudou, apenas atualiza o ponteiro
                        atualiza_ponteiro_arvore_b(arquivo_indice, reg_atualizado->idAttack, nova_posicao);
                    }
                }
                
                // Libera memória do registro atualizado
                free(reg_atualizado->country);
                free(reg_atualizado->attackType);
                free(reg_atualizado->targetIndustry);
                free(reg_atualizado->defenseMechanism);
                free(reg_atualizado);
            }
        }
        
        // Libera memória do registro original
        if (registro) {
            if (registro->country) free(registro->country);
            if (registro->attackType) free(registro->attackType);
            if (registro->targetIndustry) free(registro->targetIndustry);
            if (registro->defenseMechanism) free(registro->defenseMechanism);
            free(registro);
        }
    }
    
    free(posicoes_match);
    
    return encontrou;
} 