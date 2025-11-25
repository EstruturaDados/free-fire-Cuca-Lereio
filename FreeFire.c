/* mochila.c
 *
 * Esta é uma Versão simples do sistema de mochila com duas representações:
 *  - Vetor (lista sequencial)
 *  - Lista encadeada (dinâmica)
 *
 * Operações implementadas:
 *  - inserir
 *  - remover por nome
 *  - listar todos itens
 *  - buscar sequencial por nome
 *
 * Operações adicionais (vetor):
 *  - ordenar por nome (Selection Sort)
 *  - buscar binária por nome (após ordenar)
 *
 * Contadores de comparações:
 *  - seqComparisonsVetor
 *  - binComparisonsVetor
 *  - seqComparisonsLista
 *
 * Para compilar:
 *   gcc mochila.c -o mochila
 *
 * Autor: Revyd Ribeiro Oliveira
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITENS_VETOR 100
#define NOME_LEN 30
#define TIPO_LEN 20

/* --------------------------- Structs --------------------------- */

typedef struct {
    char nome[NOME_LEN];
    char tipo[TIPO_LEN];
    int quantidade;
} Item;

typedef struct No {
    Item dados;
    struct No *proximo;
} No;

/* ------------------------ Vetor (sequencial) ------------------------ */

typedef struct {
    Item itens[MAX_ITENS_VETOR];
    int tamanho;
} MochilaVetor;

/* --------------------- Contadores / Estatísticas --------------------- */
/* Comparações realizadas nas buscas */
long long seqComparisonsVetor = 0;
long long binComparisonsVetor = 0;
long long seqComparisonsLista = 0;

/* ------------------------ Funções utilitárias ------------------------ */

/* limpa buffer de entrada restante */
void limparEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* lê uma string com segurança (remove newline) */
void lerString(const char *prompt, char *buffer, int tamanho) {
    printf("%s", prompt);
    if (fgets(buffer, tamanho, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
}

/* imprime um item */
void imprimirItem(const Item *it) {
    printf("Nome: %-28s | Tipo: %-12s | Qtde: %d\n", it->nome, it->tipo, it->quantidade);
}

/* ----------------------- Funções do vetor ----------------------- */

/* inicializa mochila vetor */
void inicializarVetor(MochilaVetor *m) {
    m->tamanho = 0;
}

/* busca sequencial no vetor (retorna índice ou -1) */
int buscarSequencialVetor(MochilaVetor *m, const char *nome) {
    for (int i = 0; i < m->tamanho; ++i) {
        seqComparisonsVetor++;
        if (strcmp(m->itens[i].nome, nome) == 0) return i;
    }
    return -1;
}

/* inserir no vetor:
   - se existir item com mesmo nome, soma quantidade
   - caso contrário, adiciona no final (se houver espaço)
*/
void inserirItemVetor(MochilaVetor *m, Item it) {
    int idx = buscarSequencialVetor(m, it.nome);
    if (idx != -1) {
        m->itens[idx].quantidade += it.quantidade;
        printf("Item existente. Quantidade atualizada.\n");
        return;
    }
    if (m->tamanho >= MAX_ITENS_VETOR) {
        printf("Mochila cheia (vetor). Não é possível inserir.\n");
        return;
    }
    m->itens[m->tamanho++] = it;
    printf("Item inserido no vetor.\n");
}

/* remover por nome no vetor */
void removerItemVetor(MochilaVetor *m, const char *nome) {
    int idx = buscarSequencialVetor(m, nome);
    if (idx == -1) {
        printf("Item não encontrado no vetor.\n");
        return;
    }
    /* desloca elementos à esquerda */
    for (int i = idx; i < m->tamanho - 1; ++i) {
        m->itens[i] = m->itens[i+1];
    }
    m->tamanho--;
    printf("Item removido do vetor.\n");
}

/* listar itens do vetor */
void listarVetor(MochilaVetor *m) {
    if (m->tamanho == 0) {
        printf("(Vetor) Mochila vazia.\n");
        return;
    }
    printf("Itens na mochila (vetor) - total: %d\n", m->tamanho);
    for (int i = 0; i < m->tamanho; ++i) {
        printf("[%d] ", i);
        imprimirItem(&m->itens[i]);
    }
}

/* ordenar vetor por nome (Selection Sort) */
void ordenarVetor(MochilaVetor *m) {
    for (int i = 0; i < m->tamanho - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < m->tamanho; ++j) {
            if (strcmp(m->itens[j].nome, m->itens[minIdx].nome) < 0) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Item tmp = m->itens[i];
            m->itens[i] = m->itens[minIdx];
            m->itens[minIdx] = tmp;
        }
    }
    printf("Vetor ordenado por nome.\n");
}

/* busca binária no vetor (supõe vetor ordenado) */
int buscarBinariaVetor(MochilaVetor *m, const char *nome) {
    int left = 0, right = m->tamanho - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        binComparisonsVetor++;
        int cmp = strcmp(m->itens[mid].nome, nome);
        if (cmp == 0) return mid;
        else if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

/* --------------------- Funções da lista encadeada --------------------- */

/* inserir item no início (simples) - se já existir, soma quantidade */
void inserirItemLista(No **cabeca, Item it) {
    /* busca se existe */
    No *curr = *cabeca;
    while (curr != NULL) {
        seqComparisonsLista++;
        if (strcmp(curr->dados.nome, it.nome) == 0) {
            curr->dados.quantidade += it.quantidade;
            printf("Item existente na lista. Quantidade atualizada.\n");
            return;
        }
        curr = curr->proximo;
    }
    /* inserir no início */
    No *novo = (No*) malloc(sizeof(No));
    if (!novo) {
        printf("Erro de alocação.\n");
        return;
    }
    novo->dados = it;
    novo->proximo = *cabeca;
    *cabeca = novo;
    printf("Item inserido na lista encadeada.\n");
}

/* remover por nome na lista */
void removerItemLista(No **cabeca, const char *nome) {
    No *curr = *cabeca;
    No *prev = NULL;
    while (curr != NULL) {
        seqComparisonsLista++;
        if (strcmp(curr->dados.nome, nome) == 0) {
            /* remove curr */
            if (prev == NULL) *cabeca = curr->proximo;
            else prev->proximo = curr->proximo;
            free(curr);
            printf("Item removido da lista.\n");
            return;
        }
        prev = curr;
        curr = curr->proximo;
    }
    printf("Item não encontrado na lista.\n");
}

/* listar itens da lista */
void listarLista(No *cabeca) {
    if (cabeca == NULL) {
        printf("(Lista) Mochila vazia.\n");
        return;
    }
    printf("Itens na mochila (lista encadeada):\n");
    No *curr = cabeca;
    int i = 0;
    while (curr != NULL) {
        printf("[%d] ", i++);
        imprimirItem(&curr->dados);
        curr = curr->proximo;
    }
}

/* buscar sequencial na lista  */
No* buscarSequencialLista(No *cabeca, const char *nome) {
    No *curr = cabeca;
    while (curr != NULL) {
        seqComparisonsLista++;
        if (strcmp(curr->dados.nome, nome) == 0) return curr;
        curr = curr->proximo;
    }
    return NULL;
}

/* liberar toda a lista (para sair) */
void liberarLista(No **cabeca) {
    No *curr = *cabeca;
    while (curr != NULL) {
        No *prox = curr->proximo;
        free(curr);
        curr = prox;
    }
    *cabeca = NULL;
}

/* --------------------------- Menu / UI --------------------------- */

void menuVetor(MochilaVetor *m) {
    int opcao;
    char nomeBusca[NOME_LEN], tipo[TIPO_LEN];
    Item it;
    do {
        printf("\n=== Mochila (Vetor) ===\n");
        printf("1. Inserir item\n");
        printf("2. Remover item por nome\n");
        printf("3. Listar itens\n");
        printf("4. Buscar sequencial (linear)\n");
        printf("5. Ordenar por nome (Selection Sort)\n");
        printf("6. Buscar binária (requer ordenação)\n");
        printf("7. Mostrar contadores de comparações\n");
        printf("0. Voltar ao menu principal\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) { limparEntrada(); opcao = -1; }
        limparEntrada();
        switch (opcao) {
            case 1:
                lerString("Nome: ", it.nome, NOME_LEN);
                lerString("Tipo: ", it.tipo, TIPO_LEN);
                printf("Quantidade: ");
                if (scanf("%d", &it.quantidade) != 1) { limparEntrada(); it.quantidade = 1; }
                limparEntrada();
                inserirItemVetor(m, it);
                break;
            case 2:
                lerString("Nome do item a remover: ", nomeBusca, NOME_LEN);
                removerItemVetor(m, nomeBusca);
                break;
            case 3:
                listarVetor(m);
                break;
            case 4:
                lerString("Nome para busca (sequencial): ", nomeBusca, NOME_LEN);
                {
                    clock_t t0 = clock();
                    int idx = buscarSequencialVetor(m, nomeBusca);
                    clock_t t1 = clock();
                    double tm = (double)(t1 - t0) / CLOCKS_PER_SEC;
                    if (idx != -1) {
                        printf("Item encontrado no índice %d:\n", idx);
                        imprimirItem(&m->itens[idx]);
                    } else printf("Item não encontrado (sequencial).\n");
                    printf("Tempo: %.6f s | Comparações (seq vetor): %lld\n", tm, seqComparisonsVetor);
                }
                break;
            case 5:
                {
                    clock_t t0 = clock();
                    ordenarVetor(m);
                    clock_t t1 = clock();
                    double tm = (double)(t1 - t0) / CLOCKS_PER_SEC;
                    printf("Tempo de ordenação: %.6f s\n", tm);
                }
                break;
            case 6:
                lerString("Nome para busca (binária): ", nomeBusca, NOME_LEN);
                {
                    clock_t t0 = clock();
                    int idx = buscarBinariaVetor(m, nomeBusca);
                    clock_t t1 = clock();
                    double tm = (double)(t1 - t0) / CLOCKS_PER_SEC;
                    if (idx != -1) {
                        printf("Item encontrado (binária) no índice %d:\n", idx);
                        imprimirItem(&m->itens[idx]);
                    } else printf("Item não encontrado (binária).\n");
                    printf("Tempo: %.6f s | Comparações (binária vetor): %lld\n", tm, binComparisonsVetor);
                }
                break;
            case 7:
                printf("Comparações acumuladas:\n");
                printf(" - Busca sequencial: %lld\n", seqComparisonsVetor);
                printf(" - Busca binária: %lld\n", binComparisonsVetor);
                break;
            case 0:
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}

void menuLista(No **cabeca) {
    int opcao;
    char nomeBusca[NOME_LEN], tipo[TIPO_LEN];
    Item it;
    do {
        printf("\n=== Mochila (Lista Encadeada) ===\n");
        printf("1. Inserir item\n");
        printf("2. Remover item por nome\n");
        printf("3. Listar itens\n");
        printf("4. Buscar sequencial (lista)\n");
        printf("5. Mostrar contadores de comparações\n");
        printf("0. Voltar ao menu principal\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) { limparEntrada(); opcao = -1; }
        limparEntrada();
        switch (opcao) {
            case 1:
                lerString("Nome: ", it.nome, NOME_LEN);
                lerString("Tipo: ", it.tipo, TIPO_LEN);
                printf("Quantidade: ");
                if (scanf("%d", &it.quantidade) != 1) { limparEntrada(); it.quantidade = 1; }
                limparEntrada();
                inserirItemLista(cabeca, it);
                break;
            case 2:
                lerString("Nome do item a remover: ", nomeBusca, NOME_LEN);
                removerItemLista(cabeca, nomeBusca);
                break;
            case 3:
                listarLista(*cabeca);
                break;
            case 4:
                lerString("Nome para busca (sequencial - lista): ", nomeBusca, NOME_LEN);
                {
                    clock_t t0 = clock();
                    No *achado = buscarSequencialLista(*cabeca, nomeBusca);
                    clock_t t1 = clock();
                    double tm = (double)(t1 - t0) / CLOCKS_PER_SEC;
                    if (achado != NULL) {
                        printf("Item encontrado na lista:\n");
                        imprimirItem(&achado->dados);
                    } else printf("Item não encontrado (lista).\n");
                    printf("Tempo: %.6f s | Comparações (seq lista): %lld\n", tm, seqComparisonsLista);
                }
                break;
            case 5:
                printf("Comparações acumuladas:\n");
                printf(" - Busca sequencial (lista): %lld\n", seqComparisonsLista);
                break;
            case 0:
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}

/* função exemplificada (opcional) */
void popularExemplo(MochilaVetor *mv, No **cabeca) {
    Item itensEx[] = {
        {"PocaoVermelha", "Cura", 2},
        {"PocaoAzul", "Mana", 3},
        {"Bandagem", "Cura", 5},
        {"Granada", "Ataque", 1},
        {"ChaveMisteriosa", "Missao", 1},
        {"Lanterna", "Util", 1},
        {"PocaoVermelha", "Cura", 1}, /* para testar merge */
    };
    int n = sizeof(itensEx)/sizeof(itensEx[0]);
    for (int i = 0; i < n; ++i) {
        inserirItemVetor(mv, itensEx[i]);
        inserirItemLista(cabeca, itensEx[i]);
    }
}

/* ----------------------------- main ----------------------------- */

int main() {
    MochilaVetor mochilaV;
    No *cabeca = NULL;
    inicializarVetor(&mochilaV);

    printf("=== Sistema de Mochila: Vetor vs Lista Encadeada ===\n");
    printf("Requisitos: inserir, remover, listar, buscar (sequencial),\nordenar (vetor) e buscar binária (vetor). Contadores incluídos.\n");

    /* alguns itens para teste (opcional) */
    popularExemplo(&mochilaV, &cabeca);

    int opcao;
    do {
        printf("\n--- Menu Principal ---\n");
        printf("1. Usar mochila (vetor)\n");
        printf("2. Usar mochila (lista encadeada)\n");
        printf("3. Mostrar contadores (todos)\n");
        printf("9. Reiniciar contadores\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) { limparEntrada(); opcao = -1; }
        limparEntrada();
        switch (opcao) {
            case 1:
                menuVetor(&mochilaV);
                break;
            case 2:
                menuLista(&cabeca);
                break;
            case 3:
                printf("Contadores atuais:\n");
                printf(" - Busca sequencial (vetor): %lld\n", seqComparisonsVetor);
                printf(" - Busca binária (vetor): %lld\n", binComparisonsVetor);
                printf(" - Busca sequencial (lista): %lld\n", seqComparisonsLista);
                break;
            case 9:
                seqComparisonsVetor = 0;
                binComparisonsVetor = 0;
                seqComparisonsLista = 0;
                printf("Contadores reiniciados.\n");
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);

    liberarLista(&cabeca);
    return 0;
}
