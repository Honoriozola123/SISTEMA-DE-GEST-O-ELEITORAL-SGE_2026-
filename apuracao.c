// Inclui a biblioteca padrão de entrada/saída para usar printf, scanf, etc.
#include <stdio.h>
// Inclui a biblioteca padrão de alocação de memória para usar malloc, free, etc.
#include <stdlib.h>
// Inclui a biblioteca padrão de manipulação de strings para usar strcpy, strcmp, etc.
#include <string.h>
// Inclui a biblioteca padrão de manipulação de tempo para usar time, localtime, etc.
#include <time.h>
// Inclui o cabeçalho das funções de apuração
#include "apuracao.h"
// Inclui o cabeçalho das funções de candidatos
#include "candidatos.h"
// Inclui o cabeçalho das funções de CAPs (Centros de Apuração)
#include "caps.h"
// Inclui o cabeçalho das funções de eleitores
#include "eleitores.h"

// ================= IMPLEMENTAÇÃO SISTEMA DE APURAÇÃO =================

// Função que cria e inicializa um novo sistema de apuração
SistemaApuracao* criar_sistema_apuracao() {
    // Aloca memória para a estrutura SistemaApuracao
    SistemaApuracao* sistema = (SistemaApuracao*)malloc(sizeof(SistemaApuracao));
    // Verifica se a alocação foi bem sucedida
    if (sistema) {
        // Cria uma árvore de resultados para candidatos
        sistema->arvore_candidatos = criar_arvore_resultados(TIPO_CANDIDATO);
        // Cria uma árvore de resultados para CAPs
        sistema->arvore_caps = criar_arvore_resultados(TIPO_CAP);
        // Cria uma árvore de resultados para regiões
        sistema->arvore_regioes = criar_arvore_resultados(TIPO_REGIAO);
        // Inicializa o total de votos válidos com zero
        sistema->total_votos_validos = 0;
        // Inicializa o total de votos em branco com zero
        sistema->total_votos_brancos = 0;
        // Inicializa o total de votos nulos com zero
        sistema->total_votos_nulos = 0;
        // Inicializa o total de eleitores com zero
        sistema->total_eleitores = 0;
        // Inicializa a participação geral com zero
        sistema->participacao_geral = 0.0;
    }
    // Retorna o ponteiro para o sistema criado (ou NULL se falhou)
    return sistema;
}

// Função que libera a memória alocada para o sistema de apuração
void destruir_sistema_apuracao(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido (não é NULL)
    if (!sistema) return;
    
    // Verifica se a árvore de candidatos existe
    if (sistema->arvore_candidatos) {
        // Destrói a árvore de candidatos
        destruir_arvore_resultados(sistema->arvore_candidatos);
    }
    
    // Verifica se a árvore de CAPs existe
    if (sistema->arvore_caps) {
        // Destrói a árvore de CAPs
        destruir_arvore_resultados(sistema->arvore_caps);
    }
    
    // Verifica se a árvore de regiões existe
    if (sistema->arvore_regioes) {
        // Destrói a árvore de regiões
        destruir_arvore_resultados(sistema->arvore_regioes);
    }
    
    // Libera a memória da estrutura principal do sistema
    free(sistema);
}

// Função que inicializa o sistema de apuração com dados existentes
void inicializar_apuracao(SistemaApuracao* sistema, ListaCandidatos* candidatos, 
                         ListaCAPs* caps, ListaEleitores* eleitores) {
    // Verifica se todos os parâmetros são válidos
    if (!sistema || !candidatos || !caps || !eleitores) return;
    
    // Calcula o total de eleitores (tamanho da lista de eleitores)
    sistema->total_eleitores = eleitores->tamanho;
    
    // Chama a função para apurar todos os resultados
    apurar_resultados_completos(sistema, candidatos, caps, eleitores);
    
    // Exibe mensagem de sucesso
    printf("Sistema de apuracao inicializado com sucesso!\n");
    // Exibe o total de eleitores
    printf("Total de eleitores: %d\n", sistema->total_eleitores);
}

// ================= IMPLEMENTAÇÃO APURAÇÃO DE VOTOS =================

// Função para apurar os votos dos candidatos
int apurar_votos_candidatos(SistemaApuracao* sistema, ListaCandidatos* candidatos) {
    // Verifica se os parâmetros são válidos
    if (!sistema || !candidatos) return 0;
    
    // Limpa a árvore existente de candidatos, se houver
    if (sistema->arvore_candidatos) {
        // Destrói a árvore atual
        destruir_arvore_resultados(sistema->arvore_candidatos);
        // Cria uma nova árvore vazia
        sistema->arvore_candidatos = criar_arvore_resultados(TIPO_CANDIDATO);
    }
    
    // Calcula o total de votos válidos
    sistema->total_votos_validos = 0;
    // Percorre todos os candidatos
    for (int i = 0; i < candidatos->tamanho; i++) {
        // Soma os votos de cada candidato ao total
        sistema->total_votos_validos += candidatos->candidatos[i].votos;
    }
    
    // Insere cada candidato na árvore
    for (int i = 0; i < candidatos->tamanho; i++) {
        // Cria um resultado de votação para o candidato
        ResultadoVotacao resultado = criar_resultado_candidato(
            &candidatos->candidatos[i], 
            sistema->total_votos_validos
        );
        
        // Insere o resultado na árvore
        inserir_resultado_arvore(sistema->arvore_candidatos, resultado);
    }
    
    // Atualiza os percentuais na árvore
    atualizar_percentuais_arvore(sistema->arvore_candidatos);
    
    // Exibe informações sobre a apuração
    printf("Votos dos candidatos apurados: %d candidatos, %d votos validos\n",
           candidatos->tamanho, sistema->total_votos_validos);
    
    // Retorna 1 indicando sucesso
    return 1;
}

// Função para apurar os votos dos CAPs
int apurar_votos_caps(SistemaApuracao* sistema, ListaCAPs* caps) {
    // Verifica se os parâmetros são válidos
    if (!sistema || !caps) return 0;
    
    // Limpa a árvore existente de CAPs, se houver
    if (sistema->arvore_caps) {
        // Destrói a árvore atual
        destruir_arvore_resultados(sistema->arvore_caps);
        // Cria uma nova árvore vazia
        sistema->arvore_caps = criar_arvore_resultados(TIPO_CAP);
    }
    
    // Variável para armazenar o total de votos em CAPs
    int total_votos_caps = 0;
    
    // Insere cada CAP na árvore
    for (int i = 0; i < caps->tamanho; i++) {
        // Cria um resultado de votação para o CAP
        ResultadoVotacao resultado = criar_resultado_cap(
            &caps->caps[i], 
            sistema->total_votos_validos
        );
        
        // Insere o resultado na árvore
        inserir_resultado_arvore(sistema->arvore_caps, resultado);
        // Soma os eleitores atendidos por este CAP
        total_votos_caps += caps->caps[i].eleitores_atendidos;
    }
    
    // Atualiza os percentuais na árvore
    atualizar_percentuais_arvore(sistema->arvore_caps);
    
    // Exibe informações sobre a apuração
    printf("Votos dos CAPs apurados: %d CAPs, %d votos totais\n",
           caps->tamanho, total_votos_caps);
    
    // Retorna 1 indicando sucesso
    return 1;
}

// Função para apurar os votos por região
int apurar_votos_regioes(SistemaApuracao* sistema, ListaCAPs* caps) {
    // Verifica se os parâmetros são válidos
    if (!sistema || !caps) return 0;
    
    // Define uma estrutura auxiliar para agrupar votos por região
    typedef struct RegiaoVotos {
        // Nome da região (tamanho máximo definido por MAX_REGIAO)
        char nome[MAX_REGIAO];
        // Total de votos na região
        int votos;
        // Ponteiro para o próximo nó da lista ligada
        struct RegiaoVotos* proximo;
    } RegiaoVotos;
    
    // Cria uma lista ligada vazia para armazenar as regiões
    RegiaoVotos* lista_regioes = NULL;
    // Contador de regiões encontradas
    int total_regioes = 0;
    
    // Percorre todos os CAPs para agrupar votos por região
    for (int i = 0; i < caps->tamanho; i++) {
        // Obtém a região do CAP atual
        char* regiao = caps->caps[i].regiao;
        // Obtém o número de eleitores atendidos por este CAP
        int votos_regiao = caps->caps[i].eleitores_atendidos;
        
        // Busca a região na lista ligada
        RegiaoVotos* atual = lista_regioes;
        RegiaoVotos* anterior = NULL;
        
        // Percorre a lista até encontrar a região ou chegar ao final
        while (atual != NULL && strcmp(atual->nome, regiao) != 0) {
            anterior = atual;
            atual = atual->proximo;
        }
        
        // Verifica se a região já existe na lista
        if (atual) {
            // Região já existe: soma os votos
            atual->votos += votos_regiao;
        } else {
            // Região não existe: cria um novo nó
            RegiaoVotos* nova_regiao = (RegiaoVotos*)malloc(sizeof(RegiaoVotos));
            // Copia o nome da região para o novo nó
            strncpy(nova_regiao->nome, regiao, MAX_REGIAO - 1);
            // Define o número de votos da região
            nova_regiao->votos = votos_regiao;
            // Define o próximo como NULL (final da lista)
            nova_regiao->proximo = NULL;
            
            // Insere o novo nó na lista ligada
            if (anterior) {
                // Se há um nó anterior, insere após ele
                anterior->proximo = nova_regiao;
            } else {
                // Se não há nó anterior, é o primeiro da lista
                lista_regioes = nova_regiao;
            }
            
            // Incrementa o contador de regiões
            total_regioes++;
        }
    }
    
    // Limpa a árvore existente de regiões, se houver
    if (sistema->arvore_regioes) {
        // Destrói a árvore atual
        destruir_arvore_resultados(sistema->arvore_regioes);
        // Cria uma nova árvore vazia
        sistema->arvore_regioes = criar_arvore_resultados(TIPO_REGIAO);
    }
    
    // Percorre a lista ligada de regiões e insere na árvore
    RegiaoVotos* atual = lista_regioes;
    while (atual != NULL) {
        // Cria um resultado de votação para a região
        ResultadoVotacao resultado = criar_resultado_regiao(
            atual->nome,
            atual->votos,
            sistema->total_votos_validos
        );
        
        // Insere o resultado na árvore
        inserir_resultado_arvore(sistema->arvore_regioes, resultado);
        
        // Salva o próximo nó antes de liberar o atual
        RegiaoVotos* proximo = atual->proximo;
        // Libera a memória do nó atual
        free(atual);
        // Avança para o próximo nó
        atual = proximo;
    }
    
    // Atualiza os percentuais na árvore
    atualizar_percentuais_arvore(sistema->arvore_regioes);
    
    // Exibe informações sobre a apuração
    printf("Votos das regioes apurados: %d regioes\n", total_regioes);
    
    // Retorna 1 indicando sucesso
    return 1;
}

// Função para apurar todos os resultados (candidatos, CAPs e regiões)
void apurar_resultados_completos(SistemaApuracao* sistema, ListaCandidatos* candidatos, 
                                ListaCAPs* caps, ListaEleitores* eleitores) {
    // Verifica se todos os parâmetros são válidos
    if (!sistema || !candidatos || !caps || !eleitores) return;
    
    // Exibe cabeçalho indicando início da apuração
    printf("\n=== INICIANDO APURACAO COMPLETA ===\n");
    
    // 1. Apura votos dos candidatos
    printf("1. Apurando votos dos candidatos...\n");
    apurar_votos_candidatos(sistema, candidatos);
    
    // 2. Apura votos dos CAPs
    printf("2. Apurando votos dos CAPs...\n");
    apurar_votos_caps(sistema, caps);
    
    // 3. Apura votos por região
    printf("3. Apurando votos por regiao...\n");
    apurar_votos_regioes(sistema, caps);
    
    // 4. Calcula a participação geral (porcentagem de eleitores que votaram)
    int eleitores_votaram = 0;
    // Percorre todos os CAPs somando os eleitores atendidos
    for (int i = 0; i < caps->tamanho; i++) {
        eleitores_votaram += caps->caps[i].eleitores_atendidos;
    }
    
    // Atualiza o total de eleitores no sistema
    sistema->total_eleitores = eleitores->tamanho;
    
    // Calcula a porcentagem de participação
    if (sistema->total_eleitores > 0) {
        // Calcula: (eleitores que votaram / total de eleitores) * 100
        sistema->participacao_geral = (float)eleitores_votaram / 
                                     sistema->total_eleitores * 100;
    } else {
        // Se não há eleitores, participação é zero
        sistema->participacao_geral = 0.0;
    }
    
    // Exibe resumo da apuração
    printf("\n=== APURACAO CONCLUIDA ===\n");
    printf("Total de eleitores: %d\n", sistema->total_eleitores);
    printf("Eleitores que votaram: %d\n", eleitores_votaram);
    printf("Participacao geral: %.2f%%\n", sistema->participacao_geral);
    printf("Votos validos: %d\n", sistema->total_votos_validos);
}

// Função para atualizar a apuração com um novo voto (em tempo real)
int atualizar_apuracao_com_voto(SistemaApuracao* sistema, Voto* voto, 
                               ListaCandidatos* candidatos, ListaCAPs* caps) {
    // Verifica se todos os parâmetros são válidos
    if (!sistema || !voto || !candidatos || !caps) return 0;
    
    // Incrementa o contador de votos válidos
    sistema->total_votos_validos++;
    
    // Busca o resultado do candidato votado na árvore
    ResultadoVotacao* resultado_candidato = buscar_resultado_por_id(
        sistema->arvore_candidatos, 
        voto->numero_candidato
    );
    
    // Verifica se o candidato já existe na árvore
    if (resultado_candidato) {
        // Candidato existe: incrementa seu contador de votos
        resultado_candidato->votos++;
    } else {
        // Candidato não encontrado: pode ser um novo candidato
        Candidato* candidato = buscar_candidato_por_numero(candidatos, voto->numero_candidato);
        if (candidato) {
            // Cria um novo resultado para o candidato
            ResultadoVotacao novo_resultado = criar_resultado_candidato(
                candidato, 
                sistema->total_votos_validos
            );
            // Insere o novo resultado na árvore
            inserir_resultado_arvore(sistema->arvore_candidatos, novo_resultado);
        }
    }
    
    // Busca o resultado do CAP onde o voto foi registrado
    ResultadoVotacao* resultado_cap = buscar_resultado_por_id(
        sistema->arvore_caps, 
        voto->id_cap
    );
    
    // Verifica se o CAP já existe na árvore
    if (resultado_cap) {
        // CAP existe: incrementa seu contador de votos
        resultado_cap->votos++;
    } else {
        // CAP não encontrado: busca na lista de CAPs
        for (int i = 0; i < caps->tamanho; i++) {
            if (caps->caps[i].id == voto->id_cap) {
                // Cria um novo resultado para o CAP
                ResultadoVotacao novo_resultado = criar_resultado_cap(
                    &caps->caps[i], 
                    sistema->total_votos_validos
                );
                // Insere o novo resultado na árvore
                inserir_resultado_arvore(sistema->arvore_caps, novo_resultado);
                break; // Sai do loop após encontrar
            }
        }
    }
    
    // Atualiza os percentuais em ambas as árvores
    atualizar_percentuais_arvore(sistema->arvore_candidatos);
    atualizar_percentuais_arvore(sistema->arvore_caps);
    
    // Retorna 1 indicando sucesso
    return 1;
}

// ================= IMPLEMENTAÇÃO CONSULTAS E RELATÓRIOS =================

// Função para consultar e exibir resultados dos candidatos
void consultar_resultados_candidatos(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) {
        printf("Sistema de apuracao invalido!\n");
        return;
    }
    
    // Exibe cabeçalho da seção
    printf("\n=== RESULTADOS DOS CANDIDATOS ===\n");
    printf("Total de votos validos: %d\n\n", sistema->total_votos_validos);
    
    // Verifica se a árvore de candidatos está vazia
    if (arvore_vazia(sistema->arvore_candidatos)) {
        printf("Nenhum resultado disponivel.\n");
        return;
    }
    
    // Obtém o número total de nós na árvore
    int n = total_nos_arvore(sistema->arvore_candidatos);
    // Obtém um array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_candidatos, 1);
    
    // Verifica se obteve os resultados com sucesso
    if (resultados) {
        // Percorre o array de resultados
        for (int i = 0; i < n; i++) {
            // Exibe o número da posição (começando em 1)
            printf("%d. ", i + 1);
            // Exibe o resultado formatado
            exibir_resultado_formatado(resultados[i]);
        }
        // Libera a memória alocada para o array
        free(resultados);
    }
}

// Função para consultar e exibir resultados dos CAPs
void consultar_resultados_caps(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) {
        printf("Sistema de apuracao invalido!\n");
        return;
    }
    
    // Exibe cabeçalho da seção
    printf("\n=== RESULTADOS DOS CAPs ===\n");
    printf("Participacao geral: %.2f%%\n\n", sistema->participacao_geral);
    
    // Verifica se a árvore de CAPs está vazia
    if (arvore_vazia(sistema->arvore_caps)) {
        printf("Nenhum resultado disponivel.\n");
        return;
    }
    
    // Obtém o número total de nós na árvore
    int n = total_nos_arvore(sistema->arvore_caps);
    // Obtém um array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_caps, 1);
    
    // Verifica se obteve os resultados com sucesso
    if (resultados) {
        // Percorre o array de resultados
        for (int i = 0; i < n; i++) {
            // Exibe o número da posição (começando em 1)
            printf("%d. ", i + 1);
            // Exibe o resultado formatado
            exibir_resultado_formatado(resultados[i]);
        }
        // Libera a memória alocada para o array
        free(resultados);
    }
}

// Função para consultar e exibir resultados por região
void consultar_resultados_regioes(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) {
        printf("Sistema de apuracao invalido!\n");
        return;
    }
    
    // Exibe cabeçalho da seção
    printf("\n=== RESULTADOS POR REGIAO ===\n\n");
    
    // Verifica se a árvore de regiões está vazia
    if (arvore_vazia(sistema->arvore_regioes)) {
        printf("Nenhum resultado disponivel.\n");
        return;
    }
    
    // Obtém o número total de nós na árvore
    int n = total_nos_arvore(sistema->arvore_regioes);
    // Obtém um array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_regioes, 1);
    
    // Verifica se obteve os resultados com sucesso
    if (resultados) {
        // Percorre o array de resultados
        for (int i = 0; i < n; i++) {
            // Exibe o número da posição (começando em 1)
            printf("%d. ", i + 1);
            // Exibe o resultado formatado
            exibir_resultado_formatado(resultados[i]);
        }
        // Libera a memória alocada para o array
        free(resultados);
    }
}

// Função para consultar e exibir todos os resultados (candidatos, CAPs e regiões)
void consultar_todos_resultados(SistemaApuracao* sistema) {
    // Exibe cabeçalho geral
    printf("\n=== APURACAO COMPLETA ===\n");
    
    // Consulta e exibe resultados dos candidatos
    consultar_resultados_candidatos(sistema);
    printf("\n");
    // Consulta e exibe resultados dos CAPs
    consultar_resultados_caps(sistema);
    printf("\n");
    // Consulta e exibe resultados por região
    consultar_resultados_regioes(sistema);
    
    // Exibe um resumo geral
    printf("\n=== RESUMO GERAL ===\n");
    printf("Total de eleitores: %d\n", sistema->total_eleitores);
    printf("Votos validos: %d\n", sistema->total_votos_validos);
    printf("Participacao: %.2f%%\n", sistema->participacao_geral);
    
    // Exibe votos em branco se houver
    if (sistema->total_votos_brancos > 0) {
        printf("Votos em branco: %d\n", sistema->total_votos_brancos);
    }
    
    // Exibe votos nulos se houver
    if (sistema->total_votos_nulos > 0) {
        printf("Votos nulos: %d\n", sistema->total_votos_nulos);
    }
}

// Função para consultar um candidato específico pelo número
ResultadoVotacao* consultar_candidato_por_numero(SistemaApuracao* sistema, int numero) {
    // Retorna NULL se o sistema for inválido
    if (!sistema) return NULL;
    // Busca o candidato na árvore pelo ID (número)
    return buscar_resultado_por_id(sistema->arvore_candidatos, numero);
}

// Função para consultar um CAP específico pelo ID
ResultadoVotacao* consultar_cap_por_id(SistemaApuracao* sistema, int id) {
    // Retorna NULL se o sistema for inválido
    if (!sistema) return NULL;
    // Busca o CAP na árvore pelo ID
    return buscar_resultado_por_id(sistema->arvore_caps, id);
}

// Função para consultar uma região específica pelo nome
ResultadoVotacao* consultar_regiao_por_nome(SistemaApuracao* sistema, const char* regiao) {
    // Retorna NULL se o sistema for inválido
    if (!sistema) return NULL;
    // Busca a região na árvore pelo nome
    return buscar_resultado_por_nome(sistema->arvore_regioes, regiao);
}

// Função para gerar um relatório completo da apuração
void gerar_relatorio_completo(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) return;
    
    // Exibe formatação do relatório
    printf("\n");
    printf("===================================================\n");
    printf("              RELATÓRIO COMPLETO DE APURAÇÃO       \n");
    printf("===================================================\n");
    // Exibe a data e hora atual
    printf("Data/Hora: %s", ctime(&(time_t){time(NULL)}));
    printf("\n");
    
    // 1. Encontra e exibe o candidato mais votado
    ResultadoVotacao* vencedor = candidato_mais_votado_arvore(sistema->arvore_candidatos);
    if (vencedor) {
        printf("CANDIDATO MAIS VOTADO:\n");
        exibir_resultado_formatado(vencedor);
    }
    
    // 2. Encontra e exibe o CAP com maior participação
    ResultadoVotacao* cap_maior = cap_maior_participacao(sistema->arvore_caps);
    if (cap_maior) {
        printf("\nCAP COM MAIOR PARTICIPAÇÃO:\n");
        exibir_resultado_formatado(cap_maior);
    }
    
    // 3. Encontra e exibe a região com maior participação
    ResultadoVotacao* regiao_maior = regiao_maior_participacao(sistema->arvore_regioes);
    if (regiao_maior) {
        printf("\nREGIÃO COM MAIOR PARTICIPAÇÃO:\n");
        exibir_resultado_formatado(regiao_maior);
    }
    
    // 4. Exibe estatísticas gerais
    printf("\n===================================================\n");
    printf("ESTATÍSTICAS GERAIS:\n");
    printf("  Total de eleitores: %d\n", sistema->total_eleitores);
    printf("  Votos válidos: %d\n", sistema->total_votos_validos);
    printf("  Participação: %.2f%%\n", sistema->participacao_geral);
    
    // 5. Calcula e exibe percentual de votos em branco, se houver
    if (sistema->total_votos_brancos > 0) {
        float percent_brancos = (float)sistema->total_votos_brancos / 
                               sistema->total_votos_validos * 100;
        printf("  Votos em branco: %d (%.2f%%)\n", 
               sistema->total_votos_brancos, percent_brancos);
    }
    
    // 6. Calcula e exibe percentual de votos nulos, se houver
    if (sistema->total_votos_nulos > 0) {
        float percent_nulos = (float)sistema->total_votos_nulos / 
                             sistema->total_votos_validos * 100;
        printf("  Votos nulos: %d (%.2f%%)\n", 
               sistema->total_votos_nulos, percent_nulos);
    }
    
    // Fecha a formatação do relatório
    printf("===================================================\n");
}

// ================= IMPLEMENTAÇÃO RESULTADOS ORDENADOS =================

// Função para exibir ranking de candidatos em formato de tabela
void exibir_ranking_candidatos(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) return;
    
    // Exibe cabeçalho da tabela
    printf("\n=== RANKING DE CANDIDATOS ===\n");
    printf("Posição | Nome                     | Votos  | Percentual\n");
    printf("--------|--------------------------|--------|------------\n");
    
    // Obtém o número total de candidatos
    int n = total_nos_arvore(sistema->arvore_candidatos);
    // Obtém array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_candidatos, 1);
    
    // Verifica se obteve os resultados com sucesso
    if (resultados) {
        // Percorre o array de resultados
        for (int i = 0; i < n; i++) {
            // Exibe cada linha da tabela formatada
            printf("%-8d| %-25s| %-7d| %-10.2f%%\n",
                   i + 1,                     // Posição no ranking
                   resultados[i]->nome,       // Nome do candidato
                   resultados[i]->votos,      // Número de votos
                   resultados[i]->percentual);// Percentual de votos
        }
        // Libera a memória alocada para o array
        free(resultados);
    }
}

// Função para exibir ranking de CAPs em formato de tabela
void exibir_ranking_caps(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) return;
    
    // Exibe cabeçalho da tabela
    printf("\n=== RANKING DE CAPs ===\n");
    printf("Posição | Localização              | Votos  | Percentual\n");
    printf("--------|--------------------------|--------|------------\n");
    
    // Obtém o número total de CAPs
    int n = total_nos_arvore(sistema->arvore_caps);
    // Obtém array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_caps, 1);
    
    // Verifica se obteve os resultados com sucesso
    if (resultados) {
        // Percorre o array de resultados
        for (int i = 0; i < n; i++) {
            // Exibe cada linha da tabela formatada
            printf("%-8d| %-25s| %-7d| %-10.2f%%\n",
                   i + 1,                     // Posição no ranking
                   resultados[i]->nome,       // Localização do CAP
                   resultados[i]->votos,      // Número de votos
                   resultados[i]->percentual);// Percentual de votos
        }
        // Libera a memória alocada para o array
        free(resultados);
    }
}

// ================= IMPLEMENTAÇÃO EXPORTAÇÃO =================

// Função para exportar toda a apuração para arquivos CSV
int exportar_apuracao_completa(SistemaApuracao* sistema, const char* diretorio) {
    // Verifica se os parâmetros são válidos
    if (!sistema || !diretorio) return 0;
    
    // Buffer para construir caminhos de arquivo
    char caminho[500];
    // Obtém o tempo atual
    time_t agora = time(NULL);
    // Converte para estrutura de tempo local
    struct tm* t = localtime(&agora);
    
    // Cria um nome de diretório baseado na data e hora
    snprintf(caminho, sizeof(caminho), "%s/exportacao_%04d%02d%02d_%02d%02d%02d",
             diretorio,
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, // Ano, mês, dia
             t->tm_hour, t->tm_min, t->tm_sec);           // Hora, minuto, segundo
    
    // Em sistema real, aqui criaria o diretório
    // (código comentado pois seria específico do sistema operacional)
    
    // Exporta cada árvore para um arquivo CSV separado
    
    // Exporta resultados dos candidatos
    snprintf(caminho, sizeof(caminho), "%s/candidatos.csv", diretorio);
    exportar_resultados_csv(sistema->arvore_candidatos, caminho);
    
    // Exporta resultados dos CAPs
    snprintf(caminho, sizeof(caminho), "%s/caps.csv", diretorio);
    exportar_resultados_csv(sistema->arvore_caps, caminho);
    
    // Exporta resultados das regiões
    snprintf(caminho, sizeof(caminho), "%s/regioes.csv", diretorio);
    exportar_resultados_csv(sistema->arvore_regioes, caminho);
    
    // Exibe mensagem de confirmação
    printf("Apuracao exportada para o diretorio: %s\n", diretorio);
    
    // Retorna 1 indicando sucesso
    return 1;
}

// Função para exportar resultados finais para um arquivo de texto formatado
int exportar_resultados_finais(SistemaApuracao* sistema, const char* filename) {
    // Verifica se os parâmetros são válidos
    if (!sistema || !filename) return 0;
    
    // Abre o arquivo para escrita
    FILE* arquivo = fopen(filename, "w");
    // Verifica se conseguiu abrir o arquivo
    if (!arquivo) return 0;
    
    // Escreve cabeçalho do relatório
    fprintf(arquivo, "===================================================\n");
    fprintf(arquivo, "          RESULTADOS FINAIS DA ELEIÇÃO            \n");
    fprintf(arquivo, "===================================================\n");
    // Escreve data e hora atual
    fprintf(arquivo, "Data/Hora: %s", ctime(&(time_t){time(NULL)}));
    fprintf(arquivo, "\n");
    
    // Escreve informações sobre o candidato eleito (mais votado)
    ResultadoVotacao* vencedor = candidato_mais_votado_arvore(sistema->arvore_candidatos);
    if (vencedor) {
        fprintf(arquivo, "CANDIDATO ELEITO:\n");
        fprintf(arquivo, "  Nome: %s\n", vencedor->nome);
        fprintf(arquivo, "  Votos: %d\n", vencedor->votos);
        fprintf(arquivo, "  Percentual: %.2f%%\n", vencedor->percentual);
        fprintf(arquivo, "\n");
    }
    
    // Escreve estatísticas gerais
    fprintf(arquivo, "ESTATÍSTICAS GERAIS:\n");
    fprintf(arquivo, "  Total de eleitores: %d\n", sistema->total_eleitores);
    fprintf(arquivo, "  Votos válidos: %d\n", sistema->total_votos_validos);
    fprintf(arquivo, "  Participação: %.2f%%\n", sistema->participacao_geral);
    fprintf(arquivo, "\n");
    
    // Escreve cabeçalho da tabela de ranking
    fprintf(arquivo, "RANKING DE CANDIDATOS:\n");
    fprintf(arquivo, "Pos | Nome                     | Votos  | %%\n");
    fprintf(arquivo, "----|--------------------------|--------|------\n");
    
    // Obtém o número total de candidatos
    int n = total_nos_arvore(sistema->arvore_candidatos);
    // Obtém array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_candidatos, 1);
    
    // Verifica se obteve os resultados com sucesso
    if (resultados) {
        // Percorre o array escrevendo cada candidato
        for (int i = 0; i < n; i++) {
            fprintf(arquivo, "%-4d| %-25s| %-7d| %-6.2f%%\n",
                    i + 1,                     // Posição no ranking
                    resultados[i]->nome,       // Nome do candidato
                    resultados[i]->votos,      // Número de votos
                    resultados[i]->percentual);// Percentual de votos
        }
        // Libera a memória alocada para o array
        free(resultados);
    }
    
    // Fecha o arquivo
    fclose(arquivo);
    // Exibe mensagem de confirmação
    printf("Resultados finais exportados para: %s\n", filename);
    
    // Retorna 1 indicando sucesso
    return 1;
}

// ================= IMPLEMENTAÇÃO VALIDAÇÃO E AUDITORIA =================

// Função para validar a integridade da apuração
int validar_integridade_apuracao(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) return 0;
    
    // Exibe cabeçalho da validação
    printf("\n=== VALIDAÇÃO DA APURAÇÃO ===\n");
    
    // 1. Soma votos da árvore de candidatos
    int total_votos_candidatos = 0;
    // Função auxiliar recursiva para somar votos
    void somar_votos_candidatos(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        // Soma os votos do nó atual
        total_votos_candidatos += no->resultado.votos;
        // Percorre recursivamente a subárvore esquerda
        somar_votos_candidatos(no->esquerda);
        // Percorre recursivamente a subárvore direita
        somar_votos_candidatos(no->direita);
    }
    
    // Inicia a soma a partir da raiz da árvore
    somar_votos_candidatos(sistema->arvore_candidatos->raiz);
    
    // 2. Soma votos da árvore de CAPs
    int total_votos_caps = 0;
    // Função auxiliar recursiva para somar votos
    void somar_votos_caps(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        // Soma os votos do nó atual
        total_votos_caps += no->resultado.votos;
        // Percorre recursivamente a subárvore esquerda
        somar_votos_caps(no->esquerda);
        // Percorre recursivamente a subárvore direita
        somar_votos_caps(no->direita);
    }
    
    // Inicia a soma a partir da raiz da árvore
    somar_votos_caps(sistema->arvore_caps->raiz);
    
    // Exibe os totais calculados
    printf("Votos totais (candidatos): %d\n", total_votos_candidatos);
    printf("Votos totais (CAPs): %d\n", total_votos_caps);
    printf("Votos válidos registrados: %d\n", sistema->total_votos_validos);
    
    // Verifica consistência entre os totais
    if (total_votos_candidatos == sistema->total_votos_validos &&
        total_votos_caps == sistema->total_votos_validos) {
        // Todos os totais batem: integridade validada
        printf("? Integridade validada com sucesso!\n");
        return 1;
    } else {
        // Totais não batem: inconsistência detectada
        printf("? Inconsistência detectada!\n");
        return 0;
    }
}

// ================= IMPLEMENTAÇÃO VISUALIZAÇÃO AVANÇADA =================

// Função para exibir gráfico de barras dos candidatos
void exibir_grafico_barras_candidatos(SistemaApuracao* sistema) {
    // Verifica se o sistema é válido
    if (!sistema) return;
    
    // Exibe cabeçalho do gráfico
    printf("\n=== GRÁFICO DE BARRAS - CANDIDATOS ===\n\n");
    
    // Obtém o número total de candidatos
    int n = total_nos_arvore(sistema->arvore_candidatos);
    // Obtém array ordenado dos resultados (ordem decrescente)
    ResultadoVotacao** resultados = obter_resultados_ordenados(sistema->arvore_candidatos, 1);
    
    // Verifica se obteve resultados e se há candidatos
    if (!resultados || n == 0) return;
    
    // Encontra o número máximo de votos para escalar o gráfico
    int max_votos = resultados[0]->votos;
    
    // Percorre todos os candidatos para exibir suas barras
    for (int i = 0; i < n; i++) {
        // Exibe o nome do candidato alinhado à esquerda
        printf("%-20s: ", resultados[i]->nome);
        
        // Calcula o comprimento da barra proporcional ao máximo
        // A barra terá no máximo 50 caracteres
        int barra_length = (int)((float)resultados[i]->votos / max_votos * 50);
        
        // Desenha a barra usando caracteres '¦'
        for (int j = 0; j < barra_length; j++) {
            printf("¦");
        }
        
        // Exibe os números após a barra
        printf(" %d votos (%.1f%%)\n", resultados[i]->votos, resultados[i]->percentual);
    }
    
    // Libera a memória alocada para o array
    free(resultados);
}
