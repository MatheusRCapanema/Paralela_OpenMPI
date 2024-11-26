#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100

void read_file(char *filename, char ***matrix, int *rows, int *cols);
void search_words(char **matrix, int rows, int cols, char *word_list[], int word_count);
int search_word_in_direction(char **matrix, int rows, int cols, int start_row, int start_col, char *word, int x_dir, int y_dir);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char **matrix;
    int rows, cols;
    char *filename = "matriz_grande.txt";

    if (rank == 0) {
        read_file(filename, &matrix, &rows, &cols);
    }

    // Broadcast do tamanho da matriz para todos os processos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Alocar memória para a matriz em todos os processos
    if (rank != 0) {
        matrix = (char **)malloc(rows * sizeof(char *));
        for (int i = 0; i < rows; i++) {
            matrix[i] = (char *)malloc((cols + 1) * sizeof(char));
        }
    }

    // Broadcast da matriz para todos os processos
    for (int i = 0; i < rows; i++) {
        MPI_Bcast(matrix[i], cols + 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Lista de palavras a serem buscadas
    char *word_list[] = {"algoritmos","bubblesort","quicksort","mergesort","arvore","openmp","prova"};
    int word_count = sizeof(word_list) / sizeof(word_list[0]);

    // Cada processo busca por palavras em uma parte da matriz
    search_words(matrix, rows, cols, word_list, word_count);

    // Liberar memória
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);

    MPI_Finalize();
    return 0;
}

void read_file(char *filename, char ***matrix, int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Primeiro, contar o número de linhas e colunas
    char ch;
    int temp_cols = 0;
    *rows = 0;
    *cols = 0;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            (*rows)++;
            if (temp_cols > *cols) {
                *cols = temp_cols;
            }
            temp_cols = 0;
        } else {
            temp_cols++;
        }
    }
    rewind(file);

    // Alocar memória para a matriz
    *matrix = (char **)malloc((*rows) * sizeof(char *));
    for (int i = 0; i < *rows; i++) {
        (*matrix)[i] = (char *)malloc((*cols + 1) * sizeof(char));
    }

    // Ler o arquivo novamente e preencher a matriz
    int row = 0, col = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            (*matrix)[row][col] = '\0';
            row++;
            col = 0;
        } else {
            (*matrix)[row][col++] = ch;
        }
    }
    fclose(file);
}

void search_words(char **matrix, int rows, int cols, char *word_list[], int word_count) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Dividir as linhas entre os processos
    int lines_per_process = rows / size;
    int start_line = rank * lines_per_process;
    int end_line = (rank == size - 1) ? rows : start_line + lines_per_process;

    // Direções de busca (8 direções)
    int directions[8][2] = {
        {-1, 0}, // cima
        {1, 0},  // baixo
        {0, -1}, // esquerda
        {0, 1},  // direita
        {-1, -1}, // diagonal cima-esquerda
        {-1, 1},  // diagonal cima-direita
        {1, -1},  // diagonal baixo-esquerda
        {1, 1}    // diagonal baixo-direita
    };

    for (int w = 0; w < word_count; w++) {
        char *word = word_list[w];
        int word_len = strlen(word);

        for (int i = start_line; i < end_line; i++) {
            for (int j = 0; j < cols; j++) {
                for (int d = 0; d < 8; d++) {
                    if (search_word_in_direction(matrix, rows, cols, i, j, word, directions[d][0], directions[d][1])) {
                        printf("Processo %d encontrou a palavra '%s' na posição (%d, %d) na direção (%d, %d)\n",
                               rank, word, i, j, directions[d][0], directions[d][1]);
                    }
                }
            }
        }
    }
}

int search_word_in_direction(char **matrix, int rows, int cols, int start_row, int start_col, char *word, int x_dir, int y_dir) {
    int word_len = strlen(word);

    for (int k = 0; k < word_len; k++) {
        int new_row = (start_row + k * x_dir + rows) % rows;
        int new_col = (start_col + k * y_dir + cols) % cols;

        if (tolower(matrix[new_row][new_col]) != tolower(word[k])) {
            return 0;
        }
    }
    return 1;
}


