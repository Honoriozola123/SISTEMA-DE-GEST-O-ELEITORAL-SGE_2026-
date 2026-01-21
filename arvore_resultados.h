// Diretiva de pré-processador para evitar inclusões múltiplas
#ifndef ARVORE_RESULTADOS_H
#define ARVORE_RESULTADOS_H

// Inclui o cabeçalho com as estruturas de dados básicas
#include "estruturas.h"

// ================= FUNÇÕES BÁSICAS DA ÁRVORE =================

// Criação e destruição
// Protótipo da função que cria uma árvore de resultados
ArvoreResultados* criar_arvore_resultados(TipoNoArvore tipo);
// Protótipo da função que destrói uma árvore de resultados
void destruir_arvore_resultados(ArvoreResultados* arvore);

// Criação de nós
// Protótipo da função que cria um nó de resultado
NoArvoreResultados* criar_no_resultado(ResultadoVotacao resultado);
// Protótipo da função que destrói um nó de resultado
void destruir_no_resultado(NoArvoreResultados* no);

// Operações de inserção
// Protótipo da função que insere resultado na árvore
int inserir_resultado_arvore(ArvoreResultados* arvore, ResultadoVotacao resultado);
// Protótipo da função recursiva que insere nó na árvore
NoArvoreResultados* inserir_no_recursivo(NoArvoreResultados* raiz, NoArvoreResultados* novo_no);

// Operações de busca
// Protótipo da função que busca resultado por ID
ResultadoVotacao* buscar_resultado_por_id(ArvoreResultados* arvore, int id);
// Protótipo da função recursiva que busca nó por ID
NoArvoreResultados* buscar_no_recursivo(NoArvoreResultados* raiz, int id);
// Protótipo da função que busca resultado por nome
ResultadoVotacao* buscar_resultado_por_nome(ArvoreResultados* arvore, const char* nome);

// Remoção
// Protótipo da função que remove resultado da árvore
int remover_resultado_arvore(ArvoreResultados* arvore, int id);
// Protótipo da função recursiva que remove nó da árvore
NoArvoreResultados* remover_no_recursivo(NoArvoreResultados* raiz, int id);
// Protótipo da função que encontra nó mínimo
NoArvoreResultados* encontrar_minimo(NoArvoreResultados* raiz);

// ================= PERCURSOS DA ÁRVORE =================

// Percursos
// Protótipo da função de percurso em ordem
void percurso_em_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*));
// Protótipo da função de percurso pré-ordem
void percurso_pre_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*));
// Protótipo da função de percurso pós-ordem
void percurso_pos_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*));

// Percursos com filtro
// Protótipo da função de percurso em ordem filtrado por tipo
void percurso_em_ordem_filtrado(NoArvoreResultados* raiz, TipoNoArvore tipo, 
                               void (*callback)(ResultadoVotacao*));

// ================= ÁRVORE AVL (BALANCEAMENTO) =================

// Funções de balanceamento
// Protótipo da função que retorna altura de um nó
int altura_no(NoArvoreResultados* no);
// Protótipo da função que calcula fator de balanceamento
int fator_balanceamento(NoArvoreResultados* no);
// Protótipo da função que rotaciona à direita
NoArvoreResultados* rotacionar_direita(NoArvoreResultados* y);
// Protótipo da função que rotaciona à esquerda
NoArvoreResultados* rotacionar_esquerda(NoArvoreResultados* x);
// Protótipo da função que balanceia um nó
NoArvoreResultados* balancear_no(NoArvoreResultados* no);

// ================= OPERAÇÕES ESPECÍFICAS PARA RESULTADOS =================

// Criação de resultados específicos
// Protótipo da função que cria resultado para candidato
ResultadoVotacao criar_resultado_candidato(Candidato* candidato, int total_votos);
// Protótipo da função que cria resultado para CAP
ResultadoVotacao criar_resultado_cap(CAP* cap, int total_votos);
// Protótipo da função que cria resultado para região
ResultadoVotacao criar_resultado_regiao(const char* regiao, int votos_regiao, int total_votos);

// Atualização de resultados
// Protótipo da função que atualiza votos de um resultado
int atualizar_votos_resultado(ArvoreResultados* arvore, int id, int votos_adicionais);
// Protótipo da função que atualiza percentuais da árvore
int atualizar_percentuais_arvore(ArvoreResultados* arvore);

// ================= ESTATÍSTICAS E CONSULTAS =================

// Consultas básicas
// Protótipo da função que conta total de nós
int total_nos_arvore(ArvoreResultados* arvore);
// Protótipo da função que retorna altura da árvore
int altura_arvore(ArvoreResultados* arvore);
// Protótipo da função que verifica se árvore está vazia
int arvore_vazia(ArvoreResultados* arvore);

// Consultas específicas
// Protótipo da função que encontra candidato mais votado
ResultadoVotacao* candidato_mais_votado_arvore(ArvoreResultados* arvore);
// Protótipo da função que encontra candidato menos votado
ResultadoVotacao* candidato_menos_votado_arvore(ArvoreResultados* arvore);
// Protótipo da função que encontra CAP com maior participação
ResultadoVotacao* cap_maior_participacao(ArvoreResultados* arvore);
// Protótipo da função que encontra região com maior participação
ResultadoVotacao* regiao_maior_participacao(ArvoreResultados* arvore);

// Consultas por faixa
// Protótipo da função que lista candidatos acima de limite de votos
void candidatos_acima_de_limite(ArvoreResultados* arvore, int limite_votos);
// Protótipo da função que lista CAPs acima de limite de votos
void caps_acima_de_limite(ArvoreResultados* arvore, int limite_votos);

// ================= VISUALIZAÇÃO E EXPORTAÇÃO =================

// Exibição formatada
// Protótipo da função que exibe resultado formatado
void exibir_resultado_formatado(ResultadoVotacao* resultado);
// Protótipo da função que exibe árvore de resultados
void exibir_arvore_resultados(ArvoreResultados* arvore);
// Protótipo da função que exibe árvore na vertical
void exibir_arvore_vertical(NoArvoreResultados* raiz, int nivel);
// Protótipo da função que exibe árvore na horizontal
void exibir_arvore_horizontal(NoArvoreResultados* raiz, int espaco);

// Exportação
// Protótipo da função que exporta resultados para CSV
int exportar_resultados_csv(ArvoreResultados* arvore, const char* filename);
// Protótipo da função que exporta resultados para JSON
int exportar_resultados_json(ArvoreResultados* arvore, const char* filename);
// Protótipo da função que exporta árvore para formato DOT (Graphviz)
int exportar_arvore_dot(ArvoreResultados* arvore, const char* filename);

// ================= FUNÇÕES DE ORDENAÇÃO =================

// Ordenação dos resultados
// Protótipo da função que ordena resultados em ordem decrescente
void ordenar_resultados_decrescente(ArvoreResultados* arvore, 
                                   ResultadoVotacao** array, int* indice);
// Protótipo da função que ordena resultados em ordem crescente
void ordenar_resultados_crescente(ArvoreResultados* arvore, 
                                 ResultadoVotacao** array, int* indice);
// Protótipo da função que retorna array de resultados ordenados
ResultadoVotacao** obter_resultados_ordenados(ArvoreResultados* arvore, int ordem);

// Fim da diretiva de pré-processador
#endif
