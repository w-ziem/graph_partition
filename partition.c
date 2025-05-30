#include "partition.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define laplacianL(i,j) laplacian[(i)*V + (j)]

// Funkcja obliczająca macierz Laplace'a grafu
void compute_laplacian(graph_t *graph, double *laplacian) {
    int V = graph->vertices;
    int *degree = (int *)calloc(V, sizeof(int));  // Stopnie wierzchołków

    if (!degree) {
        fprintf(stderr, "Błąd alokacji pamięci dla stopni wierzchołków\n");
        exit(EXIT_FAILURE);
    }

    // Liczenie liczby krawędzi wychodzących z każdego wierzchołka
    for (int i = 0; i < V; i++) {
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            degree[i]++;
        }
    }

    // Inicjalizacja macierzy Laplace'a zerami
    for (int i = 0; i < V * V; i++) {
        for (int j = 0; j < V; j++) {
            laplacian[i] = 0.0;
        }
    }

    // Wypełnianie macierzy Laplace'a
    for (int i = 0; i < V; i++) {
        laplacianL(i, i) = degree[i];  // Wartości na przekątnej = stopnie
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            if (j == graph->adjacencyCount) return;  // Bezpieczne zakończenie
            int neighbor = graph->adjacency[j];
            laplacianL(i, neighbor) = -1.0;  // Połączenia między wierzchołkami
        }
    }
}

// Funkcja mnożąca macierz przez wektor
void mat_vec_multiply(double *matrix, double *vector, double *result, int V) {
    #pragma omp parallel for
    for (int i = 0; i < V; i++) {
        double sum = 0.0;
        for (int j = 0; j < V; j++) {
            sum += matrix[i * V + j] * vector[j];
        }
        result[i] = sum;
    }
}

// Funkcja licząca normę (długość) wektora
double norm(double *vector, int V) {
    double sum = 0.0;
    for (int i = 0; i < V; i++) {
        sum += vector[i] * vector[i];
    }
    return sqrt(sum);
}

// Funkcja obliczająca przybliżony wektor własny (Fiedlera) metodą iteracji potęgowej
double* compute_fiedler_vector(double *laplacian, int V) {
    double *x = (double *)calloc(V, sizeof(double));
    double *y = (double *)calloc(V, sizeof(double));
    
    // Initialize with random values
    for (int i = 0; i < V; i++) {
        x[i] = (double)rand() / RAND_MAX - 0.5;
    }
    
    // Remove component along first eigenvector (constant vector)
    double sum = 0.0;
    for (int i = 0; i < V; i++) sum += x[i];
    double avg = sum / V;
    for (int i = 0; i < V; i++) x[i] -= avg;
    
    // Power iteration with deflation
    double epsilon = 1e-6;  // Relaxed convergence criterion
    int max_iter = 100;     // Fewer iterations
    
    for (int iter = 0; iter < max_iter; iter++) {
        // Matrix-vector multiplication
        mat_vec_multiply(laplacian, x, y, V);
        
        // Remove constant component (deflation)
        sum = 0.0;
        for (int i = 0; i < V; i++) sum += y[i];
        avg = sum / V;
        for (int i = 0; i < V; i++) y[i] -= avg;
        
        // Normalize
        double norm_val = norm(y, V);
        if (norm_val < epsilon) break;
        
        for (int i = 0; i < V; i++) {
            x[i] = y[i] / norm_val;
        }
    }
    
    free(y);
    return x;
}

// Funkcja pomocnicza do sortowania indeksowanych wartości
static int compare_indexed_values(const void *a, const void *b) {
    indexed_value *va = (indexed_value*)a;
    indexed_value *vb = (indexed_value*)b;
    if (va->value < vb->value) return -1;
    if (va->value > vb->value) return 1;
    return 0;
}

// Funkcja dzieląca graf na numParts części metodą spektralną
partition_t spectral_partition(graph_t *graph, int numParts) {
    int V = graph->vertices;

    // Alokacja i obliczenie macierzy Laplace'a
    double *laplacian = (double *)malloc(V * V * sizeof(double *));

    if (!laplacian) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }


    compute_laplacian(graph, laplacian);
    double *fiedler = compute_fiedler_vector(laplacian, V);

    // Przygotowanie tablicy indeksowanych wartości
    indexed_value *values = malloc(V * sizeof(indexed_value));
    if (!values) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < V; i++) {
        values[i].index = i;
        values[i].value = fiedler[i];
    }

    // Sortowanie według wartości Fiedlera
    qsort(values, V, sizeof(indexed_value), compare_indexed_values);

    // Inicjalizacja podziału
    partition_t partition;
    partition.partition = (int *)malloc(V * sizeof(int));
    if (!partition.partition) {
        fprintf(stderr, "Memory allocation failed for partition\n");
        exit(1);
    }
    partition.numParts = numParts;

    // Podział na części równoliczne
    int base_size = V / numParts;
    int remainder = V % numParts;

    int current_pos = 0;
    for (int p = 0; p < numParts; p++) {
        int part_size = base_size + (p < remainder ? 1 : 0);
        for (int i = 0; i < part_size; i++) {
            partition.partition[values[current_pos + i].index] = p;
        }
        current_pos += part_size;
    }

    // Zwolnienie pamięci pomocniczej
    free(values);
    free(fiedler);
    free(laplacian);

    return partition;
}

// Funkcja ewaluująca jakość podziału grafu
void evaluate_partition(graph_t *graph, partition_t partition, double margin) {
    int V = graph->vertices;
    int numParts = partition.numParts;

    int *partVertices = (int *)calloc(numParts, sizeof(int));
    if (!partVertices) {
        fprintf(stderr, "Memory allocation failed in evaluate_partition\n");
        exit(EXIT_FAILURE);
    }

    int cutEdges = 0;

    // Liczenie wierzchołków w każdej części i przeciętych krawędzi
    for (int i = 0; i < V; i++) {
        int part = partition.partition[i];
        if (part < 0 || part >= numParts) {
            printf("Błąd: Wierzchołek %d przypisany do nieistniejącej części %d\n", i, part);
            free(partVertices);
            return;
        }
        partVertices[part]++;

        if (i + 1 >= V || graph->edgeIndices[i] < 0) continue;

        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1] && j < graph->adjacencyCount; j++) {
            int neighbor = graph->adjacency[j];
            if (neighbor >= 0 && neighbor < V) {
                if (partition.partition[i] != partition.partition[neighbor]) {
                    cutEdges++;
                }
            }
        }
    }

    cutEdges /= 2;  // Liczymy każdą przeciętą krawędź tylko raz

    printf("\n===== Ewaluacja podziału =====\n");
    // printf("Liczba przeciętych krawędzi: %d\n", cutEdges);

    double idealVertices = (double)V / numParts;
    double allowedDeviation = (margin / 100.0) * idealVertices;

    for (int p = 0; p < numParts; p++) {
        double percentage = (partVertices[p] * 100.0) / V;
        printf("Część %d: %d wierzchołków (%.2f%%)\n", p, partVertices[p], percentage);
    }

    // Sprawdzanie marginesu
    int ok = 1;
    for (int p = 0; p < numParts; p++) {
        if (fabs(partVertices[p] - idealVertices) > allowedDeviation) {
            ok = 0;
        }
    }

    if (ok) {
        printf("Podział spełnia margines %.2f%% dla %d części.\n", margin, numParts);
    } else {
        printf("Podział NIE spełnia marginesu %.2f%% dla %d części.\n", margin, numParts);
    }

    free(partVertices);
}