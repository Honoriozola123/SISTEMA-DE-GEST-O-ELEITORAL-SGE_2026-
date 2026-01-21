#ifndef FILAS_H
#define FILAS_H

#include "estruturas.h"

// ================= FILA NORMAL (FIFO) =================

// Inicialização e destruição
Fila* criar_fila();
void destruir_fila(Fila* fila);

// Operações básicas
int enfileirar(Fila* fila, Eleitor* eleitor);
Eleitor* desenfileirar(Fila* fila);
Eleitor* frente_fila(Fila* fila);

// Consultas
bool fila_vazia(Fila* fila);
int tamanho_fila(Fila* fila);
void imprimir_fila(Fila* fila);

// Operações especiais
int mover_eleitor_fila(Fila* origem, Fila* destino, int id_eleitor);
int buscar_na_fila(Fila* fila, int id_eleitor);
void limpar_fila(Fila* fila);

// ================= FILA DE PRIORIDADE =================

// Versão 1: Fila separada para prioritários
Fila* criar_fila_prioritaria();
int enfileirar_prioritario(Fila* fila_prioritaria, Fila* fila_normal, Eleitor* eleitor);

// Versão 2: Fila única com prioridade (mais complexa)
typedef struct NoFilaPrioritaria {
    Eleitor* eleitor;
    int prioridade; // 0=normal, 1=alto, 2=urgente
    struct NoFilaPrioritaria* proximo;
} NoFilaPrioritaria;

typedef struct FilaPrioritaria {
    NoFilaPrioritaria* frente;
    NoFilaPrioritaria* tras;
    int tamanho;
} FilaPrioritaria;

FilaPrioritaria* criar_fila_prioritaria_avancada();
void destruir_fila_prioritaria_avancada(FilaPrioritaria* fila);
int enfileirar_com_prioridade(FilaPrioritaria* fila, Eleitor* eleitor);
Eleitor* desenfileirar_com_prioridade(FilaPrioritaria* fila);
void imprimir_fila_prioritaria(FilaPrioritaria* fila);

// ================= GERENCIAMENTO DE CAP =================

// Funções específicas para gerenciamento de filas no CAP
int adicionar_eleitor_cap_fila(CAP* cap, Eleitor* eleitor);
Eleitor* proximo_eleitor_cap(CAP* cap);
int total_eleitores_na_fila_cap(CAP* cap);
int eleitores_prioritarios_na_fila_cap(CAP* cap);
void imprimir_filas_cap(CAP* cap);

// ================= SIMULAÇÃO DE VOTAÇÃO =================

void simular_processamento_fila(CAP* cap, int num_eleitores_processar);

#endif
