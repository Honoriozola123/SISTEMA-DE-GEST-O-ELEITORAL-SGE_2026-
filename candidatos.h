// Diretiva de pré-processador para evitar inclusões múltiplas
#ifndef CANDIDATOS_H
#define CANDIDATOS_H

// Inclui o cabeçalho com as estruturas de dados básicas
#include "estruturas.h"

// Inicialização
// Protótipo da função que inicializa lista de candidatos
void inicializar_lista_candidatos(ListaCandidatos* lista, int capacidade);

// Operações básicas
// Protótipo da função que insere candidato
int inserir_candidato(ListaCandidatos* lista, Candidato novo_candidato);
// Protótipo da função que remove candidato por número
int remover_candidato_por_numero(ListaCandidatos* lista, int numero);
// Protótipo da função que busca candidato por número
Candidato* buscar_candidato_por_numero(ListaCandidatos* lista, int numero);

// Listagem e exibição
// Protótipo da função que lista todos os candidatos
void listar_candidatos(ListaCandidatos* lista);
// Protótipo da função que lista candidatos por partido
void listar_candidatos_por_partido(ListaCandidatos* lista, const char* partido);
// Protótipo da função que lista candidatos por região
void listar_candidatos_por_regiao(ListaCandidatos* lista, const char* regiao);

// Validação
// Protótipo da função que verifica se candidato existe
int candidato_existe(ListaCandidatos* lista, int numero);
// Protótipo da função que valida número de candidato
int numero_candidato_valido(int numero);

// Gerenciamento de memória
// Protótipo da função que libera lista de candidatos
void liberar_lista_candidatos(ListaCandidatos* lista);
// Protótipo da função que redimensiona lista de candidatos
void redimensionar_lista_candidatos(ListaCandidatos* lista, int nova_capacidade);

// Estatísticas
// Protótipo da função que calcula total de votos dos candidatos
int total_votos_candidatos(ListaCandidatos* lista);
// Protótipo da função que encontra candidato mais votado
Candidato* candidato_mais_votado(ListaCandidatos* lista);

// Fim da diretiva de pré-processador
#endif
