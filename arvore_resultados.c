#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "arvore_resultados.h"

// Função auxiliar para máximo (APENAS ESTA!)
static int max(int a, int b) {
    return (a > b) ? a : b;
}

ArvoreResultados* criar_arvore_resultados(TipoNoArvore tipo) {
    ArvoreResultados* arvore = (ArvoreResultados*)malloc(sizeof(ArvoreResultados));
    if (arvore) {
        arvore->raiz = NULL;
        arvore->total_votos = 0;
        arvore->total_eleitores = 0;
        arvore->participacao = 0.0;
        arvore->tipo_arvore = tipo;
    }
    return arvore;
}

void destruir_arvore_resultados(ArvoreResultados* arvore) {
    if (!arvore) return;
    
    // Função auxiliar para destruir recursivamente
    void destruir_recursivo(NoArvoreResultados* no) {
        if (!no) return;
        destruir_recursivo(no->esquerda);
        destruir_recursivo(no->direita);
        free(no);
    }
    
    destruir_recursivo(arvore->raiz);
    free(arvore);
}

NoArvoreResultados* criar_no_resultado(ResultadoVotacao resultado) {
    NoArvoreResultados* novo_no = (NoArvoreResultados*)malloc(sizeof(NoArvoreResultados));
    if (novo_no) {
        novo_no->resultado = resultado;
        novo_no->esquerda = NULL;
        novo_no->direita = NULL;
        novo_no->altura = 1; // Altura inicial
    }
    return novo_no;
}

void destruir_no_resultado(NoArvoreResultados* no) {
    if (no) free(no);
}

int inserir_resultado_arvore(ArvoreResultados* arvore, ResultadoVotacao resultado) {
    if (!arvore) return 0;
    
    NoArvoreResultados* novo_no = criar_no_resultado(resultado);
    if (!novo_no) return 0;
    
    arvore->raiz = inserir_no_recursivo(arvore->raiz, novo_no);
    
    // Atualizar estatísticas da árvore
    arvore->total_votos += resultado.votos;
    
    return 1;
}

NoArvoreResultados* inserir_no_recursivo(NoArvoreResultados* raiz, NoArvoreResultados* novo_no) {
    if (!raiz) return novo_no;
    
    // Comparar pelo ID para inserção ordenada
    if (novo_no->resultado.id < raiz->resultado.id) {
        raiz->esquerda = inserir_no_recursivo(raiz->esquerda, novo_no);
    } else if (novo_no->resultado.id > raiz->resultado.id) {
        raiz->direita = inserir_no_recursivo(raiz->direita, novo_no);
    } else {
        // ID já existe, atualizar votos
        raiz->resultado.votos += novo_no->resultado.votos;
        free(novo_no);
        return raiz;
    }
    
    // Atualizar altura
    raiz->altura = 1 + max(altura_no(raiz->esquerda), altura_no(raiz->direita));
    
    // Balancear a árvore (AVL)
    return balancear_no(raiz);
}

ResultadoVotacao* buscar_resultado_por_id(ArvoreResultados* arvore, int id) {
    if (!arvore) return NULL;
    
    NoArvoreResultados* no = buscar_no_recursivo(arvore->raiz, id);
    return no ? &(no->resultado) : NULL;
}

NoArvoreResultados* buscar_no_recursivo(NoArvoreResultados* raiz, int id) {
    if (!raiz || raiz->resultado.id == id) {
        return raiz;
    }
    
    if (id < raiz->resultado.id) {
        return buscar_no_recursivo(raiz->esquerda, id);
    } else {
        return buscar_no_recursivo(raiz->direita, id);
    }
}

ResultadoVotacao* buscar_resultado_por_nome(ArvoreResultados* arvore, const char* nome) {
    if (!arvore || !nome) return NULL;
    
    // Função auxiliar para busca por nome (busca linear)
    ResultadoVotacao* buscar_por_nome_recursivo(NoArvoreResultados* raiz, const char* nome) {
        if (!raiz) return NULL;
        
        if (strcmp(raiz->resultado.nome, nome) == 0) {
            return &(raiz->resultado);
        }
        
        ResultadoVotacao* encontrado = buscar_por_nome_recursivo(raiz->esquerda, nome);
        if (encontrado) return encontrado;
        
        return buscar_por_nome_recursivo(raiz->direita, nome);
    }
    
    return buscar_por_nome_recursivo(arvore->raiz, nome);
}

int remover_resultado_arvore(ArvoreResultados* arvore, int id) {
    if (!arvore) return 0;
    
    arvore->raiz = remover_no_recursivo(arvore->raiz, id);
    return 1;
}

NoArvoreResultados* remover_no_recursivo(NoArvoreResultados* raiz, int id) {
    if (!raiz) return NULL;
    
    if (id < raiz->resultado.id) {
        raiz->esquerda = remover_no_recursivo(raiz->esquerda, id);
    } else if (id > raiz->resultado.id) {
        raiz->direita = remover_no_recursivo(raiz->direita, id);
    } else {
        // Nó encontrado
        if (!raiz->esquerda || !raiz->direita) {
            // Um ou nenhum filho
            NoArvoreResultados* temp = raiz->esquerda ? raiz->esquerda : raiz->direita;
            
            if (!temp) {
                // Nenhum filho
                temp = raiz;
                raiz = NULL;
            } else {
                // Um filho
                *raiz = *temp;
            }
            
            free(temp);
        } else {
            // Dois filhos
            NoArvoreResultados* temp = encontrar_minimo(raiz->direita);
            raiz->resultado = temp->resultado;
            raiz->direita = remover_no_recursivo(raiz->direita, temp->resultado.id);
        }
    }
    
    if (!raiz) return NULL;
    
    // Atualizar altura e balancear
    raiz->altura = 1 + max(altura_no(raiz->esquerda), altura_no(raiz->direita));
    return balancear_no(raiz);
}

NoArvoreResultados* encontrar_minimo(NoArvoreResultados* raiz) {
    NoArvoreResultados* atual = raiz;
    while (atual->esquerda != NULL) {
        atual = atual->esquerda;
    }
    return atual;
}

// ================= IMPLEMENTAÇÃO PERCURSOS =================

void percurso_em_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*)) {
    if (!raiz) return;
    
    percurso_em_ordem(raiz->esquerda, callback);
    if (callback) callback(&raiz->resultado);
    percurso_em_ordem(raiz->direita, callback);
}

void percurso_pre_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*)) {
    if (!raiz) return;
    
    if (callback) callback(&raiz->resultado);
    percurso_pre_ordem(raiz->esquerda, callback);
    percurso_pre_ordem(raiz->direita, callback);
}

void percurso_pos_ordem(NoArvoreResultados* raiz, void (*callback)(ResultadoVotacao*)) {
    if (!raiz) return;
    
    percurso_pos_ordem(raiz->esquerda, callback);
    percurso_pos_ordem(raiz->direita, callback);
    if (callback) callback(&raiz->resultado);
}

void percurso_em_ordem_filtrado(NoArvoreResultados* raiz, TipoNoArvore tipo, 
                               void (*callback)(ResultadoVotacao*)) {
    if (!raiz) return;
    
    percurso_em_ordem_filtrado(raiz->esquerda, tipo, callback);
    if (raiz->resultado.tipo == tipo && callback) {
        callback(&raiz->resultado);
    }
    percurso_em_ordem_filtrado(raiz->direita, tipo, callback);
}

// ================= IMPLEMENTAÇÃO ÁRVORE AVL =================

int altura_no(NoArvoreResultados* no) {
    return no ? no->altura : 0;
}



int fator_balanceamento(NoArvoreResultados* no) {
    if (!no) return 0;
    return altura_no(no->esquerda) - altura_no(no->direita);
}

NoArvoreResultados* rotacionar_direita(NoArvoreResultados* y) {
    NoArvoreResultados* x = y->esquerda;
    NoArvoreResultados* T2 = x->direita;
    
    x->direita = y;
    y->esquerda = T2;
    
    // Atualizar alturas
    y->altura = max(altura_no(y->esquerda), altura_no(y->direita)) + 1;
    x->altura = max(altura_no(x->esquerda), altura_no(x->direita)) + 1;
    
    return x;
}

NoArvoreResultados* rotacionar_esquerda(NoArvoreResultados* x) {
    NoArvoreResultados* y = x->direita;
    NoArvoreResultados* T2 = y->esquerda;
    
    y->esquerda = x;
    x->direita = T2;
    
    // Atualizar alturas
    x->altura = max(altura_no(x->esquerda), altura_no(x->direita)) + 1;
    y->altura = max(altura_no(y->esquerda), altura_no(y->direita)) + 1;
    
    return y;
}

NoArvoreResultados* balancear_no(NoArvoreResultados* no) {
    if (!no) return no;
    
    int balance = fator_balanceamento(no);
    
    // Caso Esquerda-Esquerda
    if (balance > 1 && fator_balanceamento(no->esquerda) >= 0) {
        return rotacionar_direita(no);
    }
    
    // Caso Direita-Direita
    if (balance < -1 && fator_balanceamento(no->direita) <= 0) {
        return rotacionar_esquerda(no);
    }
    
    // Caso Esquerda-Direita
    if (balance > 1 && fator_balanceamento(no->esquerda) < 0) {
        no->esquerda = rotacionar_esquerda(no->esquerda);
        return rotacionar_direita(no);
    }
    
    // Caso Direita-Esquerda
    if (balance < -1 && fator_balanceamento(no->direita) > 0) {
        no->direita = rotacionar_direita(no->direita);
        return rotacionar_esquerda(no);
    }
    
    return no;
}

// ================= IMPLEMENTAÇÃO OPERAÇÕES ESPECÍFICAS =================

ResultadoVotacao criar_resultado_candidato(Candidato* candidato, int total_votos) {
    ResultadoVotacao resultado;
    
    resultado.id = candidato->numero;
    strncpy(resultado.nome, candidato->nome, MAX_NOME - 1);
    resultado.tipo = TIPO_CANDIDATO;
    resultado.votos = candidato->votos;
    resultado.votos_validos = candidato->votos;
    resultado.votos_brancos = 0;
    resultado.votos_nulos = 0;
    
    if (total_votos > 0) {
        resultado.percentual = (float)candidato->votos / total_votos * 100;
    } else {
        resultado.percentual = 0.0;
    }
    
    return resultado;
}

ResultadoVotacao criar_resultado_cap(CAP* cap, int total_votos) {
    ResultadoVotacao resultado;
    
    resultado.id = cap->id;
    strncpy(resultado.nome, cap->localizacao, MAX_NOME - 1);
    resultado.tipo = TIPO_CAP;
    resultado.votos = cap->eleitores_atendidos;
    resultado.votos_validos = cap->eleitores_atendidos;
    resultado.votos_brancos = 0;
    resultado.votos_nulos = 0;
    
    if (total_votos > 0) {
        resultado.percentual = (float)cap->eleitores_atendidos / total_votos * 100;
    } else {
        resultado.percentual = 0.0;
    }
    
    return resultado;
}

ResultadoVotacao criar_resultado_regiao(const char* regiao, int votos_regiao, int total_votos) {
    ResultadoVotacao resultado;
    
    resultado.id = 0; // Gerar ID baseado no hash da região
    for (int i = 0; regiao[i] != '\0'; i++) {
        resultado.id += regiao[i];
    }
    
    strncpy(resultado.nome, regiao, MAX_NOME - 1);
    resultado.tipo = TIPO_REGIAO;
    resultado.votos = votos_regiao;
    resultado.votos_validos = votos_regiao;
    resultado.votos_brancos = 0;
    resultado.votos_nulos = 0;
    
    if (total_votos > 0) {
        resultado.percentual = (float)votos_regiao / total_votos * 100;
    } else {
        resultado.percentual = 0.0;
    }
    
    return resultado;
}

int atualizar_votos_resultado(ArvoreResultados* arvore, int id, int votos_adicionais) {
    if (!arvore) return 0;
    
    ResultadoVotacao* resultado = buscar_resultado_por_id(arvore, id);
    if (!resultado) return 0;
    
    resultado->votos += votos_adicionais;
    arvore->total_votos += votos_adicionais;
    
    return 1;
}

int atualizar_percentuais_arvore(ArvoreResultados* arvore) {
    if (!arvore || arvore->total_votos == 0) return 0;
    
    void atualizar_percentual_no(NoArvoreResultados* no, int total_votos) {
        if (!no) return;
        
        no->resultado.percentual = (float)no->resultado.votos / total_votos * 100;
        atualizar_percentual_no(no->esquerda, total_votos);
        atualizar_percentual_no(no->direita, total_votos);
    }
    
    atualizar_percentual_no(arvore->raiz, arvore->total_votos);
    return 1;
}

// ================= IMPLEMENTAÇÃO ESTATÍSTICAS E CONSULTAS =================

int total_nos_arvore(ArvoreResultados* arvore) {
    if (!arvore) return 0;
    
    int contar_nos_recursivo(NoArvoreResultados* raiz) {
        if (!raiz) return 0;
        return 1 + contar_nos_recursivo(raiz->esquerda) + contar_nos_recursivo(raiz->direita);
    }
    
    return contar_nos_recursivo(arvore->raiz);
}

int altura_arvore(ArvoreResultados* arvore) {
    if (!arvore || !arvore->raiz) return 0;
    return arvore->raiz->altura;
}

int arvore_vazia(ArvoreResultados* arvore) {
    return (!arvore || !arvore->raiz);
}

ResultadoVotacao* candidato_mais_votado_arvore(ArvoreResultados* arvore) {
    if (!arvore || arvore->tipo_arvore != TIPO_CANDIDATO) return NULL;
    
    NoArvoreResultados* atual = arvore->raiz;
    NoArvoreResultados* mais_votado = NULL;
    
    // Percorrer a árvore para encontrar o máximo
    void encontrar_maximo_recursivo(NoArvoreResultados* no) {
        if (!no) return;
        
        if (!mais_votado || no->resultado.votos > mais_votado->resultado.votos) {
            mais_votado = no;
        }
        
        encontrar_maximo_recursivo(no->esquerda);
        encontrar_maximo_recursivo(no->direita);
    }
    
    encontrar_maximo_recursivo(atual);
    return mais_votado ? &(mais_votado->resultado) : NULL;
}

ResultadoVotacao* candidato_menos_votado_arvore(ArvoreResultados* arvore) {
    if (!arvore || arvore->tipo_arvore != TIPO_CANDIDATO) return NULL;
    
    NoArvoreResultados* atual = arvore->raiz;
    NoArvoreResultados* menos_votado = NULL;
    
    // Percorrer a árvore para encontrar o mínimo
    void encontrar_minimo_recursivo(NoArvoreResultados* no) {
        if (!no) return;
        
        if (!menos_votado || no->resultado.votos < menos_votado->resultado.votos) {
            menos_votado = no;
        }
        
        encontrar_minimo_recursivo(no->esquerda);
        encontrar_minimo_recursivo(no->direita);
    }
    
    encontrar_minimo_recursivo(atual);
    return menos_votado ? &(menos_votado->resultado) : NULL;
}

ResultadoVotacao* cap_maior_participacao(ArvoreResultados* arvore) {
    if (!arvore || arvore->tipo_arvore != TIPO_CAP) return NULL;
    
    // Implementação similar à candidato_mais_votado_arvore
    return candidato_mais_votado_arvore(arvore); // Reutiliza a mesma lógica
}

ResultadoVotacao* regiao_maior_participacao(ArvoreResultados* arvore) {
    if (!arvore || arvore->tipo_arvore != TIPO_REGIAO) return NULL;
    
    // Implementação similar à candidato_mais_votado_arvore
    return candidato_mais_votado_arvore(arvore); // Reutiliza a mesma lógica
}

void candidatos_acima_de_limite(ArvoreResultados* arvore, int limite_votos) {
    if (!arvore || arvore->tipo_arvore != TIPO_CANDIDATO) return;
    
    printf("\n=== CANDIDATOS COM MAIS DE %d VOTOS ===\n", limite_votos);
    
    void verificar_limite_recursivo(NoArvoreResultados* no, int limite) {
        if (!no) return;
        
        verificar_limite_recursivo(no->esquerda, limite);
        
        if (no->resultado.votos > limite) {
            exibir_resultado_formatado(&no->resultado);
        }
        
        verificar_limite_recursivo(no->direita, limite);
    }
    
    verificar_limite_recursivo(arvore->raiz, limite_votos);
}

void caps_acima_de_limite(ArvoreResultados* arvore, int limite_votos) {
    if (!arvore || arvore->tipo_arvore != TIPO_CAP) return;
    
    printf("\n=== CAPs COM MAIS DE %d VOTOS ===\n", limite_votos);
    candidatos_acima_de_limite(arvore, limite_votos); // Reutiliza mesma lógica
}

// ================= IMPLEMENTAÇÃO VISUALIZAÇÃO E EXPORTAÇÃO =================

void exibir_resultado_formatado(ResultadoVotacao* resultado) {
    if (!resultado) {
        printf("Resultado invalido!\n");
        return;
    }
    
    const char* tipo_str;
    switch (resultado->tipo) {
        case TIPO_CANDIDATO: tipo_str = "CANDIDATO"; break;
        case TIPO_CAP: tipo_str = "CAP"; break;
        case TIPO_REGIAO: tipo_str = "REGIAO"; break;
        default: tipo_str = "DESCONHECIDO";
    }
    
    printf("+-----------------------------------------------------+\n");
    printf("¦ %-20s: %-25s ¦\n", "Tipo", tipo_str);
    printf("¦ %-20s: %-25s ¦\n", "Nome", resultado->nome);
    printf("¦ %-20s: %-25d ¦\n", "ID", resultado->id);
    printf("¦ %-20s: %-25d ¦\n", "Votos", resultado->votos);
    printf("¦ %-20s: %-24.2f%% ¦\n", "Percentual", resultado->percentual);
    printf("+-----------------------------------------------------+\n");
}

void exibir_arvore_resultados(ArvoreResultados* arvore) {
    if (!arvore) {
        printf("Arvore invalida!\n");
        return;
    }
    
    printf("\n=== ARVORE DE RESULTADOS ===\n");
    printf("Tipo: ");
    switch (arvore->tipo_arvore) {
        case TIPO_CANDIDATO: printf("CANDIDATOS\n"); break;
        case TIPO_CAP: printf("CAPs\n"); break;
        case TIPO_REGIAO: printf("REGIOES\n"); break;
        default: printf("DESCONHECIDO\n");
    }
    
    printf("Total de nos: %d\n", total_nos_arvore(arvore));
    printf("Altura: %d\n", altura_arvore(arvore));
    printf("Total de votos: %d\n\n", arvore->total_votos);
    
    printf("Resultados (em ordem):\n");
    percurso_em_ordem(arvore->raiz, exibir_resultado_formatado);
}

void exibir_arvore_vertical(NoArvoreResultados* raiz, int nivel) {
    if (!raiz) return;
    
    exibir_arvore_vertical(raiz->direita, nivel + 1);
    
    for (int i = 0; i < nivel; i++) printf("    ");
    
    printf("%d (%s)\n", raiz->resultado.id, raiz->resultado.nome);
    
    exibir_arvore_vertical(raiz->esquerda, nivel + 1);
}

void exibir_arvore_horizontal(NoArvoreResultados* raiz, int espaco) {
    if (!raiz) return;
    
    espaco += 10;
    
    exibir_arvore_horizontal(raiz->direita, espaco);
    
    printf("\n");
    for (int i = 10; i < espaco; i++) printf(" ");
    printf("%d\n", raiz->resultado.id);
    
    exibir_arvore_horizontal(raiz->esquerda, espaco);
}

int exportar_resultados_csv(ArvoreResultados* arvore, const char* filename) {
    if (!arvore || !filename) return 0;
    
    FILE* arquivo = fopen(filename, "w");
    if (!arquivo) return 0;
    
    // Cabeçalho
    fprintf(arquivo, "Tipo;ID;Nome;Votos;Percentual\n");
    
    // Dados
    void exportar_no_csv(NoArvoreResultados* no) {
        if (!no) return;
        
        const char* tipo_str;
        switch (no->resultado.tipo) {
            case TIPO_CANDIDATO: tipo_str = "CANDIDATO"; break;
            case TIPO_CAP: tipo_str = "CAP"; break;
            case TIPO_REGIAO: tipo_str = "REGIAO"; break;
            default: tipo_str = "DESCONHECIDO";
        }
        
        fprintf(arquivo, "%s;%d;%s;%d;%.2f\n",
                tipo_str,
                no->resultado.id,
                no->resultado.nome,
                no->resultado.votos,
                no->resultado.percentual);
        
        exportar_no_csv(no->esquerda);
        exportar_no_csv(no->direita);
    }
    
    exportar_no_csv(arvore->raiz);
    fclose(arquivo);
    
    printf("Resultados exportados para %s\n", filename);
    return 1;
}

int exportar_resultados_json(ArvoreResultados* arvore, const char* filename) {
    if (!arvore || !filename) return 0;
    
    FILE* arquivo = fopen(filename, "w");
    if (!arquivo) return 0;
    
    fprintf(arquivo, "{\n");
    fprintf(arquivo, "  \"tipo\": \"");
    
    switch (arvore->tipo_arvore) {
        case TIPO_CANDIDATO: fprintf(arquivo, "candidatos"); break;
        case TIPO_CAP: fprintf(arquivo, "caps"); break;
        case TIPO_REGIAO: fprintf(arquivo, "regioes"); break;
        default: fprintf(arquivo, "desconhecido");
    }
    
    fprintf(arquivo, "\",\n");
    fprintf(arquivo, "  \"total_votos\": %d,\n", arvore->total_votos);
    fprintf(arquivo, "  \"resultados\": [\n");
    
    int primeiro = 1;
    void exportar_no_json(NoArvoreResultados* no) {
        if (!no) return;
        
        exportar_no_json(no->esquerda);
        
        if (!primeiro) fprintf(arquivo, ",\n");
        primeiro = 0;
        
        fprintf(arquivo, "    {\n");
        fprintf(arquivo, "      \"id\": %d,\n", no->resultado.id);
        fprintf(arquivo, "      \"nome\": \"%s\",\n", no->resultado.nome);
        fprintf(arquivo, "      \"votos\": %d,\n", no->resultado.votos);
        fprintf(arquivo, "      \"percentual\": %.2f\n", no->resultado.percentual);
        fprintf(arquivo, "    }");
        
        exportar_no_json(no->direita);
    }
    
    exportar_no_json(arvore->raiz);
    
    fprintf(arquivo, "\n  ]\n");
    fprintf(arquivo, "}\n");
    
    fclose(arquivo);
    printf("Resultados exportados para %s (JSON)\n", filename);
    return 1;
}

// ================= IMPLEMENTAÇÃO FUNÇÕES DE ORDENAÇÃO =================

void ordenar_resultados_decrescente(ArvoreResultados* arvore, 
                                   ResultadoVotacao** array, int* indice) {
    if (!arvore || !array || !indice) return;
    
    void coletar_resultados(NoArvoreResultados* no) {
        if (!no) return;
        
        coletar_resultados(no->esquerda);
        array[(*indice)++] = &(no->resultado);
        coletar_resultados(no->direita);
    }
    
    *indice = 0;
    coletar_resultados(arvore->raiz);
    
    // Ordenação por bolha decrescente (por votos)
    for (int i = 0; i < *indice - 1; i++) {
        for (int j = 0; j < *indice - i - 1; j++) {
            if (array[j]->votos < array[j + 1]->votos) {
                ResultadoVotacao* temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

ResultadoVotacao** obter_resultados_ordenados(ArvoreResultados* arvore, int ordem) {
    if (!arvore) return NULL;
    
    int n = total_nos_arvore(arvore);
    ResultadoVotacao** array = (ResultadoVotacao**)malloc(n * sizeof(ResultadoVotacao*));
    
    if (!array) return NULL;
    
    int indice = 0;
    
    // Coletar resultados em array
    void coletar_em_array(NoArvoreResultados* no) {
        if (!no) return;
        coletar_em_array(no->esquerda);
        array[indice++] = &(no->resultado);
        coletar_em_array(no->direita);
    }
    
    coletar_em_array(arvore->raiz);
    
    // Ordenar
    if (ordem == 1) { // Decrescente
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (array[j]->votos < array[j + 1]->votos) {
                    ResultadoVotacao* temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }
    
    return array;
}
