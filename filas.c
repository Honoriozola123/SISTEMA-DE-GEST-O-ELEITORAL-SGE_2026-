#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filas.h"

// ================= IMPLEMENTAÇÃO FILA NORMAL =================

Fila* criar_fila() {
    Fila* fila = (Fila*)malloc(sizeof(Fila));
    if (fila) {
        fila->frente = NULL;
        fila->tras = NULL;
        fila->tamanho = 0;
    }
    return fila;
}

void destruir_fila(Fila* fila) {
    if (!fila) return;
    
    while (!fila_vazia(fila)) {
        // Desenfileira mas não libera o eleitor (será liberado por quem o criou)
        desenfileirar(fila);
    }
    free(fila);
}

int enfileirar(Fila* fila, Eleitor* eleitor) {
    if (!fila || !eleitor) return 0;
    
    NoFila* novo_no = (NoFila*)malloc(sizeof(NoFila));
    if (!novo_no) return 0;
    
    novo_no->eleitor = eleitor;
    novo_no->proximo = NULL;
    
    if (fila_vazia(fila)) {
        fila->frente = novo_no;
    } else {
        fila->tras->proximo = novo_no;
    }
    
    fila->tras = novo_no;
    fila->tamanho++;
    
    return 1;
}

Eleitor* desenfileirar(Fila* fila) {
    if (!fila || fila_vazia(fila)) return NULL;
    
    NoFila* no_removido = fila->frente;
    Eleitor* eleitor = no_removido->eleitor;
    
    fila->frente = fila->frente->proximo;
    
    if (fila->frente == NULL) {
        fila->tras = NULL;
    }
    
    free(no_removido);
    fila->tamanho--;
    
    return eleitor;
}

Eleitor* frente_fila(Fila* fila) {
    if (!fila || fila_vazia(fila)) return NULL;
    return fila->frente->eleitor;
}

bool fila_vazia(Fila* fila) {
    return (!fila || fila->tamanho == 0);
}

int tamanho_fila(Fila* fila) {
    return fila ? fila->tamanho : 0;
}

void imprimir_fila(Fila* fila) {
    if (!fila) {
        printf("Fila invalida!\n");
        return;
    }
    
    printf("Fila (%d eleitores):\n", fila->tamanho);
    
    if (fila_vazia(fila)) {
        printf("  [VAZIA]\n");
        return;
    }
    
    NoFila* atual = fila->frente;
    int posicao = 1;
    
    while (atual != NULL) {
        printf("  %d. %s (ID: %d", posicao++, atual->eleitor->nome, atual->eleitor->id);
        
        if (atual->eleitor->prioridade != PRIORIDADE_NORMAL) {
            printf(", PRIORIDADE: ");
            switch (atual->eleitor->prioridade) {
                case PRIORIDADE_ALTA: printf("ALTA"); break;
                case PRIORIDADE_URGENTE: printf("URGENTE"); break;
                default: printf("DESCONHECIDA");
            }
        }
        
        printf(")\n");
        atual = atual->proximo;
    }
}

int mover_eleitor_fila(Fila* origem, Fila* destino, int id_eleitor) {
    if (!origem || !destino || fila_vazia(origem)) return 0;
    
    // Se o eleitor estiver na frente, basta desenfileirar e enfileirar
    Eleitor* frente = frente_fila(origem);
    if (frente && frente->id == id_eleitor) {
        Eleitor* eleitor = desenfileirar(origem);
        if (eleitor) {
            return enfileirar(destino, eleitor);
        }
    }
    
    // Buscar em outras posições (mais complexo)
    Fila* temp = criar_fila();
    int encontrado = 0;
    
    while (!fila_vazia(origem)) {
        Eleitor* eleitor = desenfileirar(origem);
        if (eleitor->id == id_eleitor) {
            enfileirar(destino, eleitor);
            encontrado = 1;
            break;
        } else {
            enfileirar(temp, eleitor);
        }
    }
    
    // Restaurar a fila original
    while (!fila_vazia(temp)) {
        enfileirar(origem, desenfileirar(temp));
    }
    
    destruir_fila(temp);
    return encontrado;
}

int buscar_na_fila(Fila* fila, int id_eleitor) {
    if (!fila || fila_vazia(fila)) return 0;
    
    NoFila* atual = fila->frente;
    int posicao = 1;
    
    while (atual != NULL) {
        if (atual->eleitor->id == id_eleitor) {
            return posicao;
        }
        atual = atual->proximo;
        posicao++;
    }
    
    return 0; // Não encontrado
}

void limpar_fila(Fila* fila) {
    if (!fila) return;
    
    while (!fila_vazia(fila)) {
        desenfileirar(fila);
    }
}

// ================= IMPLEMENTAÇÃO FILA DE PRIORIDADE AVANÇADA =================

FilaPrioritaria* criar_fila_prioritaria_avancada() {
    FilaPrioritaria* fila = (FilaPrioritaria*)malloc(sizeof(FilaPrioritaria));
    if (fila) {
        fila->frente = NULL;
        fila->tras = NULL;
        fila->tamanho = 0;
    }
    return fila;
}

void destruir_fila_prioritaria_avancada(FilaPrioritaria* fila) {
    if (!fila) return;
    
    NoFilaPrioritaria* atual = fila->frente;
    while (atual != NULL) {
        NoFilaPrioritaria* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    free(fila);
}

int enfileirar_com_prioridade(FilaPrioritaria* fila, Eleitor* eleitor) {
    if (!fila || !eleitor) return 0;
    
    NoFilaPrioritaria* novo_no = (NoFilaPrioritaria*)malloc(sizeof(NoFilaPrioritaria));
    if (!novo_no) return 0;
    
    novo_no->eleitor = eleitor;
    novo_no->prioridade = eleitor->prioridade;
    novo_no->proximo = NULL;
    
    // Se a fila está vazia ou o novo elemento tem prioridade mais alta que todos
    if (fila->frente == NULL || novo_no->prioridade > fila->frente->prioridade) {
        novo_no->proximo = fila->frente;
        fila->frente = novo_no;
        
        if (fila->tras == NULL) {
            fila->tras = novo_no;
        }
    } else {
        // Encontrar a posição correta baseada na prioridade
        NoFilaPrioritaria* atual = fila->frente;
        NoFilaPrioritaria* anterior = NULL;
        
        while (atual != NULL && atual->prioridade >= novo_no->prioridade) {
            anterior = atual;
            atual = atual->proximo;
        }
        
        if (anterior == NULL) {
            // Inserir no início
            novo_no->proximo = fila->frente;
            fila->frente = novo_no;
        } else {
            // Inserir no meio ou fim
            anterior->proximo = novo_no;
            novo_no->proximo = atual;
            
            if (atual == NULL) {
                fila->tras = novo_no;
            }
        }
    }
    
    fila->tamanho++;
    return 1;
}

Eleitor* desenfileirar_com_prioridade(FilaPrioritaria* fila) {
    if (!fila || fila->frente == NULL) return NULL;
    
    NoFilaPrioritaria* no_removido = fila->frente;
    Eleitor* eleitor = no_removido->eleitor;
    
    fila->frente = fila->frente->proximo;
    
    if (fila->frente == NULL) {
        fila->tras = NULL;
    }
    
    free(no_removido);
    fila->tamanho--;
    
    return eleitor;
}

void imprimir_fila_prioritaria(FilaPrioritaria* fila) {
    if (!fila) {
        printf("Fila prioritaria invalida!\n");
        return;
    }
    
    printf("Fila Prioritaria (%d eleitores):\n", fila->tamanho);
    
    if (fila->frente == NULL) {
        printf("  [VAZIA]\n");
        return;
    }
    
    NoFilaPrioritaria* atual = fila->frente;
    int posicao = 1;
    
    while (atual != NULL) {
        const char* str_prioridade;
        switch (atual->prioridade) {
            case PRIORIDADE_NORMAL: str_prioridade = "NORMAL"; break;
            case PRIORIDADE_ALTA: str_prioridade = "ALTA"; break;
            case PRIORIDADE_URGENTE: str_prioridade = "URGENTE"; break;
            default: str_prioridade = "DESCONHECIDA";
        }
        
        printf("  %d. %s (ID: %d, PRIORIDADE: %s)\n", 
               posicao++, 
               atual->eleitor->nome, 
               atual->eleitor->id, 
               str_prioridade);
        
        atual = atual->proximo;
    }
}

// ================= GERENCIAMENTO DE CAP =================

int adicionar_eleitor_cap_fila(CAP* cap, Eleitor* eleitor) {
    if (!cap || !eleitor) return 0;
    
    // Determinar se vai para fila normal ou prioritária
    if (eleitor->prioridade == PRIORIDADE_NORMAL) {
        if (!cap->fila_normal) {
            cap->fila_normal = criar_fila();
        }
        if (enfileirar(cap->fila_normal, eleitor)) {
            cap->eleitores_na_fila++;
            return 1;
        }
    } else {
        if (!cap->fila_prioritaria) {
            cap->fila_prioritaria = criar_fila();
        }
        if (enfileirar(cap->fila_prioritaria, eleitor)) {
            cap->eleitores_na_fila++;
            return 1;
        }
    }
    
    return 0;
}

Eleitor* proximo_eleitor_cap(CAP* cap) {
    if (!cap) return NULL;
    
    // Primeiro atender prioritários
    if (cap->fila_prioritaria && !fila_vazia(cap->fila_prioritaria)) {
        cap->eleitores_na_fila--;
        cap->eleitores_atendidos++;
        return desenfileirar(cap->fila_prioritaria);
    }
    
    // Depois atender normais
    if (cap->fila_normal && !fila_vazia(cap->fila_normal)) {
        cap->eleitores_na_fila--;
        cap->eleitores_atendidos++;
        return desenfileirar(cap->fila_normal);
    }
    
    return NULL;
}

int total_eleitores_na_fila_cap(CAP* cap) {
    if (!cap) return 0;
    
    int total = 0;
    if (cap->fila_normal) total += tamanho_fila(cap->fila_normal);
    if (cap->fila_prioritaria) total += tamanho_fila(cap->fila_prioritaria);
    
    return total;
}

int eleitores_prioritarios_na_fila_cap(CAP* cap) {
    if (!cap || !cap->fila_prioritaria) return 0;
    return tamanho_fila(cap->fila_prioritaria);
}

void imprimir_filas_cap(CAP* cap) {
    if (!cap) {
        printf("CAP invalido!\n");
        return;
    }
    
    printf("\n=== FILAS DO CAP %d ===\n", cap->id);
    printf("Localizacao: %s\n", cap->localizacao);
    
    printf("\nFila Prioritária:\n");
    if (cap->fila_prioritaria && !fila_vazia(cap->fila_prioritaria)) {
        imprimir_fila(cap->fila_prioritaria);
    } else {
        printf("  [VAZIA]\n");
    }
    
    printf("\nFila Normal:\n");
    if (cap->fila_normal && !fila_vazia(cap->fila_normal)) {
        imprimir_fila(cap->fila_normal);
    } else {
        printf("  [VAZIA]\n");
    }
    
    printf("\nEstatisticas:\n");
    printf("  Total na fila: %d\n", total_eleitores_na_fila_cap(cap));
    printf("  Prioritarios: %d\n", eleitores_prioritarios_na_fila_cap(cap));
    printf("  Eleitores atendidos: %d\n", cap->eleitores_atendidos);
}

void simular_processamento_fila(CAP* cap, int num_eleitores_processar) {
    if (!cap || num_eleitores_processar <= 0) return;
    
    printf("\n=== SIMULACAO DE PROCESSAMENTO ===\n");
    printf("Processando %d eleitores do CAP %d...\n\n", num_eleitores_processar, cap->id);
    
    for (int i = 0; i < num_eleitores_processar; i++) {
        Eleitor* eleitor = proximo_eleitor_cap(cap);
        if (eleitor) {
            printf("%d. Atendendo: %s (ID: %d", i+1, eleitor->nome, eleitor->id);
            
            if (eleitor->prioridade != PRIORIDADE_NORMAL) {
                printf(", PRIORITARIO");
            }
            
            printf(")\n");
            
            // Simular tempo de processamento
            // (na implementação real, aqui seria o voto)
        } else {
            printf("Nao ha mais eleitores na fila.\n");
            break;
        }
    }
    
    printf("\nSimulacao concluida.\n");
    printf("Eleitores restantes na fila: %d\n", total_eleitores_na_fila_cap(cap));
}
