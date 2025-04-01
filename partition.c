#include "partition.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Funkcja obliczająca macierz Laplace'a grafu
void compute_laplacian(graph_t *graph, double **laplacian) {
    int V = graph->vertices;
    int *degree = (int *)calloc(V, sizeof(int));

    // Obliczanie stopni wierzchołków
    for (int i = 0; i < V; i++) {
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            degree[i]++;
        }
    }

    // Tworzenie macierzy Laplace'a
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            laplacian[i][j] = 0.0;
        }
    }
    for (int i = 0; i < V; i++) {
        laplacian[i][i] = degree[i];
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            int neighbor = graph->adjacency[j];
            laplacian[i][neighbor] = -1.0;
        }
    }
    free(degree);
}

// Funkcja obliczająca iloczyn macierzy przez wektor
void mat_vec_multiply(double **matrix, double *vector, double *result, int V) {
    for (int i = 0; i < V; i++) {
        result[i] = 0.0;
        for (int j = 0; j < V; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

// Funkcja obliczająca normę wektora
double norm(double *vector, int V) {
    double sum = 0.0;
    for (int i = 0; i < V; i++) {
        sum += vector[i] * vector[i];
    }
    return sqrt(sum);
}

// Funkcja obliczająca drugi najmniejszy wektor własny macierzy Laplace'a (wektor Fiedlera)
double* compute_fiedler_vector(double **laplacian, int V) {
    double *x = (double *)calloc(V, sizeof(double));  // Wektor początkowy
    double *Ax = (double *)calloc(V, sizeof(double)); // Wektor wynikowy Ax
    double *r = (double *)calloc(V, sizeof(double));  // Reszta Ax - λx
    double alpha, beta, norm_r;

    // Inicjalizacja
    for (int i = 0; i < V; i++) {
        x[i] = 1.0; // Zaczynamy od wektora o wszystkich elementach równych 1
    }

    for (int iter = 0; iter < 1000; iter++) {
        mat_vec_multiply(laplacian, x, Ax, V);
        norm_r = norm(Ax, V);

        // Reszta (błąd)
        for (int i = 0; i < V; i++) {
            r[i] = Ax[i] - x[i] * norm_r;
        }

        // Normalizacja reszty
        norm_r = norm(r, V);
        for (int i = 0; i < V; i++) {
            r[i] /= norm_r;
        }

        // Obliczanie współczynnika alpha (gdzie jest to najmniejsze wektory własne)
        alpha = 0;
        for (int i = 0; i < V; i++) {
            alpha += x[i] * r[i];
        }

        // Aktualizacja wektora x
        for (int i = 0; i < V; i++) {
            x[i] -= alpha * r[i];
        }

        // Współczynnik beta
        beta = norm(x, V);
        for (int i = 0; i < V; i++) {
            x[i] /= beta;
        }
    }

    free(Ax);
    free(r);

    return x;
}

// Funkcja dzieląca graf na dwie części na podstawie wektora Fiedlera
partition_t spectral_partition(graph_t *graph) {
    int V = graph->vertices;
    double **laplacian = (double **)malloc(V * sizeof(double *));
    for (int i = 0; i < V; i++) {
        laplacian[i] = (double *)malloc(V * sizeof(double));
    }

    compute_laplacian(graph, laplacian);
    double *fiedler_vector = compute_fiedler_vector(laplacian, V);

    partition_t partition;
    partition.partition = (int *)malloc(V * sizeof(int));
    partition.numParts = 2;

    // Podział grafu w oparciu o znak wartości w wektorze Fiedlera
    for (int i = 0; i < V; i++) {
        partition.partition[i] = (fiedler_vector[i] >= 0) ? 1 : 0;
    }

    free(fiedler_vector);
    for (int i = 0; i < V; i++) {
        free(laplacian[i]);
    }
    free(laplacian);

    return partition;
}
