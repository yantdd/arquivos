/*******************************************************************************
 *                                                                             *
 *                     SCC0215 - Organização de Arquivos                       *
 *                                                                             *
 *                     Funcionalidade 8 - Busca Múltipla                       *
 *                                                                             *
 * Professora: Cristina Dutra de Aguiar                                        *
 *                                                                             *
 * Aluno: Yan Trindade Meireles - 13680035                                     *
 *                                                                             *
 * Aluno: Rafael Perez Carmanhani - 15485420                                   *
 *                                                                             *
 *******************************************************************************/

#include "busca_multipla.h"
#include "imprime_bin.h"
#include <string.h>

/*
 * Executa buscas múltiplas com critérios especificados, utilizando índice árvore-B
 * quando possível para otimização de performance. Se idAttack está nos critérios,
 * usa busca indexada O(log n), caso contrário usa busca sequencial O(n).
 * Parâmetros:
 *  nome_arquivo_dados - nome do arquivo de dados
 *  nome_arquivo_indice - nome do arquivo de índice
 *  num_buscas - número de buscas a serem realizadas
 * Retorno:
 *  true se todas as buscas foram executadas com sucesso, false caso contrário
 */
bool executa_busca_multipla(char *nome_arquivo_dados, char *nome_arquivo_indice, int num_buscas) {
    // Abre arquivo de dados
    FILE *arquivo_dados = fopen(nome_arquivo_dados, "rb");
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
    FILE *arquivo_indice = fopen(nome_arquivo_indice, "rb");
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
    
    // Processa cada busca
    for (int busca_atual = 0; busca_atual < num_buscas; busca_atual++) {
        int num_criterios;
        scanf("%d", &num_criterios);
        
        // Aloca estrutura para a busca
        BUSCA_MULTIPLA busca;
        busca.num_criterios = num_criterios;
        busca.criterios = malloc(num_criterios * sizeof(CRITERIO_BUSCA));
        
        bool tem_id_attack = false;
        
        // Lê os critérios de busca
        for (int i = 0; i < num_criterios; i++) {
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
        
        bool encontrou = false;
        
        // NOVA ESTRATÉGIA: SEMPRE usar o arquivo de índices
        // COM idAttack: busca indexada O(log n) - mais eficiente
        // SEM idAttack: busca com in-order traversal O(n) - mantém uso do índice
        if (tem_id_attack) {
            encontrou = busca_com_indice(arquivo_dados, arquivo_indice, &busca);
        } else {
            encontrou = busca_com_in_order(arquivo_dados, arquivo_indice, &busca);
        }
        
        // Se não encontrou registros, imprime mensagem padrão
        if (!encontrou) {
            printf("Registro inexistente.\n\n");
        }
        
        // Libera memória da busca
        libera_busca_multipla(&busca);
        
        printf("**********\n");
    }
    
    // Fecha os arquivos
    fclose(arquivo_dados);
    fclose(arquivo_indice);
    free(header);
    
    return true;
}

/*
 * Verifica se um registro satisfaz todos os critérios de busca especificados.
 * Compara cada campo do registro com os valores dos critérios, considerando
 * tipos de dados diferentes (inteiros, floats e strings).
 * Parâmetros:
 *  registro - ponteiro para o registro a ser verificado
 *  busca - estrutura contendo os critérios de busca
 * Retorno:
 *  true se o registro satisfaz todos os critérios, false caso contrário
 */
bool satisfaz_criterios(REG_DADOS *registro, BUSCA_MULTIPLA *busca) {
    for (int i = 0; i < busca->num_criterios; i++) {
        if (strcmp(busca->criterios[i].nome_campo, "idAttack") == 0) {
            int valor = atoi(busca->criterios[i].valor_campo);
            if (registro->idAttack != valor) return false;
        }
        else if (strcmp(busca->criterios[i].nome_campo, "year") == 0) {
            int valor = atoi(busca->criterios[i].valor_campo);
            if (registro->year != valor) return false;
        }
        else if (strcmp(busca->criterios[i].nome_campo, "financialLoss") == 0) {
            float valor = atof(busca->criterios[i].valor_campo);
            if (registro->financialLoss != valor) return false;
        }
        else if (strcmp(busca->criterios[i].nome_campo, "country") == 0) {
            if (strcmp(registro->country, busca->criterios[i].valor_campo) != 0) return false;
        }
        else if (strcmp(busca->criterios[i].nome_campo, "attackType") == 0) {
            if (strcmp(registro->attackType, busca->criterios[i].valor_campo) != 0) return false;
        }
        else if (strcmp(busca->criterios[i].nome_campo, "targetIndustry") == 0) {
            if (strcmp(registro->targetIndustry, busca->criterios[i].valor_campo) != 0) return false;
        }
        else if (strcmp(busca->criterios[i].nome_campo, "defenseMechanism") == 0) {
            if (strcmp(registro->defenseMechanism, busca->criterios[i].valor_campo) != 0) return false;
        }
    }
    return true;
}

/*
 * Libera toda a memória alocada para uma busca múltipla.
 * Desaloca os arrays de critérios e suas strings associadas.
 * Parâmetro:
 *  busca - ponteiro para a estrutura de busca a ser liberada
 */
void libera_busca_multipla(BUSCA_MULTIPLA *busca) {
    for (int i = 0; i < busca->num_criterios; i++) {
        free(busca->criterios[i].nome_campo);
        free(busca->criterios[i].valor_campo);
    }
    free(busca->criterios);
}

/*
 * Lê um registro completo de uma posição específica no arquivo.
 * Utilizada para carregar registros encontrados via índice.
 * Parâmetros:
 *  arquivo - ponteiro para o arquivo de dados
 *  offset - posição do registro no arquivo
 * Retorno:
 *  ponteiro para o registro carregado ou NULL em caso de erro
 */
REG_DADOS* le_registro_completo(FILE *arquivo, long long int offset) {
    fseek(arquivo, offset, SEEK_SET);
    return get_registro(arquivo);
}

/*
 * Realiza busca múltipla SEMPRE usando o arquivo de índices.
 * Se idAttack está nos critérios, faz busca indexada O(log n).
 * Caso contrário, percorre a árvore-B em ordem (in-order traversal) O(n).
 */
bool busca_com_indice(FILE *arquivo_dados, FILE *arquivo_indice, BUSCA_MULTIPLA *busca) {
    // Encontra o valor do idAttack nos critérios
    int id_attack = -1;
    for (int i = 0; i < busca->num_criterios; i++) {
        if (strcmp(busca->criterios[i].nome_campo, "idAttack") == 0) {
            id_attack = atoi(busca->criterios[i].valor_campo);
            break;
        }
    }
    
    // Busca otimizada no índice O(log n)
    long long int offset_registro;
    if (!busca_arvore_b(arquivo_indice, id_attack, &offset_registro)) {
        return false;
    }
    
    // Lê o registro diretamente da posição encontrada
    REG_DADOS *registro = le_registro_completo(arquivo_dados, offset_registro);
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
    if (satisfaz_criterios(registro, busca)) {
        imprime_registro_bin(registro);
        // imprime_registro_bin já libera toda a memória do registro
        return true;
    }
    
    // Libera memória apenas se não imprimiu
    if (registro->country) free(registro->country);
    if (registro->attackType) free(registro->attackType);
    if (registro->targetIndustry) free(registro->targetIndustry);
    if (registro->defenseMechanism) free(registro->defenseMechanism);
    free(registro);
    return false;
}

/*
 * Busca usando in-order traversal da árvore-B (quando não há idAttack nos critérios).
 * SEMPRE usa o arquivo de índices.
 */
bool busca_com_in_order(FILE *arquivo_dados, FILE *arquivo_indice, BUSCA_MULTIPLA *busca) {
    // Obtém lista ordenada de todas as chaves via in-order traversal
    LISTA_CHAVES *lista = in_order_traversal(arquivo_indice);
    if (lista == NULL) return false;
    
    bool encontrou_algum = false;
    
    // Percorre todos os registros na ordem das chaves
    for (int i = 0; i < lista->num_chaves; i++) {
        // Lê o registro usando o ponteiro do índice
        REG_DADOS *registro = le_registro_completo(arquivo_dados, lista->chaves[i].ptr);
        
        if (registro != NULL && registro->removido == '0' && satisfaz_criterios(registro, busca)) {
            imprime_registro_bin(registro);
            encontrou_algum = true;
        } else {
            // Libera memória apenas se não imprimiu (pois imprime_registro_bin já libera)
            if (registro) {
                if (registro->country) free(registro->country);
                if (registro->attackType) free(registro->attackType);
                if (registro->targetIndustry) free(registro->targetIndustry);
                if (registro->defenseMechanism) free(registro->defenseMechanism);
                free(registro);
            }
        }
    }
    
    // Libera a lista de chaves
    libera_lista_chaves(lista);
    
    return encontrou_algum;
} 