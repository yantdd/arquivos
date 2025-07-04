#ifndef ARVORE_B_H
#define ARVORE_B_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDEM_ARVORE 3
#define TAMANHO_CABECALHO 44
#define TAMANHO_NO 44

// Cabeçalho do arquivo de índice árvore-B
typedef struct cabecalho_indice {
    char status;           // '0' inconsistente, '1' consistente
    int noRaiz;           // RRN do nó raiz, -1 se vazia
    int proxRRN;          // Próximo RRN a ser usado
    int nroNos;           // Número de nós na árvore
} CABECALHO_INDICE;

// Estrutura para armazenar um par chave-ponteiro
typedef struct chave_ptr {
    int chave;            // Campo idAttack (4 bytes)
    long long int ptr;    // Referência para o registro no arquivo de dados (8 bytes)
} CHAVE_PTR;

// Estrutura do nó da árvore-B
typedef struct no_arvore {
    int tipoNo;           // -1: folha, 0: raiz, 1: intermediário
    int nroChaves;        // Número de chaves no nó
    int P[ORDEM_ARVORE];  // Ponteiros para filhos (RRNs)
    CHAVE_PTR dados[ORDEM_ARVORE - 1]; // Chaves e ponteiros para registros
} NO_ARVORE;

// Estrutura para retorno de inserção (para controlar splits)
typedef struct resultado_insercao {
    bool promoveu;        // Se houve promoção de chave
    int chave_promovida;
    long long int ptr_promovido;
    int rrn_promovido;    // RRN do novo nó criado
} RESULTADO_INSERCAO;

// Funções básicas da árvore-B
CABECALHO_INDICE* inicializa_cabecalho_indice();
NO_ARVORE* inicializa_no();
void escreve_cabecalho_indice(FILE *indice, CABECALHO_INDICE *cab);
void le_cabecalho_indice(FILE *indice, CABECALHO_INDICE *cab);
void escreve_no(FILE *indice, NO_ARVORE *no, int rrn);
NO_ARVORE* le_no(FILE *indice, int rrn);

// Operações da árvore-B
bool busca_arvore_b(FILE *indice, int chave, long long int *ptr_resultado);
bool insere_arvore_b(FILE *indice, int chave, long long int ptr);
bool remove_arvore_b(FILE *indice, int chave);
bool atualiza_ponteiro_arvore_b(FILE *indice, int chave, long long int novo_ptr);

// Funções auxiliares
int busca_posicao_chave(NO_ARVORE *no, int chave);
void split_no(NO_ARVORE *no_original, NO_ARVORE *novo_no, int chave_nova, long long int ptr_novo, int filho_direito, int *chave_promovida, long long int *ptr_promovido);
void insere_chave_no(NO_ARVORE *no, int chave, long long int ptr, int filho_direito, int pos);

// Função para criar arquivo de índice
bool cria_arquivo_indice(char *nome_arquivo);


#endif // ARVORE_B_H 