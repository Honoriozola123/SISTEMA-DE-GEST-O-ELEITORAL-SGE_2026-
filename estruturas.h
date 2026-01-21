// Diretiva de pré-processador para evitar inclusões múltiplas
#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Inclui biblioteca para tipo bool
#include <stdbool.h>
// Inclui biblioteca para manipulação de tempo
#include <time.h>

// Definição de constantes para tamanhos máximos
#define MAX_NOME 100      // Tamanho máximo para nomes
#define MAX_PARTIDO 50    // Tamanho máximo para partidos
#define MAX_LOCAL 100     // Tamanho máximo para localizações
#define MAX_DOC 20        // Tamanho máximo para documentos
#define MAX_REGIAO 50     // Tamanho máximo para regiões

// Tipos de prioridade para filas
typedef enum {
    PRIORIDADE_NORMAL = 0,    // Prioridade normal
    PRIORIDADE_ALTA = 1,      // Idosos (65+ anos)
    PRIORIDADE_URGENTE = 2    // Grávidas, deficientes físicos graves
} TipoPrioridade;

// Tipos de nós para Árvores binárias
typedef enum {
    TIPO_CANDIDATO,   // Tipo para nós de candidatos
    TIPO_CAP,         // Tipo para nós de CAPs
    TIPO_REGIAO,      // Tipo para nós de regiões
    TIPO_RESULTADO    // Tipo para nós de resultados genéricos
} TipoNoArvore;

// Estrutura para Resultado de Votação
typedef struct ResultadoVotacao {
    int id;                 // ID do candidato, CAP ou região
    char nome[MAX_NOME];    // Nome do candidato, CAP ou região
    TipoNoArvore tipo;      // Tipo do resultado
    int votos;              // Total de votos
    int votos_validos;      // Votos válidos
    int votos_brancos;      // Votos em branco (se aplicável)
    int votos_nulos;        // Votos nulos (se aplicável)
    float percentual;       // Percentual em relação ao total
} ResultadoVotacao;

// Nó da Árvore Binária para Resultados
typedef struct NoArvoreResultados {
    ResultadoVotacao resultado;           // Dados do resultado
    struct NoArvoreResultados* esquerda;  // Ponteiro para filho esquerdo
    struct NoArvoreResultados* direita;   // Ponteiro para filho direito
    int altura; // Para árvore AVL (balanceamento)
} NoArvoreResultados;

// Árvore Binária de Resultados
typedef struct ArvoreResultados {
    NoArvoreResultados* raiz;     // Ponteiro para raiz da árvore
    int total_votos;              // Total de votos na árvore
    int total_eleitores;          // Total de eleitores
    float participacao;           // Percentual de participação
    TipoNoArvore tipo_arvore;     // Tipo de organização da árvore
} ArvoreResultados;

// Estrutura para Eleitor
typedef struct Eleitor {
    int id;                     // ID único do eleitor
    char nome[MAX_NOME];        // Nome completo
    char documento[MAX_DOC];    // Documento (CPF, RG, etc.)
    int id_cap;                 // ID do CAP associado
    int votou;                  // 0 = não votou, 1 = votou
    int idade;                  // Para determinar prioridade
    TipoPrioridade prioridade;  // Tipo de prioridade
    struct Eleitor* proximo;    // Ponteiro para próximo eleitor (lista ligada)
} Eleitor;

// Estrutura para Nó da Fila
typedef struct NoFila {
    Eleitor* eleitor;           // Ponteiro para eleitor
    struct NoFila* proximo;     // Ponteiro para próximo nó
} NoFila;

// Estrutura para Fila (FIFO)
typedef struct Fila {
    NoFila* frente;             // Ponteiro para frente da fila
    NoFila* tras;               // Ponteiro para trás da fila
    int tamanho;                // Número de elementos na fila
} Fila;

// Estrutura para Nó da Pilha
typedef struct NoPilha {
    void* dado;                 // Dado genérico (pode ser qualquer tipo)
    struct NoPilha* abaixo;     // Ponteiro para elemento abaixo
} NoPilha;

// Estrutura para Pilha (LIFO)
typedef struct Pilha {
    NoPilha* topo;              // Ponteiro para topo da pilha
    int tamanho;                // Número de elementos na pilha
} Pilha;

// Estrutura para Voto
typedef struct Voto {
    int id_eleitor;             // ID do eleitor que votou
    int numero_candidato;       // Número do candidato votado
    time_t timestamp;           // Data/hora do voto
    int id_cap;                 // ID do CAP onde foi votado
    int numero_urna;            // Número da urna utilizada
} Voto;

// Estrutura para Operação (log do sistema)
typedef struct Operacao {
    char tipo[50];              // Tipo da operação
    char descricao[200];        // Descrição detalhada
    time_t timestamp;           // Data/hora da operação
    int usuario_id;             // ID do usuário que realizou
} Operacao;

// Estrutura para Urna Eletrônica
typedef struct Urna {
    int numero;                 // Número da urna
    int cap_id;                 // ID do CAP onde está a urna
    int votos_registrados;      // Número de votos registrados
    bool ativa;                 // Status da urna (ativa/inativa)
    Pilha* historico_votos;     // Pilha com histórico de votos
} Urna;

// Estrutura para Candidato
typedef struct Candidato {
    int numero;                 // Número do candidato
    char nome[MAX_NOME];        // Nome completo
    char partido[MAX_PARTIDO];  // Partido político
    char regiao[MAX_REGIAO];    // Região de candidatura
    int votos;                  // Total de votos recebidos
} Candidato;

// Estrutura para CAP (Centro de Apuração de Votos)
typedef struct CAP {
    int id;                     // ID único do CAP
    char localizacao[MAX_LOCAL];// Endereço/localização
    char regiao[MAX_REGIAO];    // Região onde está localizado
    int num_urnas;              // Número de urnas disponíveis
    int max_eleitores_por_urna; // Capacidade máxima por urna
    
    // Filas de eleitores
    Fila* fila_normal;          // Fila de eleitores normais
    Fila* fila_prioritaria;     // Fila de eleitores prioritários
    
    // Array de urnas
    Urna** urnas;               // Array de ponteiros para urnas
    
    // Contadores
    int eleitores_atendidos;    // Eleitores já atendidos
    int eleitores_na_fila;      // Eleitores esperando na fila
    
    // Resultados deste CAP
    ArvoreResultados* resultados_cap; // Árvore com resultados do CAP
} CAP;

// Estruturas para listas sequenciais (arrays dinâmicos)

// Lista de Candidatos
typedef struct ListaCandidatos {
    Candidato* candidatos;      // Array de candidatos
    int tamanho;                // Número atual de candidatos
    int capacidade;             // Capacidade máxima do array
} ListaCandidatos;

// Lista de CAPs
typedef struct ListaCAPs {
    CAP* caps;                  // Array de CAPs
    int tamanho;                // Número atual de CAPs
    int capacidade;             // Capacidade máxima do array
} ListaCAPs;

// Estrutura para Nó da Lista Ligada de Eleitores
typedef struct NoEleitor {
    Eleitor eleitor;            // Dados do eleitor
    struct NoEleitor* proximo;  // Ponteiro para próximo nó
} NoEleitor;

// Estrutura para Lista Ligada de Eleitores
typedef struct ListaEleitores {
    NoEleitor* inicio;          // Ponteiro para primeiro eleitor
    int tamanho;                // Número de eleitores
    int ultimo_id;              // Último ID gerado
} ListaEleitores;

// Estrutura para Sistema Completo
typedef struct SistemaEleitoral {
    ListaCandidatos candidatos;         // Lista de candidatos
    ListaCAPs caps;                     // Lista de CAPs
    ListaEleitores eleitores;           // Lista de eleitores
    ArvoreResultados* arvore_candidatos; // Árvore de resultados de candidatos
    ArvoreResultados* arvore_caps;      // Árvore de resultados de CAPs
    ArvoreResultados* arvore_regioes;   // Árvore de resultados por região
    Pilha* historico_operacoes;         // Pilha com histórico de operações
    bool votacao_ativa;                 // Status da votação
    int usuario_logado;                 // ID do usuário logado
} SistemaEleitoral;

// Fim da diretiva de pré-processador
#endif
