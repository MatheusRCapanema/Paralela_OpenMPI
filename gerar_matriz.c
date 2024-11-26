#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LINHAS 50000
#define COLUNAS 50000
#define MAX_TAMANHO_PALAVRA 100
#define MAX_TENTATIVAS 1000

char gerar_letra_aleatoria() {
    return 'A' + rand() % 26;
}

typedef struct {
    char palavra[MAX_TAMANHO_PALAVRA + 1];
    int tamanho_palavra;
    int quantidade;
} Palavra;

#define SET_BIT(mask, row, col) (mask[row][(col) / 8] |= (1 << ((col) % 8)))
#define GET_BIT(mask, row, col) (mask[row][(col) / 8] & (1 << ((col) % 8)))

int pode_inserir_palavra(unsigned char **mask, int linha_inicio, int coluna_inicio, int direcao, int tamanho_palavra) {
    int i;
    if (direcao == 0) { // Horizontal
        for (i = 0; i < tamanho_palavra; i++) {
            if (GET_BIT(mask, linha_inicio, coluna_inicio + i)) {
                return 0;
            }
        }
    } else if (direcao == 1) { // Vertical
        for (i = 0; i < tamanho_palavra; i++) {
            if (GET_BIT(mask, linha_inicio + i, coluna_inicio)) {
                return 0;
            }
        }
    } else { // Diagonal
        for (i = 0; i < tamanho_palavra; i++) {
            if (GET_BIT(mask, linha_inicio + i, coluna_inicio + i)) {
                return 0;
            }
        }
    }
    return 1;
}

void inserir_palavra(char **matriz, unsigned char **mask, int linha_inicio, int coluna_inicio, int direcao, char *palavra, int tamanho_palavra) {
    int i;
    if (direcao == 0) { // Horizontal
        for (i = 0; i < tamanho_palavra; i++) {
            matriz[linha_inicio][coluna_inicio + i] = palavra[i];
            SET_BIT(mask, linha_inicio, coluna_inicio + i);
        }
    } else if (direcao == 1) { // Vertical
        for (i = 0; i < tamanho_palavra; i++) {
            matriz[linha_inicio + i][coluna_inicio] = palavra[i];
            SET_BIT(mask, linha_inicio + i, coluna_inicio);
        }
    } else { // Diagonal
        for (i = 0; i < tamanho_palavra; i++) {
            matriz[linha_inicio + i][coluna_inicio + i] = palavra[i];
            SET_BIT(mask, linha_inicio + i, coluna_inicio + i);
        }
    }
}

int main() {
    FILE *arquivo;
    char **matriz;
    unsigned char **mask;
    int i, j, k;

    srand(time(NULL));

    int num_palavras;
    printf("Digite o número de palavras: ");
    scanf("%d", &num_palavras);

    Palavra *palavras = malloc(num_palavras * sizeof(Palavra));

    for (i = 0; i < num_palavras; i++) {
        printf("Digite a palavra %d: ", i + 1);
        scanf("%s", palavras[i].palavra);
        palavras[i].tamanho_palavra = strlen(palavras[i].palavra);
        printf("Digite a quantidade de vezes que a palavra '%s' deve aparecer: ", palavras[i].palavra);
        scanf("%d", &palavras[i].quantidade);
    }

    matriz = (char **)malloc(LINHAS * sizeof(char *));
    for (i = 0; i < LINHAS; i++) {
        matriz[i] = (char *)malloc((COLUNAS + 1) * sizeof(char));
    }

    mask = (unsigned char **)malloc(LINHAS * sizeof(unsigned char *));
    for (i = 0; i < LINHAS; i++) {
        mask[i] = (unsigned char *)calloc((COLUNAS + 7) / 8, sizeof(unsigned char));
    }

    // Preenche a matriz com letras aleatórias
    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            matriz[i][j] = gerar_letra_aleatoria();
        }
        matriz[i][COLUNAS] = '\0';
    }

    // Insere as palavras na matriz
    for (k = 0; k < num_palavras; k++) {
        int inseridas = 0;
        while (inseridas < palavras[k].quantidade) {
            int direcao = rand() % 3;
            int linha_inicio, coluna_inicio;

            if (direcao == 0) { // Horizontal
                linha_inicio = rand() % LINHAS;
                coluna_inicio = rand() % (COLUNAS - palavras[k].tamanho_palavra);
            } else if (direcao == 1) { // Vertical
                linha_inicio = rand() % (LINHAS - palavras[k].tamanho_palavra);
                coluna_inicio = rand() % COLUNAS;
            } else { // Diagonal
                linha_inicio = rand() % (LINHAS - palavras[k].tamanho_palavra);
                coluna_inicio = rand() % (COLUNAS - palavras[k].tamanho_palavra);
            }

            if (pode_inserir_palavra(mask, linha_inicio, coluna_inicio, direcao, palavras[k].tamanho_palavra)) {
                inserir_palavra(matriz, mask, linha_inicio, coluna_inicio, direcao, palavras[k].palavra, palavras[k].tamanho_palavra);
                printf("Palavra '%s' inserida na direção %s, posição (linha: %d, coluna: %d).\n",
                       palavras[k].palavra,
                       (direcao == 0) ? "Horizontal" : (direcao == 1) ? "Vertical" : "Diagonal",
                       linha_inicio + 1,
                       coluna_inicio + 1);
                inseridas++;
            } else {
                static int tentativas = 0;
                tentativas++;
                if (tentativas > MAX_TENTATIVAS) {
                    printf("Não foi possível inserir a palavra '%s' após %d tentativas.\n", palavras[k].palavra, MAX_TENTATIVAS);
                    break;
                }
            }
        }
    }

    arquivo = fopen("matriz_grande.txt", "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < LINHAS; i++) {
        fprintf(arquivo, "%s\n", matriz[i]);
    }

    fclose(arquivo);
    printf("Arquivo 'matriz_grande.txt' gerado com sucesso!\n");

    for (i = 0; i < LINHAS; i++) {
        free(matriz[i]);
        free(mask[i]);
    }
    free(matriz);
    free(mask);
    free(palavras);

    return 0;
}
