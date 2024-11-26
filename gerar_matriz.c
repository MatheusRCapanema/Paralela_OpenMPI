#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LINHAS 50000
#define COLUNAS 50000
#define TAMANHO_PALAVRA 10

char gerar_letra_aleatoria() {
    return 'A' + rand() % 26;
}

int main() {
    FILE *arquivo;
    char **matriz;
    int i, j;
    char palavra[] = "OPENMPIEX";
    int tamanho_palavra = strlen(palavra);

    int direcao = rand() % 3;

    matriz = (char **)malloc(LINHAS * sizeof(char *));
    for (i = 0; i < LINHAS; i++) {
        matriz[i] = (char *)malloc((COLUNAS + 1) * sizeof(char));
    }

    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            matriz[i][j] = gerar_letra_aleatoria();
        }
        matriz[i][COLUNAS] = '\0';
    }

    int linha_inicio, coluna_inicio;

    if (direcao == 0) {
        linha_inicio = rand() % LINHAS;
        coluna_inicio = rand() % (COLUNAS - tamanho_palavra);
    } else if (direcao == 1) {
        linha_inicio = rand() % (LINHAS - tamanho_palavra);
        coluna_inicio = rand() % COLUNAS;
    } else {
        linha_inicio = rand() % (LINHAS - tamanho_palavra);
        coluna_inicio = rand() % (COLUNAS - tamanho_palavra);
    }

    for (i = 0; i < tamanho_palavra; i++) {
        if (direcao == 0) {
            matriz[linha_inicio][coluna_inicio + i] = palavra[i];
        } else if (direcao == 1) {
            matriz[linha_inicio + i][coluna_inicio] = palavra[i];
        } else {
            matriz[linha_inicio + i][coluna_inicio + i] = palavra[i];
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
    printf("Palavra '%s' inserida na matriz na direção %s, posição (linha: %d, coluna: %d).\n",
           palavra,
           (direcao == 0) ? "Horizontal" : (direcao == 1) ? "Vertical" : "Diagonal",
           linha_inicio + 1,
           coluna_inicio + 1);

    for (i = 0; i < LINHAS; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}

