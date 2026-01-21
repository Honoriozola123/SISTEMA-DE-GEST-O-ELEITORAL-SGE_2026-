// Inclusão das bibliotecas necessárias
#include <stdio.h>  // Para funções de entrada/saída
#include <stdlib.h> // Para alocação de memória
#include <string.h> // Para manipulação de strings
#include <math.h>   // Para funções matemáticas (não usada neste arquivo)
#include "arvore_resultados.h" // Cabeçalho das funções da árvore

// Função auxiliar para máximo (APENAS ESTA!)
// Função estática (visível apenas neste arquivo) que retorna o máximo entre dois inteiros
static int max(int a, int b) {
    return (a > b) ? a : b; // Operador ternário: se a > b retorna a, senão retorna b
}

// Função que cria uma nova árvore de resultados
ArvoreResultados* criar_arvore_resultados(TipoNoArvore tipo) {
    // Aloca memória para a estrutura ArvoreResultados
    ArvoreResultados* arvore = (ArvoreResultados*)malloc(sizeof(ArvoreResultados));
    // Verifica se a alocação foi bem sucedida
    if (arvore) {
        arvore->raiz = NULL; // Inicializa a raiz como NULL (árvore vazia)
        arvore->total_votos = 0; // Inicializa contador de votos
        arvore->total_eleitores = 0; // Inicializa contador de eleitores
        arvore->participacao = 0.0; // Inicializa participação
        arvore->tipo_arvore = tipo; // Define o tipo da árvore
    }
    return arvore; // Retorna a árvore criada (ou NULL se falhou)
}

// Função que destrói uma árvore de resultados e libera toda sua memória
void destruir_arvore_resultados(ArvoreResultados* arvore) {
    // Verifica se a árvore é válida
    if (!arvore) return;
    
    // Função auxiliar para destruir recursivamente
    void destruir_recursivo(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        // Destrói recursivamente a subárvore esquerda
        destruir_recursivo(no->esquerda);
        // Destrói recursivamente a subárvore direita
        destruir_recursivo(no->direita);
        // Libera a memória do nó atual
        free(no);
    }
    
    // Chama a função auxiliar começando pela raiz
    destruir_recursivo(arvore->raiz);
    // Libera a estrutura principal da árvore
    free(arvore);
}

// Função que cria um novo nó para a árvore de resultados
NoArvoreResultados* criar_no_resultado(ResultadoVotacao resultado) {
    // Aloca memória para um novo nó
    NoArvoreResultados* novo_no = (NoArvoreResultados*)malloc(sizeof(NoArvoreResultados));
    // Verifica se a alocação foi bem sucedida
    if (novo_no) {
        novo_no->resultado = resultado; // Copia o resultado para o nó
        novo_no->esquerda = NULL; // Inicializa filho esquerdo como NULL
        novo_no->direita = NULL;  // Inicializa filho direito como NULL
        novo_no->altura = 1; // Altura inicial (nó folha tem altura 1)
    }
    return novo_no; // Retorna o novo nó (ou NULL se falhou)
}

// Função que destrói um nó da árvore
void destruir_no_resultado(NoArvoreResultados* no) {
    // Libera a memória do nó se ele não for NULL
    if (no) free(no);
}

// Função que insere um resultado na árvore
int inserir_resultado_arvore(ArvoreResultados* arvore, ResultadoVotacao resultado) {
    // Verifica se a árvore é válida
    if (!arvore) return 0;
    
    // Cria um novo nó com o resultado
    NoArvoreResultados* novo_no = criar_no_resultado(resultado);
    // Verifica se a criação foi bem sucedida
    if (!novo_no) return 0;
    
    // Insere o novo nó na árvore (chama função recursiva)
    arvore->raiz = inserir_no_recursivo(arvore->raiz, novo_no);
    
    // Atualiza estatísticas da árvore
    arvore->total_votos += resultado.votos;
    
    // Retorna 1 indicando sucesso
    return 1;
}

// Função recursiva para inserir um nó na árvore
NoArvoreResultados* inserir_no_recursivo(NoArvoreResultados* raiz, NoArvoreResultados* novo_no) {
    // Caso base: árvore vazia, o novo nó se torna a raiz
    if (!raiz) return novo_no;
    
    // Comparar pelo ID para inserção ordenada
    if (novo_no->resultado.id < raiz->resultado.id) {
        // ID menor: vai para subárvore esquerda
        raiz->esquerda = inserir_no_recursivo(raiz->esquerda, novo_no);
    } else if (novo_no->resultado.id > raiz->resultado.id) {
        // ID maior: vai para subárvore direita
        raiz->direita = inserir_no_recursivo(raiz->direita, novo_no);
    } else {
        // ID já existe, atualizar votos (soma votos do novo ao existente)
        raiz->resultado.votos += novo_no->resultado.votos;
        // Libera o novo nó (não será usado)
        free(novo_no);
        // Retorna a raiz (sem alterações na estrutura)
        return raiz;
    }
    
    // Atualizar altura do nó atual
    raiz->altura = 1 + max(altura_no(raiz->esquerda), altura_no(raiz->direita));
    
    // Balancear a árvore (AVL) e retornar a nova raiz
    return balancear_no(raiz);
}

// Função para buscar um resultado por ID
ResultadoVotacao* buscar_resultado_por_id(ArvoreResultados* arvore, int id) {
    // Verifica se a árvore é válida
    if (!arvore) return NULL;
    
    // Busca o nó com o ID especificado
    NoArvoreResultados* no = buscar_no_recursivo(arvore->raiz, id);
    // Retorna o resultado (ou NULL se não encontrado)
    return no ? &(no->resultado) : NULL;
}

// Função recursiva para buscar um nó por ID
NoArvoreResultados* buscar_no_recursivo(NoArvoreResultados* raiz, int id) {
    // Caso base: raiz nula ou ID encontrado
    if (!raiz || raiz->resultado.id == id) {
        return raiz;
    }
    
    // Se o ID for menor, busca na subárvore esquerda
    if (id < raiz->resultado.id) {
        return buscar_no_recursivo(raiz->esquerda, id);
    } else {
        // Se o ID for maior, busca na subárvore direita
        return buscar_no_recursivo(raiz->direita, id);
    }
}

// Função para buscar um resultado por nome
ResultadoVotacao* buscar_resultado_por_nome(ArvoreResultados* arvore, const char* nome) {
    // Verifica se a árvore e o nome são válidos
    if (!arvore || !nome) return NULL;
    
    // Função auxiliar para busca por nome (busca linear em toda árvore)
    ResultadoVotacao* buscar_por_nome_recursivo(NoArvoreResultados* raiz, const char* nome) {
        // Caso base: nó nulo
        if (!raiz) return NULL;
        
        // Verifica se o nome do nó atual é o procurado
        if (strcmp(raiz->resultado.nome, nome) == 0) {
            return &(raiz->resultado);
        }
        
        // Busca na subárvore esquerda
        ResultadoVotacao* encontrado = buscar_por_nome_recursivo(raiz->esquerda, nome);
        // Se encontrou na esquerda, retorna
        if (encontrado) return encontrado;
        
        // Se não encontrou na esquerda, busca na direita
        return buscar_por_nome_recursivo(raiz->direita, nome);
    }
    
    // Inicia a busca a partir da raiz
    return buscar_por_nome_recursivo(arvore->raiz, nome);
}

// Função para remover um resultado da árvore por ID
int remover_resultado_arvore(ArvoreResultados* arvore, int id) {
    // Verifica se a árvore é válida
    if (!arvore) return 0;
    
    // Chama função recursiva para remover o nó
    arvore->raiz = remover_no_recursivo(arvore->raiz, id);
    return 1; // Retorna sucesso (mesmo se não encontrou o ID)
}

// Função recursiva para remover um nó por ID
NoArvoreResultados* remover_no_recursivo(NoArvoreResultados* raiz, int id) {
    // Caso base: nó não encontrado
    if (!raiz) return NULL;
    
    // Busca o nó a ser removido
    if (id < raiz->resultado.id) {
        // ID menor: busca na subárvore esquerda
        raiz->esquerda = remover_no_recursivo(raiz->esquerda, id);
    } else if (id > raiz->resultado.id) {
        // ID maior: busca na subárvore direita
        raiz->direita = remover_no_recursivo(raiz->direita, id);
    } else {
        // Nó encontrado
        if (!raiz->esquerda || !raiz->direita) {
            // Caso 1: um ou nenhum filho
            NoArvoreResultados* temp = raiz->esquerda ? raiz->esquerda : raiz->direita;
            
            if (!temp) {
                // Caso 1a: nenhum filho
                temp = raiz;
                raiz = NULL;
            } else {
                // Caso 1b: um filho
                *raiz = *temp; // Copia os dados do filho para o nó atual
            }
            
            free(temp); // Libera o nó removido
        } else {
            // Caso 2: dois filhos
            NoArvoreResultados* temp = encontrar_minimo(raiz->direita);
            // Substitui os dados do nó atual pelos do sucessor in-order
            raiz->resultado = temp->resultado;
            // Remove o sucessor in-order da subárvore direita
            raiz->direita = remover_no_recursivo(raiz->direita, temp->resultado.id);
        }
    }
    
    // Se a árvore ficou vazia após remoção
    if (!raiz) return NULL;
    
    // Atualizar altura e balancear
    raiz->altura = 1 + max(altura_no(raiz->esquerda), altura_no(raiz->direita));
    return balancear_no(raiz);
}

// Função para encontrar o nó com menor valor (mais à esquerda)
NoArvoreResultados* encontrar_minimo(NoArvoreResultados* raiz) {
    NoArvoreResultados* atual = raiz;
    // Percorre para a esquerda até encontrar o nó mais à esquerda
    while (atual->esquerda != NULL) {
        atual = atual->esquerda;
    }
    return atual;
}

// ================= IMPLEMENTAÇÃO PERCURSOS =================

// Função que realiza percurso em ordem (esquerda, raiz, direita)
void percurso_em_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*)) {
    // Caso base: nó nulo
    if (!raiz) return;
    
    // Percorre subárvore esquerda
    percurso_em_ordem(raiz->esquerda, callback);
    // Executa callback no nó atual (se fornecido)
    if (callback) callback(&raiz->resultado);
    // Percorre subárvore direita
    percurso_em_ordem(raiz->direita, callback);
}

// Função que realiza percurso pré-ordem (raiz, esquerda, direita)
void percurso_pre_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*)) {
    // Caso base: nó nulo
    if (!raiz) return;
    
    // Executa callback no nó atual (se fornecido)
    if (callback) callback(&raiz->resultado);
    // Percorre subárvore esquerda
    percurso_pre_ordem(raiz->esquerda, callback);
    // Percorre subárvore direita
    percurso_pre_ordem(raiz->direita, callback);
}

// Função que realiza percurso pós-ordem (esquerda, direita, raiz)
void percurso_pos_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*)) {
    // Caso base: nó nulo
    if (!raiz) return;
    
    // Percorre subárvore esquerda
    percurso_pos_ordem(raiz->esquerda, callback);
    // Percorre subárvore direita
    percurso_pos_ordem(raiz->direita, callback);
    // Executa callback no nó atual (se fornecido)
    if (callback) callback(&raiz->resultado);
}

// Função que realiza percurso em ordem filtrado por tipo
void percurso_em_ordem_filtrado(NoArvoreResultados* raiz, TipoNoArvore tipo, 
                               void (*callback)(ResultadoVotacao*)) {
    // Caso base: nó nulo
    if (!raiz) return;
    
    // Percorre subárvore esquerda
    percurso_em_ordem_filtrado(raiz->esquerda, tipo, callback);
    // Executa callback apenas se o tipo do nó for o especificado
    if (raiz->resultado.tipo == tipo && callback) {
        callback(&raiz->resultado);
    }
    // Percorre subárvore direita
    percurso_em_ordem_filtrado(raiz->direita, tipo, callback);
}

// ================= IMPLEMENTAÇÃO ÁRVORE AVL =================

// Função que retorna a altura de um nó
int altura_no(NoArvoreResultados* no) {
    // Retorna 0 se o nó for NULL, senão retorna sua altura
    return no ? no->altura : 0;
}

// Função que calcula o fator de balanceamento de um nó
int fator_balanceamento(NoArvoreResultados* no) {
    // Se nó for NULL, retorna 0
    if (!no) return 0;
    // Fator = altura(esquerda) - altura(direita)
    return altura_no(no->esquerda) - altura_no(no->direita);
}

// Função que realiza rotação simples à direita
NoArvoreResultados* rotacionar_direita(NoArvoreResultados* y) {
    // x é o filho esquerdo de y
    NoArvoreResultados* x = y->esquerda;
    // T2 é a subárvore direita de x
    NoArvoreResultados* T2 = x->direita;
    
    // Realiza a rotação
    x->direita = y; // y se torna filho direito de x
    y->esquerda = T2; // T2 se torna filho esquerdo de y
    
    // Atualizar alturas
    y->altura = max(altura_no(y->esquerda), altura_no(y->direita)) + 1;
    x->altura = max(altura_no(x->esquerda), altura_no(x->direita)) + 1;
    
    // Retorna a nova raiz (x)
    return x;
}

// Função que realiza rotação simples à esquerda
NoArvoreResultados* rotacionar_esquerda(NoArvoreResultados* x) {
    // y é o filho direito de x
    NoArvoreResultados* y = x->direita;
    // T2 é a subárvore esquerda de y
    NoArvoreResultados* T2 = y->esquerda;
    
    // Realiza a rotação
    y->esquerda = x; // x se torna filho esquerdo de y
    x->direita = T2; // T2 se torna filho direito de x
    
    // Atualizar alturas
    x->altura = max(altura_no(x->esquerda), altura_no(x->direita)) + 1;
    y->altura = max(altura_no(y->esquerda), altura_no(y->direita)) + 1;
    
    // Retorna a nova raiz (y)
    return y;
}

// Função que balanceia um nó (implementação AVL)
NoArvoreResultados* balancear_no(NoArvoreResultados* no) {
    // Se nó for NULL, retorna NULL
    if (!no) return no;
    
    // Calcula fator de balanceamento
    int balance = fator_balanceamento(no);
    
    // Caso Esquerda-Esquerda (LL)
    if (balance > 1 && fator_balanceamento(no->esquerda) >= 0) {
        return rotacionar_direita(no);
    }
    
    // Caso Direita-Direita (RR)
    if (balance < -1 && fator_balanceamento(no->direita) <= 0) {
        return rotacionar_esquerda(no);
    }
    
    // Caso Esquerda-Direita (LR)
    if (balance > 1 && fator_balanceamento(no->esquerda) < 0) {
        no->esquerda = rotacionar_esquerda(no->esquerda);
        return rotacionar_direita(no);
    }
    
    // Caso Direita-Esquerda (RL)
    if (balance < -1 && fator_balanceamento(no->direita) > 0) {
        no->direita = rotacionar_direita(no->direita);
        return rotacionar_esquerda(no);
    }
    
    // Nó já balanceado
    return no;
}

// ================= IMPLEMENTAÇÃO OPERAÇÕES ESPECÍFICAS =================

// Função que cria um resultado de votação para um candidato
ResultadoVotacao criar_resultado_candidato(Candidato* candidato, int total_votos) {
    ResultadoVotacao resultado;
    
    // Copia dados do candidato para o resultado
    resultado.id = candidato->numero;
    strncpy(resultado.nome, candidato->nome, MAX_NOME - 1);
    resultado.tipo = TIPO_CANDIDATO;
    resultado.votos = candidato->votos;
    resultado.votos_validos = candidato->votos;
    resultado.votos_brancos = 0;
    resultado.votos_nulos = 0;
    
    // Calcula percentual
    if (total_votos > 0) {
        resultado.percentual = (float)candidato->votos / total_votos * 100;
    } else {
        resultado.percentual = 0.0;
    }
    
    return resultado;
}

// Função que cria um resultado de votação para um CAP
ResultadoVotacao criar_resultado_cap(CAP* cap, int total_votos) {
    ResultadoVotacao resultado;
    
    // Copia dados do CAP para o resultado
    resultado.id = cap->id;
    strncpy(resultado.nome, cap->localizacao, MAX_NOME - 1);
    resultado.tipo = TIPO_CAP;
    resultado.votos = cap->eleitores_atendidos;
    resultado.votos_validos = cap->eleitores_atendidos;
    resultado.votos_brancos = 0;
    resultado.votos_nulos = 0;
    
    // Calcula percentual
    if (total_votos > 0) {
        resultado.percentual = (float)cap->eleitores_atendidos / total_votos * 100;
    } else {
        resultado.percentual = 0.0;
    }
    
    return resultado;
}

// Função que cria um resultado de votação para uma região
ResultadoVotacao criar_resultado_regiao(const char* regiao, int votos_regiao, int total_votos) {
    ResultadoVotacao resultado;
    
    // Gera ID baseado no hash da região (soma dos caracteres)
    resultado.id = 0;
    for (int i = 0; regiao[i] != '\0'; i++) {
        resultado.id += regiao[i];
    }
    
    // Copia dados da região
    strncpy(resultado.nome, regiao, MAX_NOME - 1);
    resultado.tipo = TIPO_REGIAO;
    resultado.votos = votos_regiao;
    resultado.votos_validos = votos_regiao;
    resultado.votos_brancos = 0;
    resultado.votos_nulos = 0;
    
    // Calcula percentual
    if (total_votos > 0) {
        resultado.percentual = (float)votos_regiao / total_votos * 100;
    } else {
        resultado.percentual = 0.0;
    }
    
    return resultado;
}

// Função que atualiza os votos de um resultado existente
int atualizar_votos_resultado(ArvoreResultados* arvore, int id, int votos_adicionais) {
    // Verifica se a árvore é válida
    if (!arvore) return 0;
    
    // Busca o resultado pelo ID
    ResultadoVotacao* resultado = buscar_resultado_por_id(arvore, id);
    // Se não encontrou, retorna 0
    if (!resultado) return 0;
    
    // Atualiza votos do resultado
    resultado->votos += votos_adicionais;
    // Atualiza total de votos da árvore
    arvore->total_votos += votos_adicionais;
    
    return 1; // Retorna sucesso
}

// Função que atualiza todos os percentuais na árvore
int atualizar_percentuais_arvore(ArvoreResultados* arvore) {
    // Verifica se a árvore é válida e tem votos
    if (!arvore || arvore->total_votos == 0) return 0;
    
    // Função auxiliar recursiva para atualizar percentuais
    void atualizar_percentual_no(NoArvoreResultados* no, int total_votos) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Calcula percentual do nó atual
        no->resultado.percentual = (float)no->resultado.votos / total_votos * 100;
        // Atualiza subárvore esquerda
        atualizar_percentual_no(no->esquerda, total_votos);
        // Atualiza subárvore direita
        atualizar_percentual_no(no->direita, total_votos);
    }
    
    // Inicia atualização a partir da raiz
    atualizar_percentual_no(arvore->raiz, arvore->total_votos);
    return 1; // Retorna sucesso
}

// ================= IMPLEMENTAÇÃO ESTATÍSTICAS E CONSULTAS =================

// Função que conta o total de nós na árvore
int total_nos_arvore(ArvoreResultados* arvore) {
    // Verifica se a árvore é válida
    if (!arvore) return 0;
    
    // Função auxiliar recursiva para contar nós
    int contar_nos_recursivo(NoArvoreResultados* raiz) {
        // Caso base: nó nulo
        if (!raiz) return 0;
        // Conta este nó + nós da esquerda + nós da direita
        return 1 + contar_nos_recursivo(raiz->esquerda) + contar_nos_recursivo(raiz->direita);
    }
    
    // Inicia contagem a partir da raiz
    return contar_nos_recursivo(arvore->raiz);
}

// Função que retorna a altura da árvore
int altura_arvore(ArvoreResultados* arvore) {
    // Se árvore vazia ou inválida, altura é 0
    if (!arvore || !arvore->raiz) return 0;
    // Retorna altura da raiz
    return arvore->raiz->altura;
}

// Função que verifica se a árvore está vazia
int arvore_vazia(ArvoreResultados* arvore) {
    // Retorna verdadeiro se árvore inválida ou raiz NULL
    return (!arvore || !arvore->raiz);
}

// Função que encontra o candidato mais votado na árvore
ResultadoVotacao* candidato_mais_votado_arvore(ArvoreResultados* arvore) {
    // Verifica se é árvore de candidatos
    if (!arvore || arvore->tipo_arvore != TIPO_CANDIDATO) return NULL;
    
    NoArvoreResultados* atual = arvore->raiz;
    NoArvoreResultados* mais_votado = NULL;
    
    // Função auxiliar recursiva para encontrar máximo
    void encontrar_maximo_recursivo(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Atualiza mais_votado se necessário
        if (!mais_votado || no->resultado.votos > mais_votado->resultado.votos) {
            mais_votado = no;
        }
        
        // Busca nas subárvores
        encontrar_maximo_recursivo(no->esquerda);
        encontrar_maximo_recursivo(no->direita);
    }
    
    // Inicia busca a partir da raiz
    encontrar_maximo_recursivo(atual);
    // Retorna resultado (ou NULL se árvore vazia)
    return mais_votado ? &(mais_votado->resultado) : NULL;
}

// Função que encontra o candidato menos votado na árvore
ResultadoVotacao* candidato_menos_votado_arvore(ArvoreResultados* arvore) {
    // Verifica se é árvore de candidatos
    if (!arvore || arvore->tipo_arvore != TIPO_CANDIDATO) return NULL;
    
    NoArvoreResultados* atual = arvore->raiz;
    NoArvoreResultados* menos_votado = NULL;
    
    // Função auxiliar recursiva para encontrar mínimo
    void encontrar_minimo_recursivo(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Atualiza menos_votado se necessário
        if (!menos_votado || no->resultado.votos < menos_votado->resultado.votos) {
            menos_votado = no;
        }
        
        // Busca nas subárvores
        encontrar_minimo_recursivo(no->esquerda);
        encontrar_minimo_recursivo(no->direita);
    }
    
    // Inicia busca a partir da raiz
    encontrar_minimo_recursivo(atual);
    // Retorna resultado (ou NULL se árvore vazia)
    return menos_votado ? &(menos_votado->resultado) : NULL;
}

// Função que encontra o CAP com maior participação
ResultadoVotacao* cap_maior_participacao(ArvoreResultados* arvore) {
    // Verifica se é árvore de CAPs
    if (!arvore || arvore->tipo_arvore != TIPO_CAP) return NULL;
    
    // Reutiliza lógica de candidato_mais_votado_arvore
    return candidato_mais_votado_arvore(arvore);
}

// Função que encontra a região com maior participação
ResultadoVotacao* regiao_maior_participacao(ArvoreResultados* arvore) {
    // Verifica se é árvore de regiões
    if (!arvore || arvore->tipo_arvore != TIPO_REGIAO) return NULL;
    
    // Reutiliza lógica de candidato_mais_votado_arvore
    return candidato_mais_votado_arvore(arvore);
}

// Função que lista candidatos com votos acima de um limite
void candidatos_acima_de_limite(ArvoreResultados* arvore, int limite_votos) {
    // Verifica se é árvore de candidatos
    if (!arvore || arvore->tipo_arvore != TIPO_CANDIDATO) return;
    
    printf("\n=== CANDIDATOS COM MAIS DE %d VOTOS ===\n", limite_votos);
    
    // Função auxiliar recursiva para verificar limite
    void verificar_limite_recursivo(NoArvoreResultados* no, int limite) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Verifica subárvore esquerda
        verificar_limite_recursivo(no->esquerda, limite);
        
        // Se votos acima do limite, exibe resultado
        if (no->resultado.votos > limite) {
            exibir_resultado_formatado(&no->resultado);
        }
        
        // Verifica subárvore direita
        verificar_limite_recursivo(no->direita, limite);
    }
    
    // Inicia verificação a partir da raiz
    verificar_limite_recursivo(arvore->raiz, limite_votos);
}

// Função que lista CAPs com votos acima de um limite
void caps_acima_de_limite(ArvoreResultados* arvore, int limite_votos) {
    // Verifica se é árvore de CAPs
    if (!arvore || arvore->tipo_arvore != TIPO_CAP) return;
    
    printf("\n=== CAPs COM MAIS DE %d VOTOS ===\n", limite_votos);
    // Reutiliza lógica de candidatos_acima_de_limite
    candidatos_acima_de_limite(arvore, limite_votos);
}

// ================= IMPLEMENTAÇÃO VISUALIZAÇÃO E EXPORTAÇÃO =================

// Função que exibe um resultado formatado
void exibir_resultado_formatado(ResultadoVotacao* resultado) {
    // Verifica se resultado é válido
    if (!resultado) {
        printf("Resultado invalido!\n");
        return;
    }
    
    // Converte tipo para string
    const char* tipo_str;
    switch (resultado->tipo) {
        case TIPO_CANDIDATO: tipo_str = "CANDIDATO"; break;
        case TIPO_CAP: tipo_str = "CAP"; break;
        case TIPO_REGIAO: tipo_str = "REGIAO"; break;
        default: tipo_str = "DESCONHECIDO";
    }
    
    // Exibe resultado em formato de tabela
    printf("+-----------------------------------------------------+\n");
    printf("¦ %-20s: %-25s ¦\n", "Tipo", tipo_str);
    printf("¦ %-20s: %-25s ¦\n", "Nome", resultado->nome);
    printf("¦ %-20s: %-25d ¦\n", "ID", resultado->id);
    printf("¦ %-20s: %-25d ¦\n", "Votos", resultado->votos);
    printf("¦ %-20s: %-24.2f%% ¦\n", "Percentual", resultado->percentual);
    printf("+-----------------------------------------------------+\n");
}

// Função que exibe informações da árvore completa
void exibir_arvore_resultados(ArvoreResultados* arvore) {
    // Verifica se árvore é válida
    if (!arvore) {
        printf("Arvore invalida!\n");
        return;
    }
    
    printf("\n=== ARVORE DE RESULTADOS ===\n");
    printf("Tipo: ");
    // Exibe tipo da árvore
    switch (arvore->tipo_arvore) {
        case TIPO_CANDIDATO: printf("CANDIDATOS\n"); break;
        case TIPO_CAP: printf("CAPs\n"); break;
        case TIPO_REGIAO: printf("REGIOES\n"); break;
        default: printf("DESCONHECIDO\n");
    }
    
    // Exibe estatísticas da árvore
    printf("Total de nos: %d\n", total_nos_arvore(arvore));
    printf("Altura: %d\n", altura_arvore(arvore));
    printf("Total de votos: %d\n\n", arvore->total_votos);
    
    printf("Resultados (em ordem):\n");
    // Exibe todos os resultados em ordem
    percurso_em_ordem(arvore->raiz, exibir_resultado_formatado);
}

// Função que exibe árvore na vertical (com indentação)
void exibir_arvore_vertical(NoArvoreResultados* raiz, int nivel) {
    // Caso base: nó nulo
    if (!raiz) return;
    
    // Exibe subárvore direita (maior)
    exibir_arvore_vertical(raiz->direita, nivel + 1);
    
    // Indenta de acordo com o nível
    for (int i = 0; i < nivel; i++) printf("    ");
    
    // Exibe ID e nome do nó
    printf("%d (%s)\n", raiz->resultado.id, raiz->resultado.nome);
    
    // Exibe subárvore esquerda (menor)
    exibir_arvore_vertical(raiz->esquerda, nivel + 1);
}

// Função que exibe árvore na horizontal
void exibir_arvore_horizontal(NoArvoreResultados* raiz, int espaco) {
    // Caso base: nó nulo
    if (!raiz) return;
    
    // Aumenta espaçamento para nível mais profundo
    espaco += 10;
    
    // Exibe subárvore direita primeiro
    exibir_arvore_horizontal(raiz->direita, espaco);
    
    // Exibe espaços e ID do nó
    printf("\n");
    for (int i = 10; i < espaco; i++) printf(" ");
    printf("%d\n", raiz->resultado.id);
    
    // Exibe subárvore esquerda
    exibir_arvore_horizontal(raiz->esquerda, espaco);
}

// Função que exporta resultados para arquivo CSV
int exportar_resultados_csv(ArvoreResultados* arvore, const char* filename) {
    // Verifica parâmetros
    if (!arvore || !filename) return 0;
    
    // Abre arquivo para escrita
    FILE* arquivo = fopen(filename, "w");
    if (!arquivo) return 0;
    
    // Escreve cabeçalho CSV
    fprintf(arquivo, "Tipo;ID;Nome;Votos;Percentual\n");
    
    // Função auxiliar recursiva para exportar nós
    void exportar_no_csv(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Converte tipo para string
        const char* tipo_str;
        switch (no->resultado.tipo) {
            case TIPO_CANDIDATO: tipo_str = "CANDIDATO"; break;
            case TIPO_CAP: tipo_str = "CAP"; break;
            case TIPO_REGIAO: tipo_str = "REGIAO"; break;
            default: tipo_str = "DESCONHECIDO";
        }
        
        // Escreve linha CSV
        fprintf(arquivo, "%s;%d;%s;%d;%.2f\n",
                tipo_str,
                no->resultado.id,
                no->resultado.nome,
                no->resultado.votos,
                no->resultado.percentual);
        
        // Exporta subárvores
        exportar_no_csv(no->esquerda);
        exportar_no_csv(no->direita);
    }
    
    // Inicia exportação da raiz
    exportar_no_csv(arvore->raiz);
    fclose(arquivo);
    
    printf("Resultados exportados para %s\n", filename);
    return 1;
}

// Função que exporta resultados para arquivo JSON
int exportar_resultados_json(ArvoreResultados* arvore, const char* filename) {
    // Verifica parâmetros
    if (!arvore || !filename) return 0;
    
    // Abre arquivo para escrita
    FILE* arquivo = fopen(filename, "w");
    if (!arquivo) return 0;
    
    // Escreve início do JSON
    fprintf(arquivo, "{\n");
    fprintf(arquivo, "  \"tipo\": \"");
    
    // Escreve tipo da árvore
    switch (arvore->tipo_arvore) {
        case TIPO_CANDIDATO: fprintf(arquivo, "candidatos"); break;
        case TIPO_CAP: fprintf(arquivo, "caps"); break;
        case TIPO_REGIAO: fprintf(arquivo, "regioes"); break;
        default: fprintf(arquivo, "desconhecido");
    }
    
    fprintf(arquivo, "\",\n");
    fprintf(arquivo, "  \"total_votos\": %d,\n", arvore->total_votos);
    fprintf(arquivo, "  \"resultados\": [\n");
    
    int primeiro = 1; // Flag para controlar vírgulas
    
    // Função auxiliar recursiva para exportar em JSON
    void exportar_no_json(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Exporta subárvore esquerda
        exportar_no_json(no->esquerda);
        
        // Adiciona vírgula se não for o primeiro elemento
        if (!primeiro) fprintf(arquivo, ",\n");
        primeiro = 0;
        
        // Escreve objeto JSON do nó
        fprintf(arquivo, "    {\n");
        fprintf(arquivo, "      \"id\": %d,\n", no->resultado.id);
        fprintf(arquivo, "      \"nome\": \"%s\",\n", no->resultado.nome);
        fprintf(arquivo, "      \"votos\": %d,\n", no->resultado.votos);
        fprintf(arquivo, "      \"percentual\": %.2f\n", no->resultado.percentual);
        fprintf(arquivo, "    }");
        
        // Exporta subárvore direita
        exportar_no_json(no->direita);
    }
    
    // Inicia exportação da raiz
    exportar_no_json(arvore->raiz);
    
    // Fecha estruturas JSON
    fprintf(arquivo, "\n  ]\n");
    fprintf(arquivo, "}\n");
    
    fclose(arquivo);
    printf("Resultados exportados para %s (JSON)\n", filename);
    return 1;
}

// ================= IMPLEMENTAÇÃO FUNÇÕES DE ORDENAÇÃO =================

// Função que ordena resultados em ordem decrescente
void ordenar_resultados_decrescente(ArvoreResultados* arvore, 
                                   ResultadoVotacao** array, int* indice) {
    // Verifica parâmetros
    if (!arvore || !array || !indice) return;
    
    // Função auxiliar para coletar resultados em array
    void coletar_resultados(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        
        // Coleta em ordem (para manter array inicialmente ordenado por ID)
        coletar_resultados(no->esquerda);
        array[(*indice)++] = &(no->resultado);
        coletar_resultados(no->direita);
    }
    
    // Inicia coleta
    *indice = 0;
    coletar_resultados(arvore->raiz);
    
    // Ordenação por bolha decrescente (por votos)
    for (int i = 0; i < *indice - 1; i++) {
        for (int j = 0; j < *indice - i - 1; j++) {
            if (array[j]->votos < array[j + 1]->votos) {
                // Troca elementos
                ResultadoVotacao* temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

// Função que retorna array de resultados ordenados
ResultadoVotacao** obter_resultados_ordenados(ArvoreResultados* arvore, int ordem) {
    // Verifica se árvore é válida
    if (!arvore) return NULL;
    
    // Conta número de nós
    int n = total_nos_arvore(arvore);
    // Aloca array de ponteiros para resultados
    ResultadoVotacao** array = (ResultadoVotacao**)malloc(n * sizeof(ResultadoVotacao*));
    
    // Verifica alocação
    if (!array) return NULL;
    
    int indice = 0;
    
    // Função auxiliar para coletar resultados em array
    void coletar_em_array(NoArvoreResultados* no) {
        // Caso base: nó nulo
        if (!no) return;
        // Coleta em ordem (esquerda, raiz, direita)
        coletar_em_array(no->esquerda);
        array[indice++] = &(no->resultado);
        coletar_em_array(no->direita);
    }
    
    // Coleta resultados
    coletar_em_array(arvore->raiz);
    
    // Ordena se necessário
    if (ordem == 1) { // Decrescente
        // Bubble sort decrescente
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (array[j]->votos < array[j + 1]->votos) {
                    // Troca elementos
                    ResultadoVotacao* temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }
    
    return array; // Retorna array ordenado
}
