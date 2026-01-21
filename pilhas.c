#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pilhas.h"

// ================= IMPLEMENTAÇÃO PILHA GENÉRICA =================

Pilha* criar_pilha() {
    Pilha* pilha = (Pilha*)malloc(sizeof(Pilha));
    if (pilha) {
        pilha->topo = NULL;
        pilha->tamanho = 0;
    }
    return pilha;
}

void destruir_pilha(Pilha* pilha) {
    if (!pilha) return;
    
    while (!pilha_vazia(pilha)) {
        // Desempilha mas não libera o dado
        desempilhar(pilha);
    }
    free(pilha);
}

int empilhar(Pilha* pilha, void* dado) {
    if (!pilha) return 0;
    
    NoPilha* novo_no = (NoPilha*)malloc(sizeof(NoPilha));
    if (!novo_no) return 0;
    
    novo_no->dado = dado;
    novo_no->abaixo = pilha->topo;
    pilha->topo = novo_no;
    pilha->tamanho++;
    
    return 1;
}

void* desempilhar(Pilha* pilha) {
    if (!pilha || pilha_vazia(pilha)) return NULL;
    
    NoPilha* no_removido = pilha->topo;
    void* dado = no_removido->dado;
    
    pilha->topo = no_removido->abaixo;
    free(no_removido);
    pilha->tamanho--;
    
    return dado;
}

void* topo_pilha(Pilha* pilha) {
    if (!pilha || pilha_vazia(pilha)) return NULL;
    return pilha->topo->dado;
}

bool pilha_vazia(Pilha* pilha) {
    return (!pilha || pilha->tamanho == 0);
}

int tamanho_pilha(Pilha* pilha) {
    return pilha ? pilha->tamanho : 0;
}

void imprimir_tamanho_pilha(Pilha* pilha, const char* nome_pilha) {
    printf("%s: %d elemento(s)\n", nome_pilha, tamanho_pilha(pilha));
}

// ================= IMPLEMENTAÇÃO PILHA DE VOTOS =================

Pilha* criar_pilha_votos() {
    return criar_pilha(); // Reutiliza a pilha genérica
}

Voto* criar_voto(int id_eleitor, int numero_candidato, int id_cap, int numero_urna) {
    Voto* voto = (Voto*)malloc(sizeof(Voto));
    if (voto) {
        voto->id_eleitor = id_eleitor;
        voto->numero_candidato = numero_candidato;
        voto->id_cap = id_cap;
        voto->numero_urna = numero_urna;
        voto->timestamp = time(NULL);
    }
    return voto;
}

void destruir_voto(Voto* voto) {
    if (voto) free(voto);
}

int empilhar_voto(Pilha* pilha, Voto* voto) {
    return empilhar(pilha, (void*)voto);
}

Voto* desempilhar_voto(Pilha* pilha) {
    return (Voto*)desempilhar(pilha);
}

void imprimir_voto(Voto* voto) {
    if (!voto) {
        printf("Voto invalido!\n");
        return;
    }
    
    char timestamp_str[50];
    struct tm* timeinfo = localtime(&voto->timestamp);
    strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    printf("Voto registrado:\n");
    printf("  Eleitor ID: %d\n", voto->id_eleitor);
    printf("  Candidato: %d\n", voto->numero_candidato);
    printf("  CAP: %d\n", voto->id_cap);
    printf("  Urna: %d\n", voto->numero_urna);
    printf("  Data/Hora: %s\n", timestamp_str);
}

void imprimir_pilha_votos(Pilha* pilha) {
    if (!pilha) {
        printf("Pilha de votos invalida!\n");
        return;
    }
    
    printf("=== PILHA DE VOTOS (%d votos) ===\n", tamanho_pilha(pilha));
    
    if (pilha_vazia(pilha)) {
        printf("  [VAZIA]\n");
        return;
    }
    
    // Para não destruir a pilha, criamos uma temporária
    Pilha* temp = criar_pilha();
    int contador = 1;
    
    while (!pilha_vazia(pilha)) {
        Voto* voto = desempilhar_voto(pilha);
        printf("%d. ", contador++);
        imprimir_voto(voto);
        empilhar_voto(temp, voto);
    }
    
    // Restaurar a pilha original
    while (!pilha_vazia(temp)) {
        Voto* voto = desempilhar_voto(temp);
        empilhar_voto(pilha, voto);
    }
    
    destruir_pilha(temp);
}

int salvar_pilha_votos_arquivo(Pilha* pilha, const char* filename) {
    if (!pilha || !filename) return 0;
    
    FILE* arquivo = fopen(filename, "wb");
    if (!arquivo) return 0;
    
    // Salvar número de votos
    int num_votos = tamanho_pilha(pilha);
    fwrite(&num_votos, sizeof(int), 1, arquivo);
    
    // Criar pilha temporária para inverter a ordem
    Pilha* temp = criar_pilha();
    
    // Transferir votos para pilha temporária (inverte ordem)
    while (!pilha_vazia(pilha)) {
        Voto* voto = desempilhar_voto(pilha);
        empilhar_voto(temp, voto);
    }
    
    // Salvar votos (do mais antigo ao mais recente)
    while (!pilha_vazia(temp)) {
        Voto* voto = desempilhar_voto(temp);
        fwrite(voto, sizeof(Voto), 1, arquivo);
        empilhar_voto(pilha, voto); // Restaurar na pilha original
    }
    
    destruir_pilha(temp);
    fclose(arquivo);
    
    printf("Pilha de votos salva em %s (%d votos)\n", filename, num_votos);
    return 1;
}

Pilha* carregar_pilha_votos_arquivo(const char* filename) {
    if (!filename) return NULL;
    
    FILE* arquivo = fopen(filename, "rb");
    if (!arquivo) return NULL;
    
    // Ler número de votos
    int num_votos;
    if (fread(&num_votos, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    Pilha* pilha = criar_pilha_votos();
    
    // Ler votos e empilhar (o último lido será o topo)
    for (int i = 0; i < num_votos; i++) {
        Voto* voto = (Voto*)malloc(sizeof(Voto));
        if (fread(voto, sizeof(Voto), 1, arquivo) != 1) {
            free(voto);
            destruir_pilha(pilha);
            fclose(arquivo);
            return NULL;
        }
        empilhar_voto(pilha, voto);
    }
    
    fclose(arquivo);
    printf("Pilha de votos carregada de %s (%d votos)\n", filename, num_votos);
    return pilha;
}

// ================= IMPLEMENTAÇÃO PILHA DE OPERAÇÕES =================

Pilha* criar_pilha_operacoes() {
    return criar_pilha();
}

Operacao* criar_operacao(const char* tipo, const char* descricao, int usuario_id) {
    Operacao* operacao = (Operacao*)malloc(sizeof(Operacao));
    if (operacao) {
        strncpy(operacao->tipo, tipo, sizeof(operacao->tipo) - 1);
        strncpy(operacao->descricao, descricao, sizeof(operacao->descricao) - 1);
        operacao->timestamp = time(NULL);
        operacao->usuario_id = usuario_id;
    }
    return operacao;
}

void destruir_operacao(Operacao* operacao) {
    if (operacao) free(operacao);
}

int registrar_operacao(Pilha* pilha, const char* tipo, const char* descricao, int usuario_id) {
    Operacao* operacao = criar_operacao(tipo, descricao, usuario_id);
    if (!operacao) return 0;
    
    // Limitar histórico a 1000 operações
    if (tamanho_pilha(pilha) > 1000) {
        // Remover a operação mais antiga (fundo da pilha)
        // Para fazer isso eficientemente, precisaríamos de uma fila
        // Implementação simplificada: criar nova pilha sem o último
        Pilha* temp = criar_pilha();
        
        // Transferir todas menos a última
        while (tamanho_pilha(pilha) > 1) {
            Operacao* op = (Operacao*)desempilhar(pilha);
            empilhar(temp, op);
        }
        
        // Liberar a operação mais antiga
        Operacao* antiga = (Operacao*)desempilhar(pilha);
        destruir_operacao(antiga);
        
        // Restaurar as outras
        while (!pilha_vazia(temp)) {
            Operacao* op = (Operacao*)desempilhar(temp);
            empilhar(pilha, op);
        }
        
        destruir_pilha(temp);
    }
    
    return empilhar(pilha, operacao);
}

void imprimir_operacao(Operacao* operacao) {
    if (!operacao) {
        printf("Operacao invalida!\n");
        return;
    }
    
    char timestamp_str[50];
    struct tm* timeinfo = localtime(&operacao->timestamp);
    strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    printf("[%s] %s - %s (Usuario: %d)\n", 
           timestamp_str, 
           operacao->tipo, 
           operacao->descricao, 
           operacao->usuario_id);
}

void imprimir_historico_operacoes(Pilha* pilha, int limite) {
    if (!pilha) {
        printf("Pilha de operacoes invalida!\n");
        return;
    }
    
    int total = tamanho_pilha(pilha);
    if (limite <= 0 || limite > total) {
        limite = total;
    }
    
    printf("=== HISTORICO DE OPERACOES (%d/%d mostradas) ===\n", limite, total);
    
    if (pilha_vazia(pilha)) {
        printf("  [VAZIO]\n");
        return;
    }
    
    // Para não destruir a pilha, criamos uma temporária
    Pilha* temp = criar_pilha();
    int contador = 0;
    
    while (!pilha_vazia(pilha) && contador < limite) {
        Operacao* operacao = (Operacao*)desempilhar(pilha);
        imprimir_operacao(operacao);
        empilhar(temp, operacao);
        contador++;
    }
    
    // Restaurar a pilha original
    while (!pilha_vazia(temp)) {
        Operacao* operacao = (Operacao*)desempilhar(temp);
        empilhar(pilha, operacao);
    }
    
    destruir_pilha(temp);
}

void limpar_historico(Pilha* pilha) {
    if (!pilha) return;
    
    while (!pilha_vazia(pilha)) {
        Operacao* operacao = (Operacao*)desempilhar(pilha);
        destruir_operacao(operacao);
    }
}

// ================= IMPLEMENTAÇÃO PILHA DE ELEITORES ATENDIDOS =================

Pilha* criar_pilha_eleitores_atendidos() {
    return criar_pilha();
}

int empilhar_eleitor_atendido(Pilha* pilha, Eleitor* eleitor) {
    return empilhar(pilha, (void*)eleitor);
}

Eleitor* desempilhar_eleitor_atendido(Pilha* pilha) {
    return (Eleitor*)desempilhar(pilha);
}

void imprimir_eleitores_atendidos(Pilha* pilha) {
    if (!pilha) {
        printf("Pilha de eleitores atendidos invalida!\n");
        return;
    }
    
    printf("=== ULTIMOS ELEITORES ATENDIDOS (%d) ===\n", tamanho_pilha(pilha));
    
    if (pilha_vazia(pilha)) {
        printf("  [VAZIA]\n");
        return;
    }
    
    Pilha* temp = criar_pilha();
    int contador = 1;
    
    while (!pilha_vazia(pilha)) {
        Eleitor* eleitor = desempilhar_eleitor_atendido(pilha);
        printf("%d. %s (ID: %d, CAP: %d)\n", 
               contador++, 
               eleitor->nome, 
               eleitor->id, 
               eleitor->id_cap);
        empilhar_eleitor_atendido(temp, eleitor);
    }
    
    // Restaurar a pilha original
    while (!pilha_vazia(temp)) {
        Eleitor* eleitor = desempilhar_eleitor_atendido(temp);
        empilhar_eleitor_atendido(pilha, eleitor);
    }
    
    destruir_pilha(temp);
}
