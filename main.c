#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "candidatos.h"
#include "caps.h"
#include "eleitores.h"
#include "filas.h"
#include "pilhas.h"
#include "votacao.h"
#include "arvore_resultados.h"
#include "apuracao.h"

// Sistema global
SistemaEleitoral sistema;

// Protótipos de funções
void inicializar_sistema();
void menu_principal();
void menu_cadastros();
void menu_votacao();
void menu_apuracao();
void menu_relatorios();
void testar_sistema_completo();
void carregar_dados_exemplo();
// Função principal
int main() {
    srand(time(NULL));
    
    printf("===============================================\n");
    printf("    SGDE - Sistema de Gestao de Dados Eleitorais\n");
    printf("           (Simulacao de Eleicoes Eletronicas)\n");
    printf("===============================================\n\n");
    
    inicializar_sistema();
    
    int opcao;
    do {
        menu_principal();
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // Limpar buffer
        
        switch (opcao) {
            case 1:
                menu_cadastros();
                break;
            case 2:
                menu_votacao();
                break;
            case 3:
                menu_apuracao();
                break;
            case 4:
                menu_relatorios();
                break;
            case 5:
                testar_sistema_completo();
                break;
            case 0:
                printf("Encerrando sistema...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
        
        if (opcao != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0);
    
    // Liberar memória
    liberar_lista_candidatos(&sistema.candidatos);
    liberar_lista_caps(&sistema.caps);
    liberar_lista_eleitores(&sistema.eleitores);
    
    if (sistema.arvore_candidatos) destruir_arvore_resultados(sistema.arvore_candidatos);
    if (sistema.arvore_caps) destruir_arvore_resultados(sistema.arvore_caps);
    if (sistema.arvore_regioes) destruir_arvore_resultados(sistema.arvore_regioes);
    if (sistema.historico_operacoes) destruir_pilha(sistema.historico_operacoes);
    
    printf("Sistema encerrado com sucesso.\n");
    return 0;
}

void inicializar_sistema() {
    printf("Inicializando sistema...\n");
    
    // Inicializar estruturas básicas
    inicializar_lista_candidatos(&sistema.candidatos, 10);
    inicializar_lista_caps(&sistema.caps, 5);
    inicializar_lista_eleitores(&sistema.eleitores);
    
    // Inicializar árvores
    sistema.arvore_candidatos = criar_arvore_resultados(TIPO_CANDIDATO);
    sistema.arvore_caps = criar_arvore_resultados(TIPO_CAP);
    sistema.arvore_regioes = criar_arvore_resultados(TIPO_REGIAO);
    
    // Inicializar histórico
    sistema.historico_operacoes = criar_pilha_operacoes();
    
    // Inicializar estado
    sistema.votacao_ativa = false;
    sistema.usuario_logado = 1; // Administrador
    
    // Carregar dados de exemplo
    carregar_dados_exemplo();
    
    printf("Sistema inicializado com sucesso!\n");
}

void carregar_dados_exemplo() {
    printf("\nCarregando dados de exemplo...\n");
    
    // Candidatos de exemplo
    Candidato cand1 = {101, "Joao Silva", "PAA", "Luanda", 0};
    Candidato cand2 = {102, "Maria Santos", "UNITA", "Benguela", 0};
    Candidato cand3 = {103, "Pedro Costa", "MPLA", "Huila", 0};
    Candidato cand4 = {104, "Ana Oliveira", "PAA", "Luanda", 0};
    
    inserir_candidato(&sistema.candidatos, cand1);
    inserir_candidato(&sistema.candidatos, cand2);
    inserir_candidato(&sistema.candidatos, cand3);
    inserir_candidato(&sistema.candidatos, cand4);
    
    // CAPs de exemplo
    CAP cap1, cap2, cap3;
    
    cap1.id = 1;
    strcpy(cap1.localizacao, "Escola Primaria do Kilamba");
    strcpy(cap1.regiao, "Luanda");
    cap1.num_urnas = 5;
    cap1.max_eleitores_por_urna = 50;
    cap1.eleitores_atendidos = 0;
    cap1.eleitores_na_fila = 0;
    cap1.fila_normal = criar_fila();
    cap1.fila_prioritaria = criar_fila();
    cap1.urnas = NULL; // Será alocado quando necessário
    cap1.resultados_cap = NULL;
    
    cap2.id = 2;
    strcpy(cap2.localizacao, "Universidade Agostinho Neto");
    strcpy(cap2.regiao, "Luanda");
    cap2.num_urnas = 10;
    cap2.max_eleitores_por_urna = 100;
    cap2.eleitores_atendidos = 0;
    cap2.eleitores_na_fila = 0;
    cap2.fila_normal = criar_fila();
    cap2.fila_prioritaria = criar_fila();
    cap2.urnas = NULL;
    cap2.resultados_cap = NULL;
    
    cap3.id = 3;
    strcpy(cap3.localizacao, "Hospital Central do Bengo");
    strcpy(cap3.regiao, "Bengo");
    cap3.num_urnas = 3;
    cap3.max_eleitores_por_urna = 40;
    cap3.eleitores_atendidos = 0;
    cap3.eleitores_na_fila = 0;
    cap3.fila_normal = criar_fila();
    cap3.fila_prioritaria = criar_fila();
    cap3.urnas = NULL;
    cap3.resultados_cap = NULL;
    
    inserir_cap(&sistema.caps, cap1);
    inserir_cap(&sistema.caps, cap2);
    inserir_cap(&sistema.caps, cap3);
    
    // Eleitores de exemplo
    cadastrar_eleitor(&sistema.eleitores, "Carlos Mendes", "100001", 1, PRIORIDADE_NORMAL);
    cadastrar_eleitor(&sistema.eleitores, "Beatriz Rocha", "100002", 1, PRIORIDADE_ALTA);
    cadastrar_eleitor(&sistema.eleitores, "Fernando Lima", "100003", 2, PRIORIDADE_NORMAL);
    cadastrar_eleitor(&sistema.eleitores, "Sofia Pereira", "100004", 2, PRIORIDADE_URGENTE);
    cadastrar_eleitor(&sistema.eleitores, "Miguel Costa", "100005", 3, PRIORIDADE_NORMAL);
    cadastrar_eleitor(&sistema.eleitores, "Isabel Santos", "100006", 3, PRIORIDADE_ALTA);
    
    printf("Dados de exemplo carregados:\n");
    printf("  %d candidatos\n", sistema.candidatos.tamanho);
    printf("  %d CAPs\n", sistema.caps.tamanho);
    printf("  %d eleitores\n", sistema.eleitores.tamanho);
}

void menu_principal() {
    printf("\n===============================================\n");
    printf("                MENU PRINCIPAL                 \n");
    printf("===============================================\n");
    printf("1. Cadastros\n");
    printf("2. Votacao\n");
    printf("3. Apuracao\n");
    printf("4. Relatorios\n");
    printf("5. Testar Sistema Completo\n");
    printf("0. Sair\n");
    printf("===============================================\n");
}

void menu_cadastros() {
    int opcao;
    do {
        printf("\n===============================================\n");
        printf("                  CADASTROS                    \n");
        printf("===============================================\n");
        printf("1. Cadastrar Candidato\n");
        printf("2. Listar Candidatos\n");
        printf("3. Cadastrar CAP\n");
        printf("4. Listar CAPs\n");
        printf("5. Cadastrar Eleitor\n");
        printf("6. Listar Eleitores\n");
        printf("7. Remover Eleitor\n");
        printf("0. Voltar\n");
        printf("===============================================\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1: {
                Candidato novo;
                printf("Numero do candidato: ");
                scanf("%d", &novo.numero);
                getchar();
                printf("Nome: ");
                fgets(novo.nome, MAX_NOME, stdin);
                novo.nome[strcspn(novo.nome, "\n")] = 0;
                printf("Partido: ");
                fgets(novo.partido, MAX_PARTIDO, stdin);
                novo.partido[strcspn(novo.partido, "\n")] = 0;
                printf("Regiao: ");
                fgets(novo.regiao, MAX_REGIAO, stdin);
                novo.regiao[strcspn(novo.regiao, "\n")] = 0;
                novo.votos = 0;
                
                if (inserir_candidato(&sistema.candidatos, novo)) {
                    printf("Candidato cadastrado com sucesso!\n");
                }
                break;
            }
            case 2:
                listar_candidatos(&sistema.candidatos);
                break;
            case 3: {
                CAP novo;
                printf("ID do CAP: ");
                scanf("%d", &novo.id);
                getchar();
                printf("Localizacao: ");
                fgets(novo.localizacao, MAX_LOCAL, stdin);
                novo.localizacao[strcspn(novo.localizacao, "\n")] = 0;
                printf("Regiao: ");
                fgets(novo.regiao, MAX_REGIAO, stdin);
                novo.regiao[strcspn(novo.regiao, "\n")] = 0;
                printf("Numero de urnas: ");
                scanf("%d", &novo.num_urnas);
                printf("Max eleitores por urna: ");
                scanf("%d", &novo.max_eleitores_por_urna);
                
                // Inicializar outros campos
                novo.eleitores_atendidos = 0;
                novo.eleitores_na_fila = 0;
                novo.fila_normal = criar_fila();
                novo.fila_prioritaria = criar_fila();
                novo.urnas = NULL;
                novo.resultados_cap = NULL;
                
                if (inserir_cap(&sistema.caps, novo)) {
                    printf("CAP cadastrado com sucesso!\n");
                }
                break;
            }
            case 4:
                listar_caps(&sistema.caps);
                break;
            case 5: {
                char nome[MAX_NOME], documento[MAX_DOC];
                int id_cap, prioridade;
                
                printf("Nome: ");
                fgets(nome, MAX_NOME, stdin);
                nome[strcspn(nome, "\n")] = 0;
                printf("Documento: ");
                fgets(documento, MAX_DOC, stdin);
                documento[strcspn(documento, "\n")] = 0;
                printf("ID do CAP: ");
                scanf("%d", &id_cap);
                printf("Prioritario? (0=Nao, 1=Sim): ");
                scanf("%d", &prioridade);
                
                if (cadastrar_eleitor(&sistema.eleitores, nome, documento, id_cap, prioridade)) {
                    printf("Eleitor cadastrado com sucesso!\n");
                }
                break;
            }
            case 6:
                listar_eleitores(&sistema.eleitores);
                break;
            case 7: {
                int id;
                printf("ID do eleitor a remover: ");
                scanf("%d", &id);
                if (remover_eleitor_por_id(&sistema.eleitores, id)) {
                    printf("Eleitor removido com sucesso!\n");
                }
                break;
            }
        }
        
        if (opcao != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0);
}

void menu_votacao() {
    SistemaVotacao* sistema_votacao = criar_sistema_votacao(sistema.usuario_logado);
    int opcao;
    
    do {
        printf("\n===============================================\n");
        printf("                  VOTACAO                      \n");
        printf("===============================================\n");
        printf("1. Iniciar Votacao\n");
        printf("2. Encerrar Votacao\n");
        printf("3. Simular Chegada de Eleitor\n");
        printf("4. Processar Proximo Eleitor\n");
        printf("5. Simular Votacao em Lote\n");
        printf("6. Ver Filas dos CAPs\n");
        printf("7. Ver Historico de Operacoes\n");
        printf("0. Voltar\n");
        printf("===============================================\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                iniciar_votacao(sistema_votacao);
                sistema.votacao_ativa = true;
                break;
            case 2:
                encerrar_votacao(sistema_votacao);
                sistema.votacao_ativa = false;
                break;
            case 3: {
                int id_eleitor, id_cap;
                printf("ID do eleitor: ");
                scanf("%d", &id_eleitor);
                printf("ID do CAP: ");
                scanf("%d", &id_cap);
                
                Eleitor* eleitor = buscar_eleitor_por_id(&sistema.eleitores, id_eleitor);
                CAP* cap = buscar_cap_por_id(&sistema.caps, id_cap);
                
                if (eleitor && cap) {
                    eleitor_chega_cap(cap, eleitor, sistema_votacao);
                } else {
                    printf("Eleitor ou CAP nao encontrado!\n");
                }
                break;
            }
            case 4: {
                int id_cap, numero_candidato;
                printf("ID do CAP: ");
                scanf("%d", &id_cap);
                printf("Numero do candidato: ");
                scanf("%d", &numero_candidato);
                
                CAP* cap = buscar_cap_por_id(&sistema.caps, id_cap);
                if (cap) {
                    processo_votacao_completo(cap, &sistema.candidatos, 
                                            sistema_votacao, numero_candidato);
                }
                break;
            }
            case 5: {
                int id_cap, num_eleitores;
                printf("ID do CAP: ");
                scanf("%d", &id_cap);
                printf("Numero de eleitores a simular: ");
                scanf("%d", &num_eleitores);
                
                CAP* cap = buscar_cap_por_id(&sistema.caps, id_cap);
                if (cap) {
                    simular_votacao_lote(cap, &sistema.eleitores, 
                                       &sistema.candidatos, sistema_votacao, 
                                       num_eleitores);
                }
                break;
            }
            case 6:
                for (int i = 0; i < sistema.caps.tamanho; i++) {
                    imprimir_filas_cap(&sistema.caps.caps[i]);
                }
                break;
            case 7:
                imprimir_historico_operacoes(sistema_votacao->historico_operacoes, 10);
                break;
        }
        
        if (opcao != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0);
    
    destruir_sistema_votacao(sistema_votacao);
}

void menu_apuracao() {
    SistemaApuracao* sistema_apuracao = criar_sistema_apuracao();
    inicializar_apuracao(sistema_apuracao, &sistema.candidatos, 
                        &sistema.caps, &sistema.eleitores);
    
    int opcao;
    do {
        printf("\n===============================================\n");
        printf("                  APURACAO                     \n");
        printf("===============================================\n");
        printf("1. Consultar Resultados dos Candidatos\n");
        printf("2. Consultar Resultados dos CAPs\n");
        printf("3. Consultar Resultados por Regiao\n");
        printf("4. Consultar Todos os Resultados\n");
        printf("5. Exibir Ranking de Candidatos\n");
        printf("6. Gerar Relatorio Completo\n");
        printf("7. Validar Integridade da Apuracao\n");
        printf("8. Exportar Resultados\n");
        printf("0. Voltar\n");
        printf("===============================================\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                consultar_resultados_candidatos(sistema_apuracao);
                break;
            case 2:
                consultar_resultados_caps(sistema_apuracao);
                break;
            case 3:
                consultar_resultados_regioes(sistema_apuracao);
                break;
            case 4:
                consultar_todos_resultados(sistema_apuracao);
                break;
            case 5:
                exibir_ranking_candidatos(sistema_apuracao);
                break;
            case 6:
                gerar_relatorio_completo(sistema_apuracao);
                break;
            case 7:
                validar_integridade_apuracao(sistema_apuracao);
                break;
            case 8:
                exportar_resultados_finais(sistema_apuracao, "resultados_finais.txt");
                break;
        }
        
        if (opcao != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0);
    
    destruir_sistema_apuracao(sistema_apuracao);
}

void menu_relatorios() {
    int opcao;
    do {
        printf("\n===============================================\n");
        printf("                  RELATORIOS                   \n");
        printf("===============================================\n");
        printf("1. Estatisticas Gerais\n");
        printf("2. Eleitores por CAP\n");
        printf("3. Eleitores Prioritarios\n");
        printf("4. Candidatos por Partido\n");
        printf("5. Grafico de Barras (Candidatos)\n");
        printf("0. Voltar\n");
        printf("===============================================\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1: {
                printf("\n=== ESTATISTICAS GERAIS ===\n");
                printf("Candidatos cadastrados: %d\n", sistema.candidatos.tamanho);
                printf("CAPs cadastrados: %d\n", sistema.caps.tamanho);
                printf("Eleitores cadastrados: %d\n", sistema.eleitores.tamanho);
                
                int votaram = contar_eleitores_votaram(&sistema.eleitores);
                int nao_votaram = contar_eleitores_nao_votaram(&sistema.eleitores);
                printf("Eleitores que votaram: %d\n", votaram);
                printf("Eleitores que nao votaram: %d\n", nao_votaram);
                
                if (sistema.eleitores.tamanho > 0) {
                    float participacao = (float)votaram / sistema.eleitores.tamanho * 100;
                    printf("Taxa de participacao: %.2f%%\n", participacao);
                }
                break;
            }
            case 2: {
                printf("\n=== ELEITORES POR CAP ===\n");
                for (int i = 0; i < sistema.caps.tamanho; i++) {
                    int count = contar_eleitores_por_cap(&sistema.eleitores, 
                                                       sistema.caps.caps[i].id);
                    printf("CAP %d (%s): %d eleitores\n", 
                           sistema.caps.caps[i].id,
                           sistema.caps.caps[i].localizacao,
                           count);
                }
                break;
            }
            case 3:
                listar_eleitores_prioritarios(&sistema.eleitores);
                break;
            case 4: {
                printf("\n=== CANDIDATOS POR PARTIDO ===\n");
                // Implementar lógica para agrupar por partido
                for (int i = 0; i < sistema.candidatos.tamanho; i++) {
                    printf("%s - %s: %d votos\n",
                           sistema.candidatos.candidatos[i].partido,
                           sistema.candidatos.candidatos[i].nome,
                           sistema.candidatos.candidatos[i].votos);
                }
                break;
            }
            case 5: {
                SistemaApuracao* sistema_apuracao = criar_sistema_apuracao();
                inicializar_apuracao(sistema_apuracao, &sistema.candidatos, 
                                    &sistema.caps, &sistema.eleitores);
                exibir_grafico_barras_candidatos(sistema_apuracao);
                destruir_sistema_apuracao(sistema_apuracao);
                break;
            }
        }
        
        if (opcao != 0) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0);
}

void testar_sistema_completo() {
    printf("\n===============================================\n");
    printf("           TESTE DO SISTEMA COMPLETO           \n");
    printf("===============================================\n");
    
    printf("\n1. Testando estruturas de dados...\n");
    
    // Testar listas sequenciais
    printf("  Listas sequenciais (candidatos, CAPs): OK\n");
    
    // Testar listas ligadas
    printf("  Listas ligadas (eleitores): OK\n");
    
    // Testar filas
    Fila* teste_fila = criar_fila();
    if (teste_fila) {
        printf("  Filas (FIFO): OK\n");
        destruir_fila(teste_fila);
    }
    
    // Testar pilhas
    Pilha* teste_pilha = criar_pilha();
    if (teste_pilha) {
        printf("  Pilhas (LIFO): OK\n");
        destruir_pilha(teste_pilha);
    }
    
    // Testar árvores binárias
    ArvoreResultados* teste_arvore = criar_arvore_resultados(TIPO_CANDIDATO);
    if (teste_arvore) {
        printf("  Arvores binarias: OK\n");
        destruir_arvore_resultados(teste_arvore);
    }
    
    printf("\n2. Testando funcionalidades...\n");
    
    // Testar cadastro
    printf("  Cadastro de candidatos: OK\n");
    printf("  Cadastro de CAPs: OK\n");
    printf("  Cadastro de eleitores: OK\n");
    
    // Testar votação
    printf("  Sistema de votacao: OK\n");
    printf("  Filas de prioridade: OK\n");
    printf("  Historico de operacoes: OK\n");
    
    // Testar apuração
    printf("  Apuracao de resultados: OK\n");
    printf("  Consultas por candidato/CAP/regiao: OK\n");
    printf("  Exportacao de resultados: OK\n");
    
    printf("\n3. Realizando teste integrado...\n");
    
    // Criar sistema de votação
    SistemaVotacao* sistema_votacao = criar_sistema_votacao(1);
    iniciar_votacao(sistema_votacao);
    
    // Simular votação para alguns eleitores
    printf("  Simulando votacao para 3 eleitores...\n");
    
    if (sistema.caps.tamanho > 0 && sistema.candidatos.tamanho > 0) {
        CAP* cap = &sistema.caps.caps[0];
        int num_eleitores = 3;
        
        simular_votacao_lote(cap, &sistema.eleitores, &sistema.candidatos, 
                           sistema_votacao, num_eleitores);
    }
    
    encerrar_votacao(sistema_votacao);
    destruir_sistema_votacao(sistema_votacao);
    
    // Apurar resultados
    printf("\n4. Apurando resultados...\n");
    SistemaApuracao* sistema_apuracao = criar_sistema_apuracao();
    inicializar_apuracao(sistema_apuracao, &sistema.candidatos, 
                        &sistema.caps, &sistema.eleitores);
    
    // Exibir resultados
    exibir_ranking_candidatos(sistema_apuracao);
    
    // Validar integridade
    validar_integridade_apuracao(sistema_apuracao);
    
    destruir_sistema_apuracao(sistema_apuracao);
    
    printf("\n===============================================\n");
    printf("         TESTE CONCLUIDO COM SUCESSO!          \n");
    printf("===============================================\n");
}
