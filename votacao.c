#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "votacao.h"
#include "filas.h"
#include "pilhas.h"
#include "eleitores.h"
#include "candidatos.h"  // Adicione esta linha
// ================= IMPLEMENTAÇÃO CONFIGURAÇÃO DO SISTEMA =================

SistemaVotacao* criar_sistema_votacao(int usuario_admin) {
    SistemaVotacao* sistema = (SistemaVotacao*)malloc(sizeof(SistemaVotacao));
    if (sistema) {
        sistema->votacao_ativa = false;
        sistema->inicio_votacao = 0;
        sistema->fim_votacao = 0;
        sistema->historico_operacoes = criar_pilha_operacoes();
        sistema->eleitores_atendidos = criar_pilha_eleitores_atendidos();
        sistema->usuario_logado = usuario_admin;
        
        // Registrar operação de inicialização
        registrar_operacao(sistema->historico_operacoes, 
                          "SISTEMA", 
                          "Sistema de votacao inicializado", 
                          usuario_admin);
    }
    return sistema;
}

void destruir_sistema_votacao(SistemaVotacao* sistema) {
    if (!sistema) return;
    
    if (sistema->historico_operacoes) {
        // Salvar histórico em arquivo antes de destruir
        // (implementação opcional)
        destruir_pilha(sistema->historico_operacoes);
    }
    
    if (sistema->eleitores_atendidos) {
        destruir_pilha(sistema->eleitores_atendidos);
    }
    
    free(sistema);
}

void iniciar_votacao(SistemaVotacao* sistema) {
    if (!sistema) return;
    
    sistema->votacao_ativa = true;
    sistema->inicio_votacao = time(NULL);
    
    registrar_operacao(sistema->historico_operacoes,
                      "VOTACAO",
                      "Votacao iniciada",
                      sistema->usuario_logado);
    
    printf("=== VOTACAO INICIADA ===\n");
    printf("Horario: %s", ctime(&sistema->inicio_votacao));
}

void encerrar_votacao(SistemaVotacao* sistema) {
    if (!sistema) return;
    
    sistema->votacao_ativa = false;
    sistema->fim_votacao = time(NULL);
    
    registrar_operacao(sistema->historico_operacoes,
                      "VOTACAO",
                      "Votacao encerrada",
                      sistema->usuario_logado);
    
    printf("=== VOTACAO ENCERRADA ===\n");
    printf("Horario: %s", ctime(&sistema->fim_votacao));
    printf("Duracao: %.0f minutos\n", 
           difftime(sistema->fim_votacao, sistema->inicio_votacao) / 60);
}

bool votacao_esta_ativa(SistemaVotacao* sistema) {
    return sistema && sistema->votacao_ativa;
}

// ================= IMPLEMENTAÇÃO GERENCIAMENTO DE URNAS =================

Urna* criar_urna(int numero, int cap_id) {
    Urna* urna = (Urna*)malloc(sizeof(Urna));
    if (urna) {
        urna->numero = numero;
        urna->cap_id = cap_id;
        urna->votos_registrados = 0;
        urna->ativa = true;
        urna->historico_votos = criar_pilha_votos();
    }
    return urna;
}

void destruir_urna(Urna* urna) {
    if (!urna) return;
    
    if (urna->historico_votos) {
        // Salvar votos em arquivo antes de destruir
        // (implementação opcional)
        destruir_pilha(urna->historico_votos);
    }
    
    free(urna);
}

int urna_disponivel(Urna* urna) {
    return (urna && urna->ativa);
}

void liberar_urna(Urna* urna) {
    if (urna) {
        urna->ativa = true; // Marcar como disponível
    }
}

int total_votos_urna(Urna* urna) {
    return urna ? urna->votos_registrados : 0;
}

void imprimir_status_urna(Urna* urna) {
    if (!urna) {
        printf("Urna invalida!\n");
        return;
    }
    
    printf("Urna %d (CAP %d):\n", urna->numero, urna->cap_id);
    printf("  Status: %s\n", urna->ativa ? "ATIVA" : "INATIVA");
    printf("  Votos registrados: %d\n", urna->votos_registrados);
    printf("  Votos na pilha: %d\n", tamanho_pilha(urna->historico_votos));
}

// ================= IMPLEMENTAÇÃO PROCESSO DE VOTAÇÃO =================

int eleitor_chega_cap(CAP* cap, Eleitor* eleitor, SistemaVotacao* sistema) {
    if (!cap || !eleitor || !sistema) return 0;
    
    // Validar se eleitor pertence a este CAP
    if (!validar_eleitor_para_voto(eleitor, cap)) {
        printf("Eleitor %s nao pertence a este CAP!\n", eleitor->nome);
        return 0;
    }
    
    // Validar se eleitor já votou
    if (eleitor->votou) {
        printf("Eleitor %s ja votou!\n", eleitor->nome);
        return 0;
    }
    
    // Adicionar à fila do CAP
    if (adicionar_eleitor_cap_fila(cap, eleitor)) {
        char descricao[200];
        snprintf(descricao, sizeof(descricao), 
                "Eleitor %s (%d) chegou ao CAP %d", 
                eleitor->nome, eleitor->id, cap->id);
        
        registrar_operacao(sistema->historico_operacoes,
                          "CHEGADA",
                          descricao,
                          sistema->usuario_logado);
        
        printf("Eleitor %s adicionado a fila do CAP %d\n", 
               eleitor->nome, cap->id);
        return 1;
    }
    
    return 0;
}

Eleitor* chamar_proximo_eleitor(CAP* cap, SistemaVotacao* sistema) {
    if (!cap || !sistema) return NULL;
    
    Eleitor* eleitor = proximo_eleitor_cap(cap);
    if (eleitor) {
        char descricao[200];
        snprintf(descricao, sizeof(descricao),
                "Eleitor %s (%d) chamado para votar no CAP %d",
                eleitor->nome, eleitor->id, cap->id);
        
        registrar_operacao(sistema->historico_operacoes,
                          "CHAMADA",
                          descricao,
                          sistema->usuario_logado);
        
        printf("Proximo eleitor: %s (ID: %d)\n", eleitor->nome, eleitor->id);
    }
    
    return eleitor;
}

Urna* direcionar_para_urna(CAP* cap, Eleitor* eleitor) {
    if (!cap || !eleitor || !cap->urnas) return NULL;
    
    // Encontrar uma urna disponível
    for (int i = 0; i < cap->num_urnas; i++) {
        if (cap->urnas[i] && urna_disponivel(cap->urnas[i])) {
            // Verificar se a urna não atingiu capacidade máxima
            if (cap->urnas[i]->votos_registrados < cap->max_eleitores_por_urna) {
                cap->urnas[i]->ativa = false; // Ocupar urna
                printf("Eleitor %s direcionado para urna %d\n", 
                       eleitor->nome, cap->urnas[i]->numero);
                return cap->urnas[i];
            }
        }
    }
    
    printf("Nenhuma urna disponivel no momento!\n");
    return NULL;
}

int registrar_voto_urna(Urna* urna, Eleitor* eleitor, int numero_candidato, 
                       ListaCandidatos* lista_candidatos, SistemaVotacao* sistema) {
    if (!urna || !eleitor || !lista_candidatos || !sistema) return 0;
    
    // Validar candidato
    if (!validar_candidato_para_voto(lista_candidatos, numero_candidato)) {
        printf("Candidato %d invalido!\n", numero_candidato);
        return 0;
    }
    
    // Criar voto
    Voto* voto = criar_voto(eleitor->id, numero_candidato, urna->cap_id, urna->numero);
    if (!voto) return 0;
    
    // Empilhar voto (registro temporário)
    if (!empilhar_voto(urna->historico_votos, voto)) {
        destruir_voto(voto);
        return 0;
    }
    
    // Atualizar contador da urna
    urna->votos_registrados++;
    
    // Atualizar contador do candidato
    Candidato* candidato = buscar_candidato_por_numero(lista_candidatos, numero_candidato);
    if (candidato) {
        candidato->votos++;
    }
    
    // Registrar operação
    char descricao[200];
    snprintf(descricao, sizeof(descricao),
            "Voto registrado: Eleitor %d -> Candidato %d (Urna %d)",
            eleitor->id, numero_candidato, urna->numero);
    
    registrar_operacao(sistema->historico_operacoes,
                      "VOTO",
                      descricao,
                      sistema->usuario_logado);
    
    printf("Voto registrado com sucesso!\n");
    return 1;
}

void finalizar_voto_eleitor(Eleitor* eleitor, Urna* urna, SistemaVotacao* sistema) {
    if (!eleitor || !urna || !sistema) return;
    
    // Marcar eleitor como votado
    eleitor->votou = 1;
    
    // Liberar urna para próximo eleitor
    liberar_urna(urna);
    
    // Adicionar à pilha de eleitores atendidos
    empilhar_eleitor_atendido(sistema->eleitores_atendidos, eleitor);
    
    // Registrar operação
    char descricao[200];
    snprintf(descricao, sizeof(descricao),
            "Voto finalizado: Eleitor %s (%d) na urna %d",
            eleitor->nome, eleitor->id, urna->numero);
    
    registrar_operacao(sistema->historico_operacoes,
                      "FINALIZACAO",
                      descricao,
                      sistema->usuario_logado);
    
    printf("Voto finalizado para %s. Urna %d liberada.\n", 
           eleitor->nome, urna->numero);
}

// Função completa de votação
int processo_votacao_completo(CAP* cap, ListaCandidatos* lista_candidatos, 
                             SistemaVotacao* sistema, int numero_candidato) {
    if (!cap || !lista_candidatos || !sistema) return 0;
    
    // 1. Chamar próximo eleitor
    Eleitor* eleitor = chamar_proximo_eleitor(cap, sistema);
    if (!eleitor) {
        printf("Nenhum eleitor na fila!\n");
        return 0;
    }
    
    // 2. Direcionar para urna
    Urna* urna = direcionar_para_urna(cap, eleitor);
    if (!urna) {
        // Devolver eleitor para fila (simplificado)
        printf("Nao foi possivel direcionar eleitor para urna.\n");
        return 0;
    }
    
    // 3. Registrar voto
    if (!registrar_voto_urna(urna, eleitor, numero_candidato, lista_candidatos, sistema)) {
        liberar_urna(urna); // Liberar urna em caso de erro
        return 0;
    }
    
    // 4. Finalizar voto
    finalizar_voto_eleitor(eleitor, urna, sistema);
    
    return 1;
}

// ================= IMPLEMENTAÇÃO CONTROLE E VALIDAÇÃO =================

int validar_eleitor_para_voto(Eleitor* eleitor, CAP* cap) {
    if (!eleitor || !cap) return 0;
    
    // Verificar se eleitor pertence ao CAP
    if (eleitor->id_cap != cap->id) {
        printf("Eleitor nao pertence a este CAP!\n");
        return 0;
    }
    
    // Verificar se eleitor já votou
    if (eleitor->votou) {
        printf("Eleitor ja votou!\n");
        return 0;
    }
    
    return 1;
}

int validar_candidato_para_voto(ListaCandidatos* lista_candidatos, int numero_candidato) {
    if (!lista_candidatos) return 0;
    
    return (buscar_candidato_por_numero(lista_candidatos, numero_candidato) != NULL);
}

int eleitor_pertence_cap(Eleitor* eleitor, CAP* cap) {
    return (eleitor && cap && eleitor->id_cap == cap->id);
}

int capacidade_urna_atingida(Urna* urna, CAP* cap) {
    if (!urna || !cap) return 1;
    
    return (urna->votos_registrados >= cap->max_eleitores_por_urna);
}

// ================= IMPLEMENTAÇÃO SIMULAÇÃO EM LOTE =================

void simular_votacao_lote(CAP* cap, ListaEleitores* lista_eleitores, 
                         ListaCandidatos* lista_candidatos, SistemaVotacao* sistema,
                         int num_eleitores_simular) {
    if (!cap || !lista_eleitores || !lista_candidatos || !sistema) return;
    
    printf("\n=== SIMULACAO DE VOTACAO EM LOTE ===\n");
    printf("CAP: %d\n", cap->id);
    printf("Eleitores a simular: %d\n\n", num_eleitores_simular);
    
    // Primeiro, adicionar eleitores à fila
    NoEleitor* atual = lista_eleitores->inicio;
    int eleitores_adicionados = 0;
    
    while (atual != NULL && eleitores_adicionados < num_eleitores_simular) {
        if (atual->eleitor.id_cap == cap->id && !atual->eleitor.votou) {
            eleitor_chega_cap(cap, &atual->eleitor, sistema);
            eleitores_adicionados++;
        }
        atual = atual->proximo;
    }
    
    printf("\n%d eleitores adicionados a fila.\n", eleitores_adicionados);
    
    // Simular votação para cada eleitor na fila
    int votos_registrados = 0;
    
    while (total_eleitores_na_fila_cap(cap) > 0 && votos_registrados < num_eleitores_simular) {
        // Simular escolha de candidato aleatória
        if (lista_candidatos->tamanho > 0) {
            int indice_candidato = rand() % lista_candidatos->tamanho;
            int numero_candidato = lista_candidatos->candidatos[indice_candidato].numero;
            
            if (processo_votacao_completo(cap, lista_candidatos, sistema, numero_candidato)) {
                votos_registrados++;
                
                // Simular pequeno atraso entre votos
                // sleep(1); // Descomentar se quiser pausa real
            }
        }
    }
    
    printf("\nSimulacao concluida: %d votos registrados.\n", votos_registrados);
}

void simular_fluxo_completo_cap(CAP* cap, ListaCandidatos* lista_candidatos,
                               SistemaVotacao* sistema, int num_eleitores) {
    if (!cap || !lista_candidatos || !sistema || num_eleitores <= 0) return;
    
    printf("\n=== FLUXO COMPLETO DE VOTACAO - CAP %d ===\n", cap->id);
    
    // Criar eleitores de teste
    ListaEleitores lista_eleitores;
    inicializar_lista_eleitores(&lista_eleitores);
    
    for (int i = 0; i < num_eleitores; i++) {
        char nome[50];
        char documento[20];
        
        snprintf(nome, sizeof(nome), "Eleitor Teste %d", i+1);
        snprintf(documento, sizeof(documento), "1000%d", i+1);
        
        // 20% serão prioritários
        int prioridade = (rand() % 5 == 0) ? PRIORIDADE_ALTA : PRIORIDADE_NORMAL;
        
        cadastrar_eleitor(&lista_eleitores, nome, documento, cap->id, prioridade);
    }
    
    // Simular votação em lote
    simular_votacao_lote(cap, &lista_eleitores, lista_candidatos, sistema, num_eleitores);
    
    // Gerar relatório
    gerar_relatorio_votacao_cap(cap, sistema);
    
    // Liberar memória
    liberar_lista_eleitores(&lista_eleitores);
}

// ================= IMPLEMENTAÇÃO ESTATÍSTICAS E RELATÓRIOS =================

void gerar_relatorio_votacao_cap(CAP* cap, SistemaVotacao* sistema) {
    if (!cap || !sistema) return;
    
    printf("\n=== RELATORIO DE VOTACAO - CAP %d ===\n", cap->id);
    printf("Localizacao: %s\n", cap->localizacao);
    printf("Urnas: %d\n", cap->num_urnas);
    printf("Capacidade por urna: %d\n", cap->max_eleitores_por_urna);
    printf("Capacidade total: %d\n", cap->num_urnas * cap->max_eleitores_por_urna);
    printf("\n");
    printf("Eleitores atendidos: %d\n", cap->eleitores_atendidos);
    printf("Eleitores na fila: %d\n", total_eleitores_na_fila_cap(cap));
    printf("Eleitores prioritarios na fila: %d\n", eleitores_prioritarios_na_fila_cap(cap));
    
    // Estatísticas das urnas
    printf("\n=== ESTATISTICAS DAS URNAS ===\n");
    int total_votos_cap = 0;
    
    if (cap->urnas) {
        for (int i = 0; i < cap->num_urnas; i++) {
            if (cap->urnas[i]) {
                printf("Urna %d: %d votos\n", 
                       cap->urnas[i]->numero, 
                       cap->urnas[i]->votos_registrados);
                total_votos_cap += cap->urnas[i]->votos_registrados;
            }
        }
    }
    
    printf("Total de votos no CAP: %d\n", total_votos_cap);
}

void gerar_relatorio_urna(Urna* urna) {
    if (!urna) return;
    
    printf("\n=== RELATORIO DA URNA %d ===\n", urna->numero);
    printf("CAP: %d\n", urna->cap_id);
    printf("Status: %s\n", urna->ativa ? "ATIVA" : "INATIVA");
    printf("Votos registrados: %d\n", urna->votos_registrados);
    printf("Votos na pilha temporaria: %d\n", tamanho_pilha(urna->historico_votos));
    
    // Mostrar últimos votos (se houver)
    if (!pilha_vazia(urna->historico_votos)) {
        printf("\n=== ULTIMOS VOTOS REGISTRADOS ===\n");
        imprimir_pilha_votos(urna->historico_votos);
    }
}

void estatisticas_tempo_fila(CAP* cap) {
    if (!cap) return;
    
    printf("\n=== ESTATISTICAS DE FILA - CAP %d ===\n", cap->id);
    printf("Total na fila: %d\n", total_eleitores_na_fila_cap(cap));
    printf("Prioritarios na fila: %d\n", eleitores_prioritarios_na_fila_cap(cap));
    
    if (cap->eleitores_atendidos > 0) {
        float media_por_urna = (float)cap->eleitores_atendidos / cap->num_urnas;
        printf("Media de eleitores por urna: %.1f\n", media_por_urna);
        printf("Taxa de ocupacao: %.1f%%\n", 
               (media_por_urna / cap->max_eleitores_por_urna) * 100);
    }
}

void imprimir_resultados_parciais(CAP* cap, ListaCandidatos* lista_candidatos) {
    if (!cap || !lista_candidatos) return;
    
    printf("\n=== RESULTADOS PARCIAIS - CAP %d ===\n", cap->id);
    
    // Contar votos por candidato neste CAP
    // (Implementação simplificada - na realidade precisaríamos rastrear votos por CAP)
    
    printf("Nota: Para resultados por CAP, e necessario rastrear cada voto com o CAP.\n");
    printf("Esta funcionalidade sera implementada com arvores binarias.\n");
}
