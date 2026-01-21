// Inclusão das bibliotecas necessárias
#include <stdio.h>   // Para funções de entrada/saída
#include <stdlib.h>  // Para alocação de memória
#include <string.h>  // Para manipulação de strings
#include <ctype.h>   // Para funções de caracteres (isdigit)
#include "eleitores.h" // Cabeçalho das funções de eleitores

// Função para inicializar a lista de eleitores
void inicializar_lista_eleitores(ListaEleitores* lista) {
    lista->inicio = NULL; // Inicializa ponteiro para início como NULL
    lista->tamanho = 0;   // Inicializa tamanho como 0
    lista->ultimo_id = 1000; // Começar IDs a partir de 1000
}

// Função para liberar toda a memória da lista de eleitores
void liberar_lista_eleitores(ListaEleitores* lista) {
    NoEleitor* atual = lista->inicio;
    NoEleitor* proximo;
    
    // Percorre toda a lista liberando cada nó
    while (atual != NULL) {
        proximo = atual->proximo; // Salva referência para próximo
        free(atual); // Libera nó atual
        atual = proximo; // Avança para próximo
    }
    
    // Reinicializa lista
    lista->inicio = NULL;
    lista->tamanho = 0;
    lista->ultimo_id = 1000;
}

// Função para gerar um novo ID único
int gerar_novo_id(ListaEleitores* lista) {
    return ++(lista->ultimo_id); // Incrementa e retorna último ID
}

// Função para validar formato do documento (exemplo básico)
int validar_documento(const char* documento) {
    // Verifica se documento é NULL ou vazio
    if (documento == NULL || strlen(documento) == 0) {
        return 0; // Inválido
    }
    
    // Verificar se tem pelo menos 5 caracteres
    if (strlen(documento) < 5) {
        return 0; // Inválido
    }
    
    // Verificar se contém apenas dígitos
    for (int i = 0; documento[i] != '\0'; i++) {
        if (!isdigit(documento[i])) {
            return 0; // Inválido
        }
    }
    
    return 1; // Válido
}

// Verificar se documento já existe na lista
int documento_existe(ListaEleitores* lista, const char* documento) {
    NoEleitor* atual = lista->inicio;
    
    // Percorre lista procurando documento
    while (atual != NULL) {
        if (strcmp(atual->eleitor.documento, documento) == 0) {
            return 1; // Documento existe
        }
        atual = atual->proximo;
    }
    
    return 0; // Documento não existe
}

// Cadastrar novo eleitor
int cadastrar_eleitor(ListaEleitores* lista, const char* nome, const char* documento, int id_cap, int prioridade) {
    // Validações
    if (nome == NULL || strlen(nome) == 0) {
        printf("Erro: Nome do eleitor invalido!\n");
        return 0;
    }
    
    if (!validar_documento(documento)) {
        printf("Erro: Documento invalido! Deve conter apenas digitos e ter pelo menos 5 caracteres.\n");
        return 0;
    }
    
    if (documento_existe(lista, documento)) {
        printf("Erro: Eleitor com documento %s ja cadastrado!\n", documento);
        return 0;
    }
    
    if (id_cap <= 0) {
        printf("Erro: ID do CAP invalido!\n");
        return 0;
    }
    
    if (prioridade != 0 && prioridade != 1) {
        printf("Erro: Prioridade deve ser 0 (normal) ou 1 (prioritario)!\n");
        return 0;
    }
    
    // Criar novo nó
    NoEleitor* novo_no = (NoEleitor*)malloc(sizeof(NoEleitor));
    if (novo_no == NULL) {
        printf("Erro: Falha na alocacao de memoria!\n");
        return 0;
    }
    
    // Preencher dados do eleitor
    novo_no->eleitor.id = gerar_novo_id(lista);
    strncpy(novo_no->eleitor.nome, nome, MAX_NOME - 1);
    novo_no->eleitor.nome[MAX_NOME - 1] = '\0'; // Garante terminação
    
    strncpy(novo_no->eleitor.documento, documento, MAX_DOC - 1);
    novo_no->eleitor.documento[MAX_DOC - 1] = '\0'; // Garante terminação
    
    novo_no->eleitor.id_cap = id_cap;
    novo_no->eleitor.votou = 0; // Não votou ainda
    novo_no->eleitor.prioridade = prioridade;
    
    // Inserir no início da lista (mais eficiente)
    novo_no->proximo = lista->inicio;
    lista->inicio = novo_no;
    lista->tamanho++;
    
    printf("Eleitor cadastrado com sucesso!\n");
    printf("ID gerado: %d\n", novo_no->eleitor.id);
    
    return 1;
}

// Buscar eleitor por ID
Eleitor* buscar_eleitor_por_id(ListaEleitores* lista, int id) {
    NoEleitor* atual = lista->inicio;
    
    // Percorre lista procurando ID
    while (atual != NULL) {
        if (atual->eleitor.id == id) {
            return &(atual->eleitor); // Retorna ponteiro para eleitor
        }
        atual = atual->proximo;
    }
    
    return NULL; // Não encontrado
}

// Buscar eleitor por documento
Eleitor* buscar_eleitor_por_documento(ListaEleitores* lista, const char* documento) {
    NoEleitor* atual = lista->inicio;
    
    // Percorre lista procurando documento
    while (atual != NULL) {
        if (strcmp(atual->eleitor.documento, documento) == 0) {
            return &(atual->eleitor); // Retorna ponteiro para eleitor
        }
        atual = atual->proximo;
    }
    
    return NULL; // Não encontrado
}

// Remover eleitor por ID
int remover_eleitor_por_id(ListaEleitores* lista, int id) {
    NoEleitor* atual = lista->inicio;
    NoEleitor* anterior = NULL;
    
    // Percorre lista procurando eleitor
    while (atual != NULL) {
        if (atual->eleitor.id == id) {
            // Verificar se já votou
            if (atual->eleitor.votou) {
                printf("Atencao: Eleitor ja votou! Remocao nao permitida apos voto.\n");
                return 0;
            }
            
            // Remover o nó
            if (anterior == NULL) {
                // Remover primeiro elemento
                lista->inicio = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            
            free(atual);
            lista->tamanho--;
            
            printf("Eleitor com ID %d removido com sucesso!\n", id);
            return 1;
        }
        
        anterior = atual;
        atual = atual->proximo;
    }
    
    printf("Eleitor com ID %d nao encontrado!\n", id);
    return 0;
}

// Remover eleitor por documento
int remover_eleitor_por_documento(ListaEleitores* lista, const char* documento) {
    // Busca eleitor por documento primeiro
    Eleitor* eleitor = buscar_eleitor_por_documento(lista, documento);
    
    if (eleitor != NULL) {
        // Se encontrou, remove por ID
        return remover_eleitor_por_id(lista, eleitor->id);
    }
    
    printf("Eleitor com documento %s nao encontrado!\n", documento);
    return 0;
}

// Listar todos os eleitores
void listar_eleitores(ListaEleitores* lista) {
    printf("\n=== LISTA DE ELEITORES ===\n");
    printf("Total cadastrados: %d\n\n", lista->tamanho);
    
    if (lista->tamanho == 0) {
        printf("Nenhum eleitor cadastrado.\n");
        return;
    }
    
    NoEleitor* atual = lista->inicio;
    int contador = 1;
    
    // Percorre e exibe todos os eleitores
    while (atual != NULL) {
        printf("Eleitor #%d:\n", contador++);
        exibir_detalhes_eleitor(&(atual->eleitor));
        printf("------------------------\n");
        atual = atual->proximo;
    }
}

// Listar eleitores por CAP
void listar_eleitores_por_cap(ListaEleitores* lista, int id_cap) {
    printf("\n=== ELEITORES DO CAP %d ===\n", id_cap);
    
    NoEleitor* atual = lista->inicio;
    int encontrados = 0;
    
    // Percorre lista procurando eleitores do CAP
    while (atual != NULL) {
        if (atual->eleitor.id_cap == id_cap) {
            printf("ID: %d | Nome: %s | Doc: %s | Votou: %s | Prioridade: %s\n",
                   atual->eleitor.id,
                   atual->eleitor.nome,
                   atual->eleitor.documento,
                   atual->eleitor.votou ? "Sim" : "Nao",
                   atual->eleitor.prioridade ? "Sim" : "Nao");
            encontrados++;
        }
        atual = atual->proximo;
    }
    
    if (encontrados == 0) {
        printf("Nenhum eleitor encontrado para este CAP.\n");
    } else {
        printf("Total: %d eleitor(es)\n", encontrados);
    }
}

// Listar eleitores por estado de voto
void listar_eleitores_por_estado_voto(ListaEleitores* lista, int votou) {
    char* estado = votou ? "JA VOTARAM" : "NAO VOTARAM";
    printf("\n=== ELEITORES QUE %s ===\n", estado);
    
    NoEleitor* atual = lista->inicio;
    int encontrados = 0;
    
    // Percorre lista procurando por estado de voto
    while (atual != NULL) {
        if (atual->eleitor.votou == votou) {
            printf("ID: %d | Nome: %s | Doc: %s | CAP: %d | Prioridade: %s\n",
                   atual->eleitor.id,
                   atual->eleitor.nome,
                   atual->eleitor.documento,
                   atual->eleitor.id_cap,
                   atual->eleitor.prioridade ? "Sim" : "Nao");
            encontrados++;
        }
        atual = atual->proximo;
    }
    
    if (encontrados == 0) {
        printf("Nenhum eleitor encontrado.\n");
    } else {
        printf("Total: %d eleitor(es)\n", encontrados);
    }
}

// Listar eleitores prioritários
void listar_eleitores_prioritarios(ListaEleitores* lista) {
    printf("\n=== ELEITORES PRIORITARIOS ===\n");
    
    NoEleitor* atual = lista->inicio;
    int encontrados = 0;
    
    // Percorre lista procurando eleitores prioritários
    while (atual != NULL) {
        if (atual->eleitor.prioridade) {
            printf("ID: %d | Nome: %s | Doc: %s | CAP: %d | Votou: %s\n",
                   atual->eleitor.id,
                   atual->eleitor.nome,
                   atual->eleitor.documento,
                   atual->eleitor.id_cap,
                   atual->eleitor.votou ? "Sim" : "Nao");
            encontrados++;
        }
        atual = atual->proximo;
    }
    
    if (encontrados == 0) {
        printf("Nenhum eleitor prioritario encontrado.\n");
    } else {
        printf("Total: %d eleitor(es) prioritario(s)\n", encontrados);
    }
}

// Exibir detalhes de um eleitor
void exibir_detalhes_eleitor(Eleitor* eleitor) {
    if (eleitor == NULL) {
        printf("Eleitor invalido!\n");
        return;
    }
    
    printf("  ID: %d\n", eleitor->id);
    printf("  Nome: %s\n", eleitor->nome);
    printf("  Documento: %s\n", eleitor->documento);
    printf("  CAP associado: %d\n", eleitor->id_cap);
    printf("  Estado do voto: %s\n", eleitor->votou ? "VOTOU" : "NAO VOTOU");
    printf("  Prioridade: %s\n", eleitor->prioridade ? "SIM (gravida/idoso/deficiente)" : "NAO");
}

// Verificar se eleitor pode votar
int eleitor_pode_votar(Eleitor* eleitor) {
    if (eleitor == NULL) {
        return 0;
    }
    
    return (eleitor->votou == 0); // Pode votar se não votou ainda
}

// Marcar eleitor como votado
int marcar_como_votado(ListaEleitores* lista, int id_eleitor) {
    // Busca eleitor por ID
    Eleitor* eleitor = buscar_eleitor_por_id(lista, id_eleitor);
    
    if (eleitor == NULL) {
        printf("Erro: Eleitor com ID %d nao encontrado!\n", id_eleitor);
        return 0;
    }
    
    if (eleitor->votou) {
        printf("Erro: Eleitor ja votou anteriormente!\n");
        return 0;
    }
    
    eleitor->votou = 1; // Marca como votado
    printf("Eleitor %s (ID: %d) marcado como votado.\n", eleitor->nome, eleitor->id);
    return 1;
}

// Verificar se eleitor já votou
int verificar_eleitor_votou(ListaEleitores* lista, int id_eleitor) {
    Eleitor* eleitor = buscar_eleitor_por_id(lista, id_eleitor);
    
    if (eleitor == NULL) {
        return -1; // Eleitor não encontrado
    }
    
    return eleitor->votou; // Retorna 0 ou 1
}

// Contar eleitores cadastrados
int contar_eleitores_cadastrados(ListaEleitores* lista) {
    return lista->tamanho;
}

// Contar eleitores por CAP
int contar_eleitores_por_cap(ListaEleitores* lista, int id_cap) {
    NoEleitor* atual = lista->inicio;
    int contador = 0;
    
    // Percorre lista contando eleitores do CAP
    while (atual != NULL) {
        if (atual->eleitor.id_cap == id_cap) {
            contador++;
        }
        atual = atual->proximo;
    }
    
    return contador;
}

// Contar eleitores que já votaram
int contar_eleitores_votaram(ListaEleitores* lista) {
    NoEleitor* atual = lista->inicio;
    int contador = 0;
    
    // Percorre lista contando eleitores que votaram
    while (atual != NULL) {
        if (atual->eleitor.votou) {
            contador++;
        }
        atual = atual->proximo;
    }
    
    return contador;
}

// Contar eleitores que não votaram
int contar_eleitores_nao_votaram(ListaEleitores* lista) {
    return lista->tamanho - contar_eleitores_votaram(lista);
}

// Contar eleitores prioritários
int contar_eleitores_prioritarios(ListaEleitores* lista) {
    NoEleitor* atual = lista->inicio;
    int contador = 0;
    
    // Percorre lista contando eleitores prioritários
    while (atual != NULL) {
        if (atual->eleitor.prioridade) {
            contador++;
        }
        atual = atual->proximo;
    }
    
    return contador;
}

// Exportar eleitores para CSV (opcional)
int exportar_eleitores_para_csv(ListaEleitores* lista, const char* filename) {
    // Abre arquivo para escrita
    FILE* arquivo = fopen(filename, "w");
    
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo %s\n", filename);
        return 0;
    }
    
    // Escrever cabeçalho
    fprintf(arquivo, "ID;Nome;Documento;CAP;Votou;Prioridade\n");
    
    // Escrever dados
    NoEleitor* atual = lista->inicio;
    
    while (atual != NULL) {
        fprintf(arquivo, "%d;%s;%s;%d;%d;%d\n",
                atual->eleitor.id,
                atual->eleitor.nome,
                atual->eleitor.documento,
                atual->eleitor.id_cap,
                atual->eleitor.votou,
                atual->eleitor.prioridade);
        atual = atual->proximo;
    }
    
    fclose(arquivo);
    printf("Dados exportados para %s\n", filename);
    return 1;
}

// Importar eleitores de CSV (opcional)
int importar_eleitores_de_csv(ListaEleitores* lista, const char* filename) {
    // Abre arquivo para leitura
    FILE* arquivo = fopen(filename, "r");
    
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo %s\n", filename);
        return 0;
    }
    
    char linha[500];
    int importados = 0;
    
    // Ler cabeçalho
    fgets(linha, sizeof(linha), arquivo);
    
    // Ler dados
    while (fgets(linha, sizeof(linha), arquivo)) {
        char nome[MAX_NOME], documento[MAX_DOC];
        int id, id_cap, votou, prioridade;
        
        // Formato: ID;Nome;Documento;CAP;Votou;Prioridade
        if (sscanf(linha, "%d;%[^;];%[^;];%d;%d;%d", 
                   &id, nome, documento, &id_cap, &votou, &prioridade) == 6) {
            
            // Verificar se documento já existe
            if (!documento_existe(lista, documento)) {
                // Criar eleitor manualmente (sem usar cadastrar_eleitor para manter IDs originais)
                NoEleitor* novo_no = (NoEleitor*)malloc(sizeof(NoEleitor));
                if (novo_no != NULL) {
                    novo_no->eleitor.id = id;
                    strncpy(novo_no->eleitor.nome, nome, MAX_NOME - 1);
                    strncpy(novo_no->eleitor.documento, documento, MAX_DOC - 1);
                    novo_no->eleitor.id_cap = id_cap;
                    novo_no->eleitor.votou = votou;
                    novo_no->eleitor.prioridade = prioridade;
                    
                    novo_no->proximo = lista->inicio;
                    lista->inicio = novo_no;
                    lista->tamanho++;
                    
                    // Atualizar último ID se necessário
                    if (id > lista->ultimo_id) {
                        lista->ultimo_id = id;
                    }
                    
                    importados++;
                }
            }
        }
    }
    
    fclose(arquivo);
    printf("%d eleitores importados de %s\n", importados, filename);
    return importados > 0;
}
