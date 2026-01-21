#ifndef PILHAS_H
#define PILHAS_H

#include "estruturas.h"

// ================= PILHA GENÉRICA (LIFO) =================

// Inicialização e destruição
Pilha* criar_pilha();
void destruir_pilha(Pilha* pilha);

// Operações básicas
int empilhar(Pilha* pilha, void* dado);
void* desempilhar(Pilha* pilha);
void* topo_pilha(Pilha* pilha);

// Consultas
bool pilha_vazia(Pilha* pilha);
int tamanho_pilha(Pilha* pilha);
void imprimir_tamanho_pilha(Pilha* pilha, const char* nome_pilha);

// ================= PILHA DE VOTOS =================

// Criação especializada
Pilha* criar_pilha_votos();

// Operações com votos
int empilhar_voto(Pilha* pilha, Voto* voto);
Voto* desempilhar_voto(Pilha* pilha);
Voto* criar_voto(int id_eleitor, int numero_candidato, int id_cap, int numero_urna);
void destruir_voto(Voto* voto);
void imprimir_voto(Voto* voto);
void imprimir_pilha_votos(Pilha* pilha);

// Backup e restauração
int salvar_pilha_votos_arquivo(Pilha* pilha, const char* filename);
Pilha* carregar_pilha_votos_arquivo(const char* filename);

// ================= PILHA DE OPERAÇÕES (HISTÓRICO) =================

Pilha* criar_pilha_operacoes();
Operacao* criar_operacao(const char* tipo, const char* descricao, int usuario_id);
void destruir_operacao(Operacao* operacao);
int registrar_operacao(Pilha* pilha, const char* tipo, const char* descricao, int usuario_id);
void imprimir_operacao(Operacao* operacao);
void imprimir_historico_operacoes(Pilha* pilha, int limite);
void limpar_historico(Pilha* pilha);

// ================= PILHA DE ELEITORES (PARA CONTROLE) =================

Pilha* criar_pilha_eleitores_atendidos();
int empilhar_eleitor_atendido(Pilha* pilha, Eleitor* eleitor);
Eleitor* desempilhar_eleitor_atendido(Pilha* pilha);
void imprimir_eleitores_atendidos(Pilha* pilha);

#endif
