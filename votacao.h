#ifndef VOTACAO_H
#define VOTACAO_H

#include "estruturas.h"

// ================= CONFIGURAÇÃO DO SISTEMA =================

typedef struct SistemaVotacao {
    bool votacao_ativa;
    time_t inicio_votacao;
    time_t fim_votacao;
    Pilha* historico_operacoes;
    Pilha* eleitores_atendidos;
    int usuario_logado; // ID do administrador ou mesário
} SistemaVotacao;

// Inicialização do sistema
SistemaVotacao* criar_sistema_votacao(int usuario_admin);
void destruir_sistema_votacao(SistemaVotacao* sistema);
void iniciar_votacao(SistemaVotacao* sistema);
void encerrar_votacao(SistemaVotacao* sistema);
bool votacao_esta_ativa(SistemaVotacao* sistema);

// ================= GERENCIAMENTO DE URNAS =================

Urna* criar_urna(int numero, int cap_id);
void destruir_urna(Urna* urna);
int urna_disponivel(Urna* urna);
void liberar_urna(Urna* urna);
int total_votos_urna(Urna* urna);
void imprimir_status_urna(Urna* urna);

// ================= PROCESSO DE VOTAÇÃO =================

// Fluxo completo de votação
int eleitor_chega_cap(CAP* cap, Eleitor* eleitor, SistemaVotacao* sistema);
Eleitor* chamar_proximo_eleitor(CAP* cap, SistemaVotacao* sistema);
Urna* direcionar_para_urna(CAP* cap, Eleitor* eleitor);
int registrar_voto_urna(Urna* urna, Eleitor* eleitor, int numero_candidato, 
                        ListaCandidatos* lista_candidatos, SistemaVotacao* sistema);
void finalizar_voto_eleitor(Eleitor* eleitor, Urna* urna, SistemaVotacao* sistema);

// Função completa de votação (chama todas as anteriores)
int processo_votacao_completo(CAP* cap, ListaCandidatos* lista_candidatos, 
                              SistemaVotacao* sistema, int numero_candidato);

// ================= CONTROLE E VALIDAÇÃO =================

int validar_eleitor_para_voto(Eleitor* eleitor, CAP* cap);
int validar_candidato_para_voto(ListaCandidatos* lista_candidatos, int numero_candidato);
int eleitor_pertence_cap(Eleitor* eleitor, CAP* cap);
int capacidade_urna_atingida(Urna* urna, CAP* cap);

// ================= SIMULAÇÃO EM LOTE =================

void simular_votacao_lote(CAP* cap, ListaEleitores* lista_eleitores, 
                         ListaCandidatos* lista_candidatos, SistemaVotacao* sistema,
                         int num_eleitores_simular);
void simular_fluxo_completo_cap(CAP* cap, ListaCandidatos* lista_candidatos,
                               SistemaVotacao* sistema, int num_eleitores);

// ================= ESTATÍSTICAS E RELATÓRIOS =================

void gerar_relatorio_votacao_cap(CAP* cap, SistemaVotacao* sistema);
void gerar_relatorio_urna(Urna* urna);
void estatisticas_tempo_fila(CAP* cap);
void imprimir_resultados_parciais(CAP* cap, ListaCandidatos* lista_candidatos);

#endif
