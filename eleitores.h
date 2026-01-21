// Diretiva de pré-processador para evitar inclusões múltiplas
#ifndef ELEITORES_H
#define ELEITORES_H

// Inclui o cabeçalho com as estruturas de dados básicas
#include "estruturas.h"

// Inicialização e destruição
// Protótipo da função que inicializa lista de eleitores
void inicializar_lista_eleitores(ListaEleitores* lista);
// Protótipo da função que libera lista de eleitores
void liberar_lista_eleitores(ListaEleitores* lista);

// Operações básicas de CRUD
// Protótipo da função que cadastra eleitor
int cadastrar_eleitor(ListaEleitores* lista, const char* nome, const char* documento, int id_cap, int prioridade);
// Protótipo da função que remove eleitor por ID
int remover_eleitor_por_id(ListaEleitores* lista, int id);
// Protótipo da função que remove eleitor por documento
int remover_eleitor_por_documento(ListaEleitores* lista, const char* documento);
// Protótipo da função que busca eleitor por ID
Eleitor* buscar_eleitor_por_id(ListaEleitores* lista, int id);
// Protótipo da função que busca eleitor por documento
Eleitor* buscar_eleitor_por_documento(ListaEleitores* lista, const char* documento);

// Listagem e exibição
// Protótipo da função que lista todos os eleitores
void listar_eleitores(ListaEleitores* lista);
// Protótipo da função que lista eleitores por CAP
void listar_eleitores_por_cap(ListaEleitores* lista, int id_cap);
// Protótipo da função que lista eleitores por estado de voto
void listar_eleitores_por_estado_voto(ListaEleitores* lista, int votou);
// Protótipo da função que lista eleitores prioritários
void listar_eleitores_prioritarios(ListaEleitores* lista);
// Protótipo da função que exibe detalhes de um eleitor
void exibir_detalhes_eleitor(Eleitor* eleitor);

// Validações e verificações
// Protótipo da função que verifica se documento existe
int documento_existe(ListaEleitores* lista, const char* documento);
// Protótipo da função que verifica se eleitor pode votar
int eleitor_pode_votar(Eleitor* eleitor);
// Protótipo da função que marca eleitor como votado
int marcar_como_votado(ListaEleitores* lista, int id_eleitor);
// Protótipo da função que verifica se eleitor já votou
int verificar_eleitor_votou(ListaEleitores* lista, int id_eleitor);

// Estatísticas
// Protótipo da função que conta eleitores cadastrados
int contar_eleitores_cadastrados(ListaEleitores* lista);
// Protótipo da função que conta eleitores por CAP
int contar_eleitores_por_cap(ListaEleitores* lista, int id_cap);
// Protótipo da função que conta eleitores que votaram
int contar_eleitores_votaram(ListaEleitores* lista);
// Protótipo da função que conta eleitores que não votaram
int contar_eleitores_nao_votaram(ListaEleitores* lista);
// Protótipo da função que conta eleitores prioritários
int contar_eleitores_prioritarios(ListaEleitores* lista);

// Exportação e importação (opcional para persistência)
// Protótipo da função que exporta eleitores para CSV
int exportar_eleitores_para_csv(ListaEleitores* lista, const char* filename);
// Protótipo da função que importa eleitores de CSV
int importar_eleitores_de_csv(ListaEleitores* lista, const char* filename);

// Funções auxiliares
// Protótipo da função que valida documento
int validar_documento(const char* documento);
// Protótipo da função que gera novo ID
int gerar_novo_id(ListaEleitores* lista);

// Fim da diretiva de pré-processador
#endif
