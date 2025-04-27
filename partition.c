#include "partition.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Funkcja obliczająca macierz Laplace'a grafu
void compute_laplacian(graph_t *graph, double **laplacian) {
    int V = graph->vertices;
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
// double* compute_fiedler_vector(double **laplacian, int V) {
//     double *x = (double *)calloc(V, sizeof(double));  // Wektor początkowy
//     double *Ax = (double *)calloc(V, sizeof(double)); // Wektor wynikowy Ax
//     double *r = (double *)calloc(V, sizeof(double));  // Reszta Ax - λx
//     double alpha, beta, norm_r;

//     if (!x || !Ax || !r) {
//         fprintf(stderr, "Błąd alokacji pamięci w compute_fiedler_vector\n");
//         exit(EXIT_FAILURE);
//     }

//     // Inicjalizacja
//     for (int i = 0; i < V; i++) {
//         x[i] = 1.0; // Zaczynamy od wektora o wszystkich elementach równych 1
//     }

//     for (int iter = 0; iter < 1000; iter++) {
//         mat_vec_multiply(laplacian, x, Ax, V);

//         // Obliczanie normy wektora Ax
//         norm_r = norm(Ax, V);
//         if (norm_r == 0) {
//             fprintf(stderr, "Norma wektora wynosi 0 w iteracji %d\n", iter);
//             break;
//         }

//         // Reszta (błąd)
//         for (int i = 0; i < V; i++) {
//             r[i] = Ax[i] - x[i] * norm_r;
//         }

//         // Normalizacja reszty
//         norm_r = norm(r, V);
//         if (norm_r == 0) {
//             fprintf(stderr, "Norma reszty wynosi 0 w iteracji %d\n", iter);
//             break;
//         }
//         for (int i = 0; i < V; i++) {
//             r[i] /= norm_r;
//         }

//         // Obliczanie współczynnika alpha
//         alpha = 0;
//         for (int i = 0; i < V; i++) {
//             alpha += x[i] * r[i];
//         }

//         // Aktualizacja wektora x
//         for (int i = 0; i < V; i++) {
//             x[i] -= alpha * r[i];
//         }

//         // Współczynnik beta
//         beta = norm(x, V);
//         if (beta == 0) {
//             fprintf(stderr, "Norma wektora x wynosi 0 w iteracji %d\n", iter);
//             break;
//         }
//         for (int i = 0; i < V; i++) {
//             x[i] /= beta;
//         }

//         printf("Iteracja %d: norma wektora x = %.6f\n", iter, beta);
//     }

//     free(Ax);
//     free(r);

//     return x;
// }


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



partition_t spectral_partition(graph_t *graph) {
    int V = graph->vertices;
    
    // Allocate and initialize Laplacian matrix
    double **laplacian = (double **)malloc(V * sizeof(double *));
    for (int i = 0; i < V; i++) {
        laplacian[i] = (double *)calloc(V, sizeof(double));
        if (!laplacian[i]) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    
    // Compute Laplacian matrix
    compute_laplacian(graph, laplacian);
    
    // Compute Fiedler vector
    double *fiedler = compute_fiedler_vector(laplacian, V);
    
    // Find median for better balanced partitioning
    double *sorted = (double *)malloc(V * sizeof(double));
    memcpy(sorted, fiedler, V * sizeof(double));
    qsort(sorted, V, sizeof(double), compare_doubles);
    double median = sorted[V/2];
    free(sorted);
    
    // Create partition
    partition_t partition;
    partition.partition = (int *)malloc(V * sizeof(int));
    partition.numParts = 2;
    
    for (int i = 0; i < V; i++) {
        partition.partition[i] = (fiedler[i] >= median) ? 1 : 0;
    }
    
    // Cleanup
    free(fiedler);
    for (int i = 0; i < V; i++) {
        free(laplacian[i]);
    }
    free(laplacian);
    
    return partition;
}




// // Funkcja dzieląca graf na dwie części na podstawie wektora Fiedlera
// partition_t spectral_partition(graph_t *graph) {
//     int V = graph->vertices;
//     double **laplacian = (double **)malloc(V * sizeof(double *));
//     if (!laplacian) {
//         fprintf(stderr, "Błąd alokacji pamięci dla macierzy Laplace'a\n");
//         exit(EXIT_FAILURE);
//     }
//     for (int i = 0; i < V; i++) {
//         laplacian[i] = (double *)calloc(V, sizeof(double));
//         if (!laplacian[i]) {
//             for (int j = 0; j < i; j++) {
//                 free(laplacian[j]);
//             }
//             free(laplacian);
//             fprintf(stderr, "Błąd alokacji pamięci dla wiersza macierzy Laplace'a\n");
//             exit(EXIT_FAILURE);
//         }
//     }

//     compute_laplacian(graph, laplacian);
//     double *fiedler_vector = compute_fiedler_vector(laplacian, V);

//     partition_t partition;
//     partition.partition = (int *)malloc(V * sizeof(int));
//     if (!partition.partition) {
//         fprintf(stderr, "Błąd alokacji pamięci dla podziału grafu\n");
//         exit(EXIT_FAILURE);
//     }
//     partition.numParts = 2;

//     // Podział grafu w oparciu o znak wartości w wektorze Fiedlera
//     for (int i = 0; i < V; i++) {
//         partition.partition[i] = (fiedler_vector[i] >= 0) ? 1 : 0;
//     }

//     free(fiedler_vector);
//     for (int i = 0; i < V; i++) {
//         free(laplacian[i]);
//     }
//     free(laplacian);

//     return partition;
// }


void evaluate_partition(graph_t *graph, partition_t partition, double margin) {
    int cutEdges = 0; // Liczba przeciętych krawędzi
    int part0Vertices = 0; // Liczba wierzchołków w części 0
    int part1Vertices = 0; // Liczba wierzchołków w części 1

    // Iteracja po wszystkich wierzchołkach
    for (int i = 0; i < graph->vertices; i++) {
        if (partition.partition[i] == 0) {
            part0Vertices++;
        } else {
            part1Vertices++;
        }

        // Iteracja po sąsiadach wierzchołka `i`
        for (int j = graph->edgeIndices[i]; j < graph->edgeIndices[i + 1]; j++) {
            int neighbor = graph->adjacency[j];
            if (partition.partition[i] != partition.partition[neighbor]) {
                cutEdges++;
            }
        }
    }

    // Każda przecięta krawędź jest liczona dwukrotnie, więc dzielimy przez 2
    cutEdges /= 2;

    // Obliczenie procentowego udziału wierzchołków w każdej części
    double totalVertices = graph->vertices;
    double part0Percentage = (part0Vertices / totalVertices) * 100.0;
    double part1Percentage = (part1Vertices / totalVertices) * 100.0;

    // Wypisanie wyników
    printf("Liczba przeciętych krawędzi: %d\n", cutEdges);
    printf("Część 0: %d wierzchołków (%.2f%%)\n", part0Vertices, part0Percentage);
    printf("Część 1: %d wierzchołków (%.2f%%)\n", part1Vertices, part1Percentage);

    // Sprawdzenie, czy podział spełnia margines
    if (part0Percentage >= (50.0 - margin) && part0Percentage <= (50.0 + margin)) {
        printf("Podział spełnia margines %.2f%%.\n", margin);
    } else {
        printf("Podział NIE spełnia marginesu %.2f%%.\n", margin);
    }
}