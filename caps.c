#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caps.h"
#include "filas.h"  // Adicionar esta linha

void inicializar_lista_caps(ListaCAPs* lista, int capacidade) {
    lista->caps = (CAP*)malloc(capacidade * sizeof(CAP));
    lista->tamanho = 0;
    lista->capacidade = capacidade;
}

int inserir_cap(ListaCAPs* lista, CAP novo_cap) {
    // Validação: evitar duplicados
    if (cap_existe(lista, novo_cap.id)) {
        printf("Erro: CAP com ID %d ja existe!\n", novo_cap.id);
        return 0;
    }
    
    // Validação: ID positivo
    if (novo_cap.id <= 0) {
        printf("Erro: ID do CAP invalido!\n");
        return 0;
    }
    
    // Redimensionar se necessário
    if (lista->tamanho >= lista->capacidade) {
        redimensionar_lista_caps(lista, lista->capacidade * 2);
    }
    
    // Inicializar filas (usando a nova estrutura)
    novo_cap.fila_normal = criar_fila();
    novo_cap.fila_prioritaria = criar_fila();
    novo_cap.urnas = NULL; // Será alocado quando necessário
    novo_cap.resultados_cap = NULL;
    
    // Inserir
    lista->caps[lista->tamanho] = novo_cap;
    lista->tamanho++;
    
    return 1;
}

int remover_cap_por_id(ListaCAPs* lista, int id) {
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->caps[i].id == id) {
            // Liberar filas de eleitores
            if (lista->caps[i].fila_normal) {
                destruir_fila(lista->caps[i].fila_normal);
            }
            if (lista->caps[i].fila_prioritaria) {
                destruir_fila(lista->caps[i].fila_prioritaria);
            }
            
            // Liberar urnas se existirem
            if (lista->caps[i].urnas) {
                for (int j = 0; j < lista->caps[i].num_urnas; j++) {
                    if (lista->caps[i].urnas[j]) {
                        // Destruir urna se tivéssemos a função
                        free(lista->caps[i].urnas[j]);
                    }
                }
                free(lista->caps[i].urnas);
            }
            
            // Liberar resultados se existirem
            if (lista->caps[i].resultados_cap) {
                // Destruir árvore se tivéssemos a função
                // destruir_arvore_resultados(lista->caps[i].resultados_cap);
            }
            
            // Mover elementos para preencher o espaço
            for (int j = i; j < lista->tamanho - 1; j++) {
                lista->caps[j] = lista->caps[j + 1];
            }
            lista->tamanho--;
            return 1;
        }
    }
    printf("CAP com ID %d nao encontrado!\n", id);
    return 0;
}

CAP* buscar_cap_por_id(ListaCAPs* lista, int id) {
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->caps[i].id == id) {
            return &lista->caps[i];
        }
    }
    return NULL;
}

void listar_caps(ListaCAPs* lista) {
    printf("\n=== LISTA DE CAPs ===\n");
    printf("Total: %d CAPs\n\n", lista->tamanho);
    
    for (int i = 0; i < lista->tamanho; i++) {
        printf("CAP #%d:\n", i+1);
        printf("  ID: %d\n", lista->caps[i].id);
        printf("  Localizacao: %s\n", lista->caps[i].localizacao);
        printf("  Regiao: %s\n", lista->caps[i].regiao);
        printf("  Numero de urnas: %d\n", lista->caps[i].num_urnas);
        printf("  Max eleitores/urna: %d\n", lista->caps[i].max_eleitores_por_urna);
        printf("  Capacidade total: %d eleitores\n", 
               lista->caps[i].num_urnas * lista->caps[i].max_eleitores_por_urna);
        
        // Mostrar estatísticas das filas
        int total_fila = 0;
        int prioritarios = 0;
        
        if (lista->caps[i].fila_normal) {
            total_fila += tamanho_fila(lista->caps[i].fila_normal);
        }
        if (lista->caps[i].fila_prioritaria) {
            prioritarios = tamanho_fila(lista->caps[i].fila_prioritaria);
            total_fila += prioritarios;
        }
        
        printf("  Eleitores na fila: %d (Prioritarios: %d)\n", total_fila, prioritarios);
        printf("  Eleitores atendidos: %d\n", lista->caps[i].eleitores_atendidos);
        printf("------------------------\n");
    }
}

int cap_existe(ListaCAPs* lista, int id) {
    return buscar_cap_por_id(lista, id) != NULL;
}

// REMOVA ou COMENTE estas funções antigas que não são mais compatíveis:
/*
int adicionar_eleitor_cap(CAP* cap, Eleitor* eleitor) {
    // Implementação removida - use as funções do módulo filas
    return 0;
}

int remover_eleitor_cap(CAP* cap, int id_eleitor) {
    // Implementação removida
    return 0;
}
*/

void liberar_lista_caps(ListaCAPs* lista) {
    if (!lista || !lista->caps) return;
    
    for (int i = 0; i < lista->tamanho; i++) {
        // Liberar filas
        if (lista->caps[i].fila_normal) {
            destruir_fila(lista->caps[i].fila_normal);
        }
        if (lista->caps[i].fila_prioritaria) {
            destruir_fila(lista->caps[i].fila_prioritaria);
        }
        
        // Liberar urnas
        if (lista->caps[i].urnas) {
            for (int j = 0; j < lista->caps[i].num_urnas; j++) {
                if (lista->caps[i].urnas[j]) {
                    free(lista->caps[i].urnas[j]);
                }
            }
            free(lista->caps[i].urnas);
        }
    }
    
    free(lista->caps);
    lista->caps = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
}

void redimensionar_lista_caps(ListaCAPs* lista, int nova_capacidade) {
    CAP* nova_lista = (CAP*)realloc(lista->caps, nova_capacidade * sizeof(CAP));
    if (nova_lista) {
        lista->caps = nova_lista;
        lista->capacidade = nova_capacidade;
    } else {
        printf("Erro ao redimensionar lista de CAPs!\n");
    }
}

int total_capacidade_eleitores(ListaCAPs* lista) {
    int total = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        total += lista->caps[i].num_urnas * lista->caps[i].max_eleitores_por_urna;
    }
    return total;
}
