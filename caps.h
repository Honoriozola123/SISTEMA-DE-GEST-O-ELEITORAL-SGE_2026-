// Diretiva de pré-processador para evitar inclusões múltiplas
#ifndef CAPS_H
#define CAPS_H

// Inclui o cabeçalho com as estruturas de dados básicas
#include "estruturas.h"

// Inicialização
// Protótipo da função que inicializa lista de CAPs
void inicializar_lista_caps(ListaCAPs* lista, int capacidade);

// Operações básicas
// Protótipo da função que insere CAP
int inserir_cap(ListaCAPs* lista, CAP novo_cap);
// Protótipo da função que remove CAP por ID
int remover_cap_por_id(ListaCAPs* lista, int id);
// Protótipo da função que busca CAP por ID
CAP* buscar_cap_por_id(ListaCAPs* lista, int id);

// Listagem e exibição
// Protótipo da função que lista todos os CAPs
void listar_caps(ListaCAPs* lista);
// Protótipo da função que lista CAPs por localização
void listar_caps_por_localizacao(ListaCAPs* lista, const char* localizacao);

// Validação
// Protótipo da função que verifica se CAP existe
int cap_existe(ListaCAPs* lista, int id);
// Protótipo da função que calcula capacidade total de um CAP
int capacidade_total_cap(CAP* cap);

// Gerenciamento de eleitores no CAP (versão básica - será expandida com filas)
// Protótipo da função que adiciona eleitor ao CAP
int adicionar_eleitor_cap(CAP* cap, Eleitor* eleitor);
// Protótipo da função que remove eleitor do CAP
int remover_eleitor_cap(CAP* cap, int id_eleitor);

// Gerenciamento de memória
// Protótipo da função que libera lista de CAPs
void liberar_lista_caps(ListaCAPs* lista);
// Protótipo da função que redimensiona lista de CAPs
void redimensionar_lista_caps(ListaCAPs* lista, int nova_capacidade);

// Estatísticas
// Protótipo da função que calcula capacidade total de eleitores
int total_capacidade_eleitores(ListaCAPs* lista);

// Fim da diretiva de pré-processador
#endif
