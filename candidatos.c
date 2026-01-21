// Inclusão das bibliotecas necessárias
#include <stdio.h>  // Para funções de entrada/saída
#include <stdlib.h> // Para alocação de memória
#include <string.h> // Para manipulação de strings
#include "candidatos.h" // Cabeçalho das funções de candidatos

// Função que inicializa uma lista de candidatos
void inicializar_lista_candidatos(ListaCandidatos* lista, int capacidade) {
    // Aloca memória para o array de candidatos
    lista->candidatos = (Candidato*)malloc(capacidade * sizeof(Candidato));
    // Inicializa tamanho como 0 (lista vazia)
    lista->tamanho = 0;
    // Define a capacidade inicial
    lista->capacidade = capacidade;
}

// Função que insere um novo candidato na lista
int inserir_candidato(ListaCandidatos* lista, Candidato novo_candidato) {
    // Validação: evitar duplicados por número
    if (candidato_existe(lista, novo_candidato.numero)) {
        printf("Erro: Candidato com numero %d ja existe!\n", novo_candidato.numero);
        return 0; // Retorna falha
    }
    
    // Validação: número deve ser positivo
    if (!numero_candidato_valido(novo_candidato.numero)) {
        printf("Erro: Numero de candidato invalido!\n");
        return 0; // Retorna falha
    }
    
    // Redimensionar se necessário (lista cheia)
    if (lista->tamanho >= lista->capacidade) {
        // Dobra a capacidade
        redimensionar_lista_candidatos(lista, lista->capacidade * 2);
    }
    
    // Insere o novo candidato na próxima posição disponível
    lista->candidatos[lista->tamanho] = novo_candidato;
    // Inicializa contador de votos como 0
    lista->candidatos[lista->tamanho].votos = 0;
    // Incrementa tamanho da lista
    lista->tamanho++;
    
    return 1; // Retorna sucesso
}

// Função que remove um candidato da lista pelo número
int remover_candidato_por_numero(ListaCandidatos* lista, int numero) {
    // Percorre a lista procurando o candidato
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->candidatos[i].numero == numero) {
            // Move elementos para preencher o espaço
            for (int j = i; j < lista->tamanho - 1; j++) {
                lista->candidatos[j] = lista->candidatos[j + 1];
            }
            // Decrementa tamanho
            lista->tamanho--;
            return 1; // Retorna sucesso
        }
    }
    // Se chegou aqui, candidato não encontrado
    printf("Candidato com numero %d nao encontrado!\n", numero);
    return 0; // Retorna falha
}

// Função que busca um candidato pelo número
Candidato* buscar_candidato_por_numero(ListaCandidatos* lista, int numero) {
    // Percorre a lista procurando o candidato
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->candidatos[i].numero == numero) {
            return &lista->candidatos[i]; // Retorna ponteiro para o candidato
        }
    }
    return NULL; // Retorna NULL se não encontrado
}

// Função que lista todos os candidatos
void listar_candidatos(ListaCandidatos* lista) {
    printf("\n=== LISTA DE CANDIDATOS ===\n");
    printf("Total: %d candidatos\n\n", lista->tamanho);
    
    // Percorre e exibe todos os candidatos
    for (int i = 0; i < lista->tamanho; i++) {
        printf("Candidato #%d:\n", i+1);
        printf("  Numero: %d\n", lista->candidatos[i].numero);
        printf("  Nome: %s\n", lista->candidatos[i].nome);
        printf("  Partido: %s\n", lista->candidatos[i].partido);
        printf("  Regiao: %s\n", lista->candidatos[i].regiao);
        printf("  Votos: %d\n", lista->candidatos[i].votos);
        printf("------------------------\n");
    }
}

// Função que lista candidatos por partido
void listar_candidatos_por_partido(ListaCandidatos* lista, const char* partido) {
    printf("\n=== CANDIDATOS DO PARTIDO: %s ===\n", partido);
    int encontrados = 0; // Contador de candidatos encontrados
    
    // Percorre a lista procurando candidatos do partido
    for (int i = 0; i < lista->tamanho; i++) {
        if (strcmp(lista->candidatos[i].partido, partido) == 0) {
            printf("Numero: %d | Nome: %s | Regiao: %s | Votos: %d\n",
                   lista->candidatos[i].numero,
                   lista->candidatos[i].nome,
                   lista->candidatos[i].regiao,
                   lista->candidatos[i].votos);
            encontrados++;
        }
    }
    
    // Se não encontrou nenhum
    if (encontrados == 0) {
        printf("Nenhum candidato encontrado para este partido.\n");
    }
}

// Função que verifica se um candidato existe pelo número
int candidato_existe(ListaCandidatos* lista, int numero) {
    // Reutiliza função de busca
    return buscar_candidato_por_numero(lista, numero) != NULL;
}

// Função que valida se um número de candidato é válido
int numero_candidato_valido(int numero) {
    return numero > 0; // Número deve ser positivo
}

// Função que libera a memória da lista de candidatos
void liberar_lista_candidatos(ListaCandidatos* lista) {
    free(lista->candidatos); // Libera array de candidatos
    lista->candidatos = NULL; // Define ponteiro como NULL
    lista->tamanho = 0; // Zera tamanho
    lista->capacidade = 0; // Zera capacidade
}

// Função que redimensiona a lista de candidatos
void redimensionar_lista_candidatos(ListaCandidatos* lista, int nova_capacidade) {
    // Realoca memória para nova capacidade
    Candidato* nova_lista = (Candidato*)realloc(lista->candidatos, nova_capacidade * sizeof(Candidato));
    if (nova_lista) {
        lista->candidatos = nova_lista; // Atualiza ponteiro
        lista->capacidade = nova_capacidade; // Atualiza capacidade
    } else {
        printf("Erro ao redimensionar lista de candidatos!\n");
    }
}

// Função que calcula o total de votos de todos os candidatos
int total_votos_candidatos(ListaCandidatos* lista) {
    int total = 0;
    // Soma votos de todos os candidatos
    for (int i = 0; i < lista->tamanho; i++) {
        total += lista->candidatos[i].votos;
    }
    return total;
}

// Função que encontra o candidato mais votado
Candidato* candidato_mais_votado(ListaCandidatos* lista) {
    // Verifica se lista está vazia
    if (lista->tamanho == 0) return NULL;
    
    // Assume primeiro candidato como mais votado inicialmente
    Candidato* mais_votado = &lista->candidatos[0];
    
    // Percorre lista procurando candidato com mais votos
    for (int i = 1; i < lista->tamanho; i++) {
        if (lista->candidatos[i].votos > mais_votado->votos) {
            mais_votado = &lista->candidatos[i];
        }
    }
    return mais_votado;
}
