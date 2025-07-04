/*******************************************************************************
 *                                                                             *
 *                     SCC0215 - Organização de Arquivos                       *
 *                                                                             *
 *                          Índice Árvore-B                                    *
 *                                                                             *
 * Professora: Cristina Dutra de Aguiar                                        *
 *                                                                             *
 * Aluno: Yan Trindade Meireles - 13680035                                     *
 *                                                                             *
 * Aluno: Rafael Perez Carmanhani - 15485420                                   *
 *                                                                             *
 *******************************************************************************/

#include "arvore_b.h"
#include <string.h>

/*
 * Inicializa um cabeçalho de índice com valores padrão conforme especificação.
 * O cabeçalho contém metadados essenciais para gerenciamento da árvore-B.
 * Retorno:
 *  ponteiro para o cabeçalho inicializado ou NULL em caso de erro
 */
CABECALHO_INDICE* inicializa_cabecalho_indice() {
    CABECALHO_INDICE *cab = malloc(sizeof(CABECALHO_INDICE));
    if (cab == NULL) return NULL;
    
    cab->status = '0';    // Inicialmente inconsistente
    cab->noRaiz = -1;     // Árvore vazia
    cab->proxRRN = 0;     // Inicialmente proxRRN = 0
    cab->nroNos = 0;      // Inicialmente nroNos = 0
    
    return cab;
}

/*
 * Inicializa um nó da árvore-B com valores padrão conforme especificação.
 * Todos os ponteiros são inicializados com -1 (nulo) e contadores zerados.
 * Retorno:
 *  ponteiro para o nó inicializado ou NULL em caso de erro
 */
NO_ARVORE* inicializa_no() {
    NO_ARVORE *no = malloc(sizeof(NO_ARVORE));
    if (no == NULL) return NULL;
    
    no->tipoNo = -1;      // Folha por padrão
    no->nroChaves = 0;    // Nenhuma chave
    
    // Inicializa todos os ponteiros para filhos com -1 (nulo conforme especificação)
    for (int i = 0; i < ORDEM_ARVORE; i++) {
        no->P[i] = -1;
    }
    
    // Inicializa todas as chaves e ponteiros com -1 conforme especificação
    for (int i = 0; i < ORDEM_ARVORE - 1; i++) {
        no->dados[i].chave = -1;
        no->dados[i].ptr = -1;
    }
    
    return no;
}

/*
 * Escreve o cabeçalho no arquivo de índice na posição inicial.
 * Preenche os bytes restantes do cabeçalho com lixo '$' conforme especificação.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  cab - ponteiro para o cabeçalho a ser escrito
 */
void escreve_cabecalho_indice(FILE *indice, CABECALHO_INDICE *cab) {
    fseek(indice, 0, SEEK_SET);
    
    fwrite(&cab->status, sizeof(char), 1, indice);
    fwrite(&cab->noRaiz, sizeof(int), 1, indice);
    fwrite(&cab->proxRRN, sizeof(int), 1, indice);
    fwrite(&cab->nroNos, sizeof(int), 1, indice);
    
    // Preenche o resto do cabeçalho com lixo ($)
    for (int i = 13; i < TAMANHO_CABECALHO; i++) {
        fputc('$', indice);
    }
}

/*
 * Lê o cabeçalho do arquivo de índice da posição inicial.
 * Carrega os metadados da árvore-B para estrutura em memória.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  cab - ponteiro para o cabeçalho onde dados serão armazenados
 */
void le_cabecalho_indice(FILE *indice, CABECALHO_INDICE *cab) {
    fseek(indice, 0, SEEK_SET);
    
    fread(&cab->status, sizeof(char), 1, indice);
    fread(&cab->noRaiz, sizeof(int), 1, indice);
    fread(&cab->proxRRN, sizeof(int), 1, indice);
    fread(&cab->nroNos, sizeof(int), 1, indice);
}

/*
 * Escreve um nó no arquivo de índice na posição especificada pelo RRN.
 * Segue a especificação: tipoNo, nroChaves, P1, C1, PR1, P2, C2, PR2, P3.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  no - ponteiro para o nó a ser escrito
 *  rrn - RRN (posição relativa) onde o nó será escrito
 */
void escreve_no(FILE *indice, NO_ARVORE *no, int rrn) {
    long long int pos = TAMANHO_CABECALHO + (rrn * TAMANHO_NO);
    fseek(indice, pos, SEEK_SET);
    
    fwrite(&no->tipoNo, sizeof(int), 1, indice);
    fwrite(&no->nroChaves, sizeof(int), 1, indice);
    
    // Escreve na ordem especificada: P1, C1, PR1, P2, C2, PR2, P3
    fwrite(&no->P[0], sizeof(int), 1, indice);                    // P1
    fwrite(&no->dados[0].chave, sizeof(int), 1, indice);          // C1
    fwrite(&no->dados[0].ptr, sizeof(long long int), 1, indice);  // PR1
    fwrite(&no->P[1], sizeof(int), 1, indice);                    // P2
    fwrite(&no->dados[1].chave, sizeof(int), 1, indice);          // C2
    fwrite(&no->dados[1].ptr, sizeof(long long int), 1, indice);  // PR2
    fwrite(&no->P[2], sizeof(int), 1, indice);                    // P3
}

/*
 * Lê um nó do arquivo de índice na posição especificada pelo RRN.
 * Segue a especificação: tipoNo, nroChaves, P1, C1, PR1, P2, C2, PR2, P3.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  rrn - RRN (posição relativa) do nó a ser lido
 * Retorno:
 *  ponteiro para o nó carregado ou NULL se RRN inválido
 */
NO_ARVORE* le_no(FILE *indice, int rrn) {
    if (rrn == -1) return NULL;
    
    NO_ARVORE *no = malloc(sizeof(NO_ARVORE));
    if (no == NULL) return NULL;
    
    long long int pos = TAMANHO_CABECALHO + (rrn * TAMANHO_NO);
    fseek(indice, pos, SEEK_SET);
    
    fread(&no->tipoNo, sizeof(int), 1, indice);
    fread(&no->nroChaves, sizeof(int), 1, indice);
    
    // Lê na ordem especificada: P1, C1, PR1, P2, C2, PR2, P3
    fread(&no->P[0], sizeof(int), 1, indice);                    // P1
    fread(&no->dados[0].chave, sizeof(int), 1, indice);          // C1
    fread(&no->dados[0].ptr, sizeof(long long int), 1, indice);  // PR1
    fread(&no->P[1], sizeof(int), 1, indice);                    // P2
    fread(&no->dados[1].chave, sizeof(int), 1, indice);          // C2
    fread(&no->dados[1].ptr, sizeof(long long int), 1, indice);  // PR2
    fread(&no->P[2], sizeof(int), 1, indice);                    // P3
    
    return no;
}

/*
 * Busca uma posição adequada para inserir uma chave no nó.
 * Implementa busca linear nas chaves do nó para encontrar posição correta.
 * Parâmetros:
 *  no - ponteiro para o nó onde buscar
 *  chave - valor da chave a ser posicionada
 * Retorno:
 *  índice da posição onde a chave deve ser inserida
 */
int busca_posicao_chave(NO_ARVORE *no, int chave) {
    int i = 0;
    while (i < no->nroChaves && chave > no->dados[i].chave) {
        i++;
    }
    return i;
}

/*
 * Insere uma chave em um nó na posição especificada.
 * Move as chaves existentes para abrir espaço e mantém ordem crescente.
 * Segue a estrutura: P1, C1, PR1, P2, C2, PR2, P3.
 * Parâmetros:
 *  no - ponteiro para o nó onde inserir
 *  chave - valor da chave a ser inserida
 *  ptr - ponteiro para o registro no arquivo de dados
 *  filho_direito - RRN do filho direito da chave
 *  pos - posição onde inserir a chave
 */
void insere_chave_no(NO_ARVORE *no, int chave, long long int ptr, int filho_direito, int pos) {
    // Move as chaves para a direita para abrir espaço
    for (int i = no->nroChaves; i > pos; i--) {
        no->dados[i] = no->dados[i-1];
        no->P[i+1] = no->P[i];
    }
    
    // Insere a nova chave na posição correta
    no->dados[pos].chave = chave;
    no->dados[pos].ptr = ptr;
    no->P[pos+1] = filho_direito;
    no->nroChaves++;
}

/*
 * Divide um nó em dois quando há overflow
 * Estratégia tradicional: promove a chave do meio
 */
void split_no(NO_ARVORE *no_original, NO_ARVORE *novo_no, int chave_nova, 
              long long int ptr_novo, int filho_direito, 
              int *chave_promovida, long long int *ptr_promovido) {
    
    // Array para as 3 chaves totais (2 existentes + 1 nova)
    CHAVE_PTR todas_chaves[ORDEM_ARVORE];
    int todos_filhos[ORDEM_ARVORE + 1];
    
    // Encontra posição de inserção da nova chave
    int pos = busca_posicao_chave(no_original, chave_nova);
    
    // Monta array com todas as chaves na ordem correta
    todos_filhos[0] = no_original->P[0];
    int j = 0;
    for (int i = 0; i < ORDEM_ARVORE; i++) {
        if (i == pos) {
            todas_chaves[i].chave = chave_nova;
            todas_chaves[i].ptr = ptr_novo;
            todos_filhos[i + 1] = filho_direito;
        } else {
            todas_chaves[i] = no_original->dados[j];
            todos_filhos[i + 1] = no_original->P[j + 1];
            j++;
        }
    }
    
    // Estratégia tradicional: promove a chave do meio (index 1)
    
    // Chave do meio é promovida
    *chave_promovida = todas_chaves[1].chave;
    *ptr_promovido = todas_chaves[1].ptr;
    
    // Nó esquerdo (original) fica com a primeira chave
    no_original->nroChaves = 1;
    no_original->dados[0] = todas_chaves[0];
    no_original->dados[1].chave = -1;
    no_original->dados[1].ptr = -1;
    no_original->P[0] = todos_filhos[0];
    no_original->P[1] = todos_filhos[1];
    no_original->P[2] = -1;
    
    // Nó direito (novo) fica com a última chave
    novo_no->nroChaves = 1;
    novo_no->dados[0] = todas_chaves[2];
    novo_no->dados[1].chave = -1;
    novo_no->dados[1].ptr = -1;
    novo_no->P[0] = todos_filhos[2];
    novo_no->P[1] = todos_filhos[3];
    novo_no->P[2] = -1;
    novo_no->tipoNo = no_original->tipoNo;
}

/*
 * Função auxiliar recursiva para inserção
 */
RESULTADO_INSERCAO* insere_recursivo(FILE *indice, int rrn_atual, int chave, long long int ptr, 
                                     CABECALHO_INDICE *cab) {
    RESULTADO_INSERCAO *resultado = malloc(sizeof(RESULTADO_INSERCAO));
    resultado->promoveu = false;
    
    // Caso base: chegou em folha inexistente (nulo)
    if (rrn_atual == -1) {
        resultado->promoveu = true;
        resultado->chave_promovida = chave;
        resultado->ptr_promovido = ptr;
        resultado->rrn_promovido = -1;
        return resultado;
    }
    
    NO_ARVORE *no_atual = le_no(indice, rrn_atual);
    if (no_atual == NULL) {
        free(resultado);
        return NULL;
    }
    
    int pos = busca_posicao_chave(no_atual, chave);
    
    // Verifica se chave já existe (não deve acontecer para idAttack único)
    if (pos < no_atual->nroChaves && no_atual->dados[pos].chave == chave) {
        free(no_atual);
        free(resultado);
        return NULL;
    }
    
    // Desce recursivamente na árvore
    RESULTADO_INSERCAO *resultado_filho = insere_recursivo(indice, no_atual->P[pos], chave, ptr, cab);
    
    if (resultado_filho == NULL || !resultado_filho->promoveu) {
        free(no_atual);
        return resultado_filho;
    }
    
    // Houve promoção de baixo, tenta inserir no nó atual
    if (no_atual->nroChaves < ORDEM_ARVORE - 1) {
        // Nó tem espaço, insere sem split
        insere_chave_no(no_atual, resultado_filho->chave_promovida, 
                       resultado_filho->ptr_promovido, resultado_filho->rrn_promovido, pos);
        escreve_no(indice, no_atual, rrn_atual);
        
        resultado->promoveu = false;
        free(resultado_filho);
        free(no_atual);
        return resultado;
    }
    
    // Nó cheio, faz split seguindo especificação
    NO_ARVORE *novo_no = inicializa_no();
    int novo_rrn = cab->proxRRN;
    cab->proxRRN++;
    cab->nroNos++;
    
    split_no(no_atual, novo_no, resultado_filho->chave_promovida, 
             resultado_filho->ptr_promovido, resultado_filho->rrn_promovido,
             &resultado->chave_promovida, &resultado->ptr_promovido);
    
    novo_no->tipoNo = no_atual->tipoNo;
    
    escreve_no(indice, no_atual, rrn_atual);
    escreve_no(indice, novo_no, novo_rrn);
    
    resultado->promoveu = true;
    resultado->rrn_promovido = novo_rrn;
    
    free(resultado_filho);
    free(no_atual);
    free(novo_no);
    
    return resultado;
}

/*
 * Insere uma nova chave na árvore-B mantendo as propriedades de balanceamento.
 * Implementa inserção com split automático quando necessário, garantindo que a árvore
 * permaneça balanceada para busca otimizada O(log n). Gerencia criação de nova raiz
 * quando há overflow na raiz atual.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  chave - valor de idAttack a ser inserido
 *  ptr - offset do registro no arquivo de dados
 * Retorno:
 *  true se a inserção foi bem-sucedida, false caso contrário
 */
bool insere_arvore_b(FILE *indice, int chave, long long int ptr) {
    CABECALHO_INDICE *cab = malloc(sizeof(CABECALHO_INDICE));
    le_cabecalho_indice(indice, cab);
    
    if (cab->status != '1') {
        free(cab);
        return false; // Arquivo inconsistente
    }
    
    // Marca como inconsistente durante a operação para garantir integridade
    cab->status = '0'; 
    escreve_cabecalho_indice(indice, cab);
    
    RESULTADO_INSERCAO *resultado = insere_recursivo(indice, cab->noRaiz, chave, ptr, cab);
    
    if (resultado == NULL) {
        cab->status = '1';
        escreve_cabecalho_indice(indice, cab);
        free(cab);
        return false;
    }
    
    // Se houve promoção na raiz, cria nova raiz
    if (resultado->promoveu) {
        if (cab->noRaiz == -1) {
            // Primeira inserção - criar nó folha que é também raiz
            NO_ARVORE *nova_raiz = inicializa_no();
            // Especificação: "Quando nó-folha = nó-raiz, tipoNo = -1"
            nova_raiz->tipoNo = -1; 
            nova_raiz->nroChaves = 1;
            nova_raiz->dados[0].chave = resultado->chave_promovida;
            nova_raiz->dados[0].ptr = resultado->ptr_promovido;
            
            // Especificação: "Quando o primeiro nó é criado (nó folha = nó raiz), proxRRN = 1"
            cab->noRaiz = 0; // Primeiro nó sempre tem RRN 0
            cab->proxRRN = 1; // Conforme especificação
            cab->nroNos = 1;
            
            escreve_no(indice, nova_raiz, 0);
            free(nova_raiz);
        } else {
            // Split da raiz - criar nova raiz interna
            NO_ARVORE *nova_raiz = inicializa_no();
            nova_raiz->tipoNo = 0; // Raiz interna
            nova_raiz->nroChaves = 1;
            nova_raiz->dados[0].chave = resultado->chave_promovida;
            nova_raiz->dados[0].ptr = resultado->ptr_promovido;
            nova_raiz->P[0] = cab->noRaiz;
            nova_raiz->P[1] = resultado->rrn_promovido;
            
            // Os nós filhos agora são intermediários (não mais raiz)
            NO_ARVORE *no_esq = le_no(indice, cab->noRaiz);
            NO_ARVORE *no_dir = le_no(indice, resultado->rrn_promovido);
            if (no_esq->tipoNo == 0) no_esq->tipoNo = 1; // Era raiz, agora intermediário
            if (no_dir->tipoNo == 0) no_dir->tipoNo = 1; // Era raiz, agora intermediário
            escreve_no(indice, no_esq, cab->noRaiz);
            escreve_no(indice, no_dir, resultado->rrn_promovido);
            free(no_esq);
            free(no_dir);
            
            int nova_raiz_rrn = cab->proxRRN;
            cab->proxRRN++;
            cab->nroNos++;
            cab->noRaiz = nova_raiz_rrn;
            
            escreve_no(indice, nova_raiz, nova_raiz_rrn);
            free(nova_raiz);
        }
    }
    
    // Marca como consistente ao final da operação
    cab->status = '1'; 
    escreve_cabecalho_indice(indice, cab);
    
    free(resultado);
    free(cab);
    return true;
}

/*
 * Busca uma chave na árvore-B de forma otimizada.
 * Implementa busca binária nos nós e navegação pela árvore, oferecendo
 * complexidade O(log n) para localização de registros. Percorre a árvore
 * desde a raiz até encontrar a chave ou determinar sua ausência.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  chave - valor de idAttack a ser buscado
 *  ptr_resultado - ponteiro para armazenar o offset do registro encontrado
 * Retorno:
 *  true se a chave foi encontrada, false caso contrário
 */
bool busca_arvore_b(FILE *indice, int chave, long long int *ptr_resultado) {
    CABECALHO_INDICE *cab = malloc(sizeof(CABECALHO_INDICE));
    le_cabecalho_indice(indice, cab);
    
    if (cab->status != '1') {
        free(cab);
        return false;
    }
    
    int rrn_atual = cab->noRaiz;
    free(cab);
    
    // Navega pela árvore desde a raiz até encontrar a chave
    while (rrn_atual != -1) {
        NO_ARVORE *no = le_no(indice, rrn_atual);
        if (no == NULL) return false;
        
        int pos = busca_posicao_chave(no, chave);
        
        // Verifica se encontrou a chave no nó atual
        if (pos < no->nroChaves && no->dados[pos].chave == chave) {
            *ptr_resultado = no->dados[pos].ptr;
            free(no);
            return true;
        }
        
        // Desce para o filho apropriado
        rrn_atual = no->P[pos];
        free(no);
    }
    
    return false;
}

/*
 * Atualiza o ponteiro de uma chave existente na árvore-B.
 * Busca a chave na árvore e atualiza apenas o ponteiro para o registro.
 * Usado quando um registro é movido para nova posição após atualização.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  chave - valor de idAttack da chave a ser atualizada
 *  novo_ptr - novo offset do registro no arquivo de dados
 * Retorno:
 *  true se a chave foi encontrada e atualizada, false caso contrário
 */
bool atualiza_ponteiro_arvore_b(FILE *indice, int chave, long long int novo_ptr) {
    CABECALHO_INDICE *cab = malloc(sizeof(CABECALHO_INDICE));
    le_cabecalho_indice(indice, cab);
    
    if (cab->status != '1') {
        free(cab);
        return false;
    }
    
    int rrn_atual = cab->noRaiz;
    free(cab);
    
    // Navega pela árvore desde a raiz até encontrar a chave
    while (rrn_atual != -1) {
        NO_ARVORE *no = le_no(indice, rrn_atual);
        if (no == NULL) return false;
        
        int pos = busca_posicao_chave(no, chave);
        
        // Verifica se encontrou a chave no nó atual
        if (pos < no->nroChaves && no->dados[pos].chave == chave) {
            // Atualiza o ponteiro
            no->dados[pos].ptr = novo_ptr;
            // Escreve o nó atualizado de volta no arquivo
            escreve_no(indice, no, rrn_atual);
            free(no);
            return true;
        }
        
        // Desce para o filho apropriado
        rrn_atual = no->P[pos];
        free(no);
    }
    
    return false;
}

/*
 * Placeholder para remoção (implementação completa seria mais complexa).
 * Funcionalidade não implementada no escopo do trabalho.
 * Parâmetros:
 *  indice - ponteiro para o arquivo de índice
 *  chave - chave a ser removida
 * Retorno:
 *  sempre false (não implementado)
 */
bool remove_arvore_b(FILE *indice, int chave) {
    // Implementação simplificada - apenas marca como não implementada
    (void)indice; // Suprime warning de parâmetro não usado
    (void)chave;  // Suprime warning de parâmetro não usado
    return false;
}

/*
 * Cria um arquivo de índice vazio com cabeçalho inicializado.
 * Prepara a estrutura básica para receber inserções posteriores.
 * Parâmetro:
 *  nome_arquivo - nome do arquivo de índice a ser criado
 * Retorno:
 *  true se o arquivo foi criado com sucesso, false caso contrário
 */
bool cria_arquivo_indice(char *nome_arquivo) {
    FILE *indice = fopen(nome_arquivo, "wb");
    if (indice == NULL) return false;
    
    CABECALHO_INDICE *cab = inicializa_cabecalho_indice();
    cab->status = '1'; // Marca como consistente
    
    escreve_cabecalho_indice(indice, cab);
    
    fclose(indice);
    free(cab);
    return true;
}

