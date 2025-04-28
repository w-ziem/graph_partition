#include "partition.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Funkcja obliczająca macierz Laplace'a grafu
void compute_laplacian(graph_t *graph, double **laplacian) {
    int V = graph->vertices;
    printf("V w funkcji compute_laplacian z graph->vertices: %d\n", V);
    int *degree = (int *)calloc(V, sizeof(int));

    if (!degree) {
        fprintf(stderr, "Błąd alokacji pamięci dla stopni wierzchołków\n");
        exit(EXIT_FAILURE);
    }

    // Obliczanie stopni wierzchołków
    for (int i = 0; i < V; i++) {
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            degree[i]++;
        }
    }

    // Inicjalizacja macierzy Laplace'a zerami
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            laplacian[i][j] = 0.0;
        }
    }

    // Tworzenie macierzy Laplace'a
    printf("Jestem przed pętlą, adjacencyCount mamy taki: %d\n", graph->adjacencyCount);
    for (int i = 0; i < V; i++) {
        laplacian[i][i] = degree[i];
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            if(j == graph->adjacencyCount) return;
            int neighbor = graph->adjacency[j];
            laplacian[i][neighbor] = -1.0;
        }
    }
    // free(degree);
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


// Helper function for qsort
static int compare_doubles(const void *a, const void *b) {
    double diff = *(const double*)a - *(const double*)b;
    return (diff > 0) - (diff < 0);
}

double* compute_fiedler_vector(double **laplacian, int V) {
    // Initialize vectors
    double *x = (double *)calloc(V, sizeof(double));
    double *prev_x = (double *)calloc(V, sizeof(double));
    double *Ax = (double *)calloc(V, sizeof(double));
    
    if (!x || !prev_x || !Ax) {
        fprintf(stderr, "Memory allocation failed in compute_fiedler_vector\n");
        exit(1);
    }

    // Initialize x with random values
    for (int i = 0; i < V; i++) {
        x[i] = (double)rand() / RAND_MAX - 0.5;
    }

    // Power iteration method
    double epsilon = 1e-10;
    int max_iter = 1000;
    
    for (int iter = 0; iter < max_iter; iter++) {
        // Store previous vector
        memcpy(prev_x, x, V * sizeof(double));
        
        // Compute Ax
        mat_vec_multiply(laplacian, x, Ax, V);
        
        // Normalize
        double norm = 0.0;
        for (int i = 0; i < V; i++) {
            norm += Ax[i] * Ax[i];
        }
        norm = sqrt(norm);
        
        if (norm < epsilon) {
            break;
        }
        
        for (int i = 0; i < V; i++) {
            x[i] = Ax[i] / norm;
        }
        
        // Check convergence
        double diff = 0.0;
        for (int i = 0; i < V; i++) {
            diff += (x[i] - prev_x[i]) * (x[i] - prev_x[i]);
        }
        if (sqrt(diff) < epsilon) {
            break;
        }
    }
    
    free(prev_x);
    free(Ax);
    return x;
}


partition_t spectral_partition(graph_t *graph, int numParts) {
    int V = graph->vertices;

    double **laplacian = (double **)malloc(V * sizeof(double *));
    if (!laplacian) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < V; i++) {
        laplacian[i] = (double *)calloc(V, sizeof(double));
        if (!laplacian[i]) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }

    compute_laplacian(graph, laplacian);
    double *fiedler = compute_fiedler_vector(laplacian, V);

    // Sortowanie wartości Fiedlera
    double *sorted = (double *)malloc(V * sizeof(double));
    memcpy(sorted, fiedler, V * sizeof(double));
    qsort(sorted, V, sizeof(double), compare_doubles);

    // Wyznaczanie progów dla zadanej liczby części
    double *thresholds = NULL;
    if (numParts > 1) {
        thresholds = (double *)malloc((numParts - 1) * sizeof(double));
        if (!thresholds) {
            fprintf(stderr, "Memory allocation failed for thresholds\n");
            exit(1);
        }
        for (int i = 1; i < numParts; i++) {
            thresholds[i-1] = sorted[(i * V) / numParts];
        }
    }

    free(sorted);

    partition_t partition;
    partition.partition = (int *)malloc(V * sizeof(int));
    if (!partition.partition) {
        fprintf(stderr, "Memory allocation failed for partition\n");
        exit(1);
    }
    partition.numParts = numParts;

    for (int i = 0; i < V; i++) {
        int p = 0;
        while (p < numParts - 1 && fiedler[i] >= thresholds[p]) {
            p++;
        }
        partition.partition[i] = p;
    }

    if (numParts > 1) {
        free(thresholds);
    }
    // free(fiedler);
    // for (int i = 0; i < V; i++) {
    //     free(laplacian[i]);
    // }
    free(laplacian);

    return partition;
}



void evaluate_partition(graph_t *graph, partition_t partition, double margin) {
    int V = graph->vertices;
    int numParts = partition.numParts;

    // Tablica: liczba wierzchołków w każdej części
    int *partVertices = (int *)calloc(numParts, sizeof(int));
    if (!partVertices) {
        fprintf(stderr, "Memory allocation failed in evaluate_partition\n");
        exit(EXIT_FAILURE);
    }

    int cutEdges = 0;

    for (int i = 0; i < V; i++) {
        int part = partition.partition[i];
        if (part < 0 || part >= numParts) {
            printf("Błąd: Wierzchołek %d przypisany do nieistniejącej części %d\n", i, part);
            free(partVertices);
            return;
        }
        partVertices[part]++;
        if (i + 1 >= V || graph->edgeIndices[i] < 0) {
            continue;
        }
        // Liczenie przeciętych krawędzi
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1] && j < graph->adjacencyCount; j++) {
            int neighbor = graph->adjacency[j];
            if (neighbor >= 0 && neighbor < V) {
                if (partition.partition[i] != partition.partition[neighbor]) {
                    cutEdges++;
                }
            }
        }
    }

    cutEdges /= 2; // Bo liczymy każdą przeciętą krawędź dwa razy

    printf("\n===== Ewaluacja podziału =====\n");
    printf("Liczba przeciętych krawędzi: %d\n", cutEdges);

    for (int p = 0; p < numParts; p++) {
        double percentage = (partVertices[p] * 100.0) / V;
        printf("Część %d: %d wierzchołków (%.2f%%)\n", p, partVertices[p], percentage);
    }

    // Można dodać jeszcze sprawdzenie równomierności podziału
    double idealShare = 100.0 / numParts;
    int ok = 1;
    for (int p = 0; p < numParts; p++) {
        double percentage = (partVertices[p] * 100.0) / V;
        if (fabs(percentage - idealShare) > margin) {
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